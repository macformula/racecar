package main

import (
	"context"
	vehcan "mac-daq/generated"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
)

func main() {
	conn, err := socketcan.DialContext(context.Background(), "can", "van0")
	if err != nil {
		panic(err)
	}

	logger, _ := zap.NewDevelopment()
	telemetry, err := NewTelemetryHandler()
	if err != nil {
		panic(err)
	}

	daq := NewDaqHandler(vehcan.Messages(), telemetry, logger)

	manager := canlink.NewBusManager(logger, &conn)
	manager.Register(daq)
	manager.Start(context.Background())

	for {
	}
}
