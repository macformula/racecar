package main

// Remote telemetry to InfluxDB for Grafana dashboarding
// This is a separate file from logger.go to seperate CSV logging from telemetry, which is optional and may be disabled by environment variables
// Configure INFLUX_URL, INFLUX_TOKEN, INFLUX_ORG, and INFLUX_BUCKET environment variables to enable telemetry. If any are missing, telemetry is disabled and only local CSV logging occurs.

import (
	"fmt"
	"io"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"

	"go.uber.org/zap"
)

// loadEnv reads plain or quoted KEY=value entries, comments, and optional export prefixes.
// Like the Python logger's load_dotenv(override=True), file values override exported values.
func loadEnv(path string) error {
	data, err := os.ReadFile(path)
	if os.IsNotExist(err) {
		return nil
	}
	if err != nil {
		return err
	}
	for index, line := range strings.Split(string(data), "\n") {
		line = strings.TrimSpace(line)
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}
		key, value, found := strings.Cut(strings.TrimPrefix(line, "export "), "=")
		if !found {
			return fmt.Errorf(".env line %d: expected KEY=value", index+1)
		}
		key, value = strings.TrimSpace(key), strings.TrimSpace(value)
		if strings.HasPrefix(value, "\"") || strings.HasPrefix(value, "'") {
			end := strings.LastIndexByte(value, value[0])
			if end == 0 {
				return fmt.Errorf(".env line %d: unclosed quote", index+1)
			}
			value = value[:end+1]
			if value[0] == '\'' {
				value = value[1 : len(value)-1]
			} else {
				value, err = strconv.Unquote(value)
				if err != nil {
					return fmt.Errorf(".env line %d: invalid quoted value", index+1)
				}
			}
		} else {
			value = strings.TrimSpace(strings.SplitN(value, " #", 2)[0])
		}
		if err := os.Setenv(key, value); err != nil {
			return fmt.Errorf(".env line %d: invalid key", index+1)
		}
	}
	return nil
}

type influxConfig struct{ endpoint, token string }

// Build the InfluxDB write endpoint from environment variables.
func influxFromEnv() (influxConfig, error) {
	values := []string{os.Getenv("INFLUX_URL"), os.Getenv("INFLUX_TOKEN"), os.Getenv("INFLUX_ORG"), os.Getenv("INFLUX_BUCKET")}
	// Incomplete credentials select local-only logging, like the Python logger.
	for _, value := range values {
		if value == "" {
			return influxConfig{}, nil
		}
	}
	u, err := url.Parse(values[0])
	if err != nil || u.Host == "" || (u.Scheme != "https" && u.Scheme != "http") || u.User != nil || u.RawQuery != "" || u.Fragment != "" {
		return influxConfig{}, fmt.Errorf("INFLUX_URL must be an http(s) base URL without credentials, query, or fragment")
	}
	u.Path = strings.TrimRight(u.Path, "/") + "/api/v2/write"
	u.RawQuery = url.Values{"org": {values[2]}, "bucket": {values[3]}, "precision": {"ns"}}.Encode()
	return influxConfig{u.String(), values[1]}, nil
}

// Send a batch of line protocol data to InfluxDB.
func writeInflux(client *http.Client, config influxConfig, payload string) error {
	req, err := http.NewRequest(http.MethodPost, config.endpoint, strings.NewReader(payload))
	if err != nil {
		return err
	}
	req.Header.Set("Authorization", "Token "+config.token)
	req.Header.Set("Content-Type", "text/plain; charset=utf-8")
	response, err := client.Do(req)
	if err != nil {
		return fmt.Errorf("InfluxDB unreachable")
	}
	defer response.Body.Close()
	_, _ = io.Copy(io.Discard, io.LimitReader(response.Body, 4096))
	if response.StatusCode != http.StatusNoContent {
		return fmt.Errorf("InfluxDB HTTP %d", response.StatusCode)
	}
	return nil
}

type influxWriter struct {
	config  influxConfig
	client  *http.Client
	logger  *zap.Logger
	queue   chan string
	stop    chan struct{}
	done    chan struct{}
	warning sync.Once
}

// Start the telemetry writer when InfluxDB is configured.
func newInfluxWriter(config influxConfig, logger *zap.Logger) *influxWriter {
	if config.endpoint == "" {
		return nil
	}
	writer := &influxWriter{
		config: config, logger: logger,
		stop: make(chan struct{}), done: make(chan struct{}),
		queue: make(chan string, 1000),
		client: &http.Client{
			Timeout:       3 * time.Second,
			CheckRedirect: func(*http.Request, []*http.Request) error { return http.ErrUseLastResponse },
		},
	}
	go writer.run()
	return writer
}

// Log the first telemetry delivery failure.
func (w *influxWriter) warn(err error) {
	w.warning.Do(func() { w.logger.Warn("telemetry unavailable; local CSV logging continues", zap.Error(err)) })
}

// Queue telemetry without blocking CAN acquisition.
func (w *influxWriter) enqueue(payload string) {
	if payload == "" {
		return
	}
	// A slow connection must not block CAN acquisition or local CSV writes.
	select {
	case w.queue <- payload:
	default:
		w.warn(fmt.Errorf("InfluxDB buffer full; telemetry dropped"))
	}
}

// Batch queued telemetry and flush it periodically or on shutdown
func (w *influxWriter) run() {
	defer close(w.done)
	defer w.client.CloseIdleConnections()
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()
	var batch []string
	flush := func() {
		if len(batch) == 0 {
			return
		}
		if err := writeInflux(w.client, w.config, strings.Join(batch, "\n")); err != nil {
			w.warn(err)
		}
		// This is best-effort output: failed batches are dropped, not replayed.
		batch = nil
	}
	for {
		// The caller has stopped producing frames before requesting shutdown.
		select {
		case <-w.stop:
			for len(w.queue) > 0 {
				batch = append(batch, <-w.queue)
			}
			flush()
			return
		default:
		}
		select {
		case <-w.stop:
		case payload := <-w.queue:
			batch = append(batch, payload)
			// Send after 100 frame payloads or the one-second timer, whichever comes first.
			if len(batch) >= 100 {
				flush()
			}
		case <-ticker.C:
			flush()
		}
	}
}

// Close finishes the current request and makes one final bounded write of buffered data.
func (w *influxWriter) Close() {
	close(w.stop)
	<-w.done
}
