package main

// Pi reads CAN frames, decodes them, and writes the decoded signals to a CSV file
// optionally sends selected telemetry to InfluxDB when configured (for Grafana dashboard)

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"sync"
	"syscall"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
)

func main() {
	// Initialize logger
	logger, err := zap.NewDevelopment()
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	// Run DAQ
	if err := run(logger, os.Args[1:]); err != nil && !errors.Is(err, flag.ErrHelp) {
		logger.Error("DAQ stopped", zap.Error(err))
		_ = logger.Sync()
		os.Exit(1)
	}
	_ = logger.Sync()
}

// Returns the CAN interfaces and whether to decode all known messages
func parseOptions(args []string) ([]string, bool, error) {
	options := flag.NewFlagSet("mac-daq", flag.ContinueOnError)
	interfaces := options.String("interfaces", "can0,can1", "CAN interfaces, separated by spaces or commas")
	all := options.Bool("all", false, "decode all known messages instead of legacy target IDs")
	// Accept the python logger's space-separated --interfaces arguments
	var normalized []string
	for i := 0; i < len(args); i++ {
		normalized = append(normalized, args[i])
		if args[i] != "--interfaces" && args[i] != "-interfaces" {
			continue
		}
		end := i + 1
		for end < len(args) && !strings.HasPrefix(args[end], "-") {
			end++
		}
		if end == i+1 {
			return nil, false, fmt.Errorf("--interfaces requires at least one interface")
		}
		normalized = append(normalized, strings.Join(args[i+1:end], ","))
		i = end - 1
	}
	if err := options.Parse(normalized); err != nil {
		return nil, false, err
	}
	if options.NArg() != 0 {
		return nil, false, fmt.Errorf("unexpected arguments: %v", options.Args())
	}
	names := strings.Split(*interfaces, ",")
	seen := make(map[string]bool)
	for _, name := range names {
		if name == "" || seen[name] {
			return nil, false, fmt.Errorf("interfaces must be nonempty and unique")
		}
		seen[name] = true
	}
	return names, *all, nil
}

// Run DAQ loop, reading CAN frames, decoding them, writing CSV, and optionally sending telemetry to InfluxDB
func run(logger *zap.Logger, args []string) (err error) {
	interfaces, all, err := parseOptions(args)
	if err != nil {
		return err
	}
	if err := loadEnv(".env"); err != nil {
		logger.Warn("could not fully load .env; continuing with available variables", zap.Error(err))
	}
	// Create frame recorder for CSV output
	recorder, err := newFrameRecorder("logs", all)
	if err != nil {
		return err
	}
	defer func() { err = errors.Join(err, recorder.Close()) }()
	config, configErr := influxFromEnv()
	writer := newInfluxWriter(config, logger)
	if writer != nil {
		defer writer.Close()
	} else {
		logger.Warn("InfluxDB disabled; check INFLUX_URL, INFLUX_TOKEN, INFLUX_ORG, INFLUX_BUCKET", zap.Error(configErr))
	}

	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	// Create a channel for timestamped CAN frames and a channel for read errors
	frames := make(chan canlink.TimestampedFrame, 128)
	readErrors := make(chan error, len(interfaces))
	var readers sync.WaitGroup
	var receivers []*socketcan.Receiver
	defer func() {
		stop()
		// Closing receivers unblocks reads so the readers can finish.
		for _, receiver := range receivers {
			_ = receiver.Close()
		}
		readers.Wait()
	}()
	for _, name := range interfaces {
		// Open CAN interface and start reading frames
		conn, err := socketcan.DialContext(ctx, "can", name)
		if err != nil {
			return fmt.Errorf("open %s: %w", name, err)
		}
		receiver := socketcan.NewReceiver(conn)
		receivers = append(receivers, receiver)
		readers.Add(1)
		// Start goroutine to read frames from receiver and send them to frames channel
		go func() {
			defer readers.Done()
			for receiver.Receive() {
				frame := canlink.TimestampedFrame{Frame: receiver.Frame(), Time: time.Now()}
				select {
				// Send the frame or stop if the context is canceled.
				case frames <- frame:
				case <-ctx.Done():
					return
				}
			}
			if ctx.Err() == nil {
				readErrors <- fmt.Errorf("read %s stopped: %v", name, receiver.Err())
			}
		}()
	}
	// Start main loop to process frames and write to CSV and InfluxDB
	logger.Info("DAQ: recording decoded CSV", zap.String("file", recorder.file.Name()), zap.Strings("interfaces", interfaces))
	for {
		select {
		case <-ctx.Done():
			// Stop reading frames and exit
			return nil
		case err := <-readErrors:
			// Stop reading frames and exit with error
			return err
		case frame := <-frames:
			// Decode the frame, write to CSV, and queue optional InfluxDB telemetry.
			signals, err := recorder.record(frame)
			if err != nil {
				return err
			}
			if writer != nil {
				writer.enqueue(telemetryLines(frame, signals))
			}
		}
	}
}
