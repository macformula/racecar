package main

// Local logging: embedded DBC decoding and CSV output.

import (
	"embed"
	"encoding/csv"
	"errors"
	"fmt"
	"os"
	"strconv"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	"go.einride.tech/can/pkg/dbc"
)

// Bundle the DBCs so deployment only needs the compiled executable.
//
//go:embed dbc/*.dbc
var dbcFiles embed.FS

type decodedSignal struct {
	name, unit, text string
	value            float64
}

type decodedMessage struct {
	*dbc.MessageDef
	choices map[string]map[float64]string
}

type frameKey struct {
	id       uint32
	extended bool
}

type decoder map[frameKey]*decodedMessage

// Load message definitions and signal labels from the embedded DBCs.
func newDecoder() (decoder, error) {
	result := make(decoder)
	for _, name := range []string{"dbc/veh.dbc", "dbc/pt.dbc"} {
		data, err := dbcFiles.ReadFile(name)
		if err != nil {
			return nil, err
		}
		// Firmware defines LvDcdc, but the unchanged legacy DAQ DBC omits it.
		if name == "dbc/veh.dbc" {
			data = append(data, []byte(`
BO_ 214 LvDcdc: 6 LVC
 SG_ LvBatteryVoltage : 0|16@1+ (0.001,0) [0|30] "V" RPI
 SG_ BusVoltage : 16|16@1+ (0.001,0) [0|24] "V" RPI
 SG_ BusCurrent : 32|16@1+ (0.001,0) [0|15] "A" RPI
`)...)
		}
		parser := dbc.NewParser(name, data)
		if err := parser.Parse(); err != nil {
			return nil, fmt.Errorf("parse %s: %w", name, err)
		}
		for _, def := range parser.File().Defs {
			if message, ok := def.(*dbc.MessageDef); ok {
				for _, signal := range message.Signals {
					if signal.IsMultiplexed || signal.IsMultiplexerSwitch || signal.Size == 0 || signal.Size > 64 || signal.StartBit >= 64 {
						return nil, fmt.Errorf("unsupported DBC signal %s.%s", message.Name, signal.Name)
					}
				}
				result[frameKey{id: message.MessageID.ToCAN(), extended: message.MessageID.IsExtended()}] = &decodedMessage{message, make(map[string]map[float64]string)}
			}
		}
		// Attach enum labels after all message definitions have been indexed.
		for _, def := range parser.File().Defs {
			switch def := def.(type) {
			case *dbc.ValueDescriptionsDef:
				message := result[frameKey{id: def.MessageID.ToCAN(), extended: def.MessageID.IsExtended()}]
				if message == nil {
					continue
				}
				choices := make(map[float64]string)
				for _, choice := range def.ValueDescriptions {
					choices[choice.Value] = choice.Description
				}
				message.choices[string(def.SignalName)] = choices
			case *dbc.SignalValueTypeDef:
				if def.SignalValueType != dbc.SignalValueTypeInt {
					return nil, fmt.Errorf("unsupported floating-point DBC signal %s", def.SignalName)
				}
			}
		}
	}
	return result, nil
}

// Decode a CAN frame into its message name and scaled signals.
func (d decoder) decode(frame can.Frame) (string, []decodedSignal) {
	message := d[frameKey{id: frame.ID, extended: frame.IsExtended}]
	// Remote requests have no signal payload; unknown or short frames are skipped.
	if frame.IsRemote || message == nil || uint64(frame.Length) < message.Size {
		return "", nil
	}
	var signals []decodedSignal
	for _, signal := range message.Signals {
		// Current vehicle and powertrain DBCs contain integer, non-multiplexed signals.
		var bits uint64
		if signal.IsBigEndian {
			bits = frame.Data.UnsignedBitsBigEndian(uint8(signal.StartBit), uint8(signal.Size))
		} else {
			bits = frame.Data.UnsignedBitsLittleEndian(uint8(signal.StartBit), uint8(signal.Size))
		}
		raw := float64(bits)
		// Extend the signal's sign bit before applying its DBC scale and offset.
		signed := int64(bits<<(64-signal.Size)) >> (64 - signal.Size)
		if signal.IsSigned {
			raw = float64(signed)
		}
		text := message.choices[string(signal.Name)][raw]
		// Preserve exact 64-bit identifiers in CSV; float64 would round them.
		if text == "" && signal.Size > 53 && signal.Factor == 1 && signal.Offset == 0 {
			if signal.IsSigned {
				text = strconv.FormatInt(signed, 10)
			} else {
				text = strconv.FormatUint(bits, 10)
			}
		}
		// DBC limits describe expected values; never clamp real fault readings.
		signals = append(signals, decodedSignal{
			name: string(signal.Name), unit: signal.Unit,
			value: raw*signal.Factor + signal.Offset, text: text,
		})
	}
	return string(message.Name), signals
}

type frameRecorder struct {
	decoder decoder
	file    *os.File
	csv     *csv.Writer
	all     bool
}

// Create a CSV file and decoder for incoming frames.
func newFrameRecorder(directory string, all bool) (*frameRecorder, error) {
	decoder, err := newDecoder()
	if err != nil {
		return nil, err
	}
	if err := os.MkdirAll(directory, 0755); err != nil {
		return nil, err
	}
	file, err := os.CreateTemp(directory, "can_log_"+time.Now().Format("20060102_150405")+"_*.csv")
	if err != nil {
		return nil, err
	}
	writer := csv.NewWriter(file)
	if err := writer.Write([]string{"timestamp_epoch_s", "message_id_hex", "message_name", "signal_name", "value", "unit"}); err != nil {
		file.Close()
		return nil, err
	}
	writer.Flush()
	if err := writer.Error(); err != nil {
		file.Close()
		return nil, err
	}
	return &frameRecorder{decoder: decoder, file: file, csv: writer, all: all}, nil
}

// Decode selected frames, write their signals to CSV, and return the signals.
func (r *frameRecorder) record(frame canlink.TimestampedFrame) ([]decodedSignal, error) {
	if err := frame.Frame.Validate(); err != nil {
		return nil, err
	}
	if !r.all && !targetIDs[frame.Frame.ID] {
		return nil, nil
	}
	name, signals := r.decoder.decode(frame.Frame)
	timestamp := strconv.FormatFloat(float64(frame.Time.UnixNano())/1e9, 'f', 9, 64)
	id := fmt.Sprintf("0x%X", frame.Frame.ID)
	for _, signal := range signals {
		value := strconv.FormatFloat(signal.value, 'g', -1, 64)
		if signal.text != "" {
			value = signal.text
		}
		if err := r.csv.Write([]string{timestamp, id, name, signal.name, value, signal.unit}); err != nil {
			return nil, err
		}
	}
	// Flush each decoded frame rather than waiting for the CSV buffer to fill.
	r.csv.Flush()
	return signals, r.csv.Error()
}

// Flush and close the CSV file.
func (r *frameRecorder) Close() error {
	r.csv.Flush()
	return errors.Join(r.csv.Error(), r.file.Close())
}
