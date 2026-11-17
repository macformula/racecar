package main

import (
	"strings"
	"testing"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
)

func TestTelemetrySpeedUsesDashCommand(t *testing.T) {
	for _, tc := range []struct {
		id   uint32
		want bool
	}{{230, true}, {775, false}} {
		frame := canlink.TimestampedFrame{Frame: can.Frame{ID: tc.id}, Time: time.Unix(1, 0)}
		got := telemetryLines(frame, []decodedSignal{{name: "Speed", value: 10}})
		if strings.Contains(got, "signal=Speed") != tc.want {
			t.Errorf("frame %d: unexpected telemetry %q", tc.id, got)
		}
	}
}
