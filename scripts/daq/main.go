package main

import (
	"context"
	"fmt"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
)

func main() {
	// connect to CAN interfaces
	can0, err := socketcan.DialContext(context.Background(), "can", "can0")
	if err != nil {
		panic(err)
	}

	can1, err := socketcan.DialContext(context.Background(), "can", "can1")
	if err != nil {
		panic(err)
	}

	// set up logging and telemetry handlers
	logger, _ := zap.NewDevelopment()
	telemetry, err := NewTelemetryHandler("./can_cache.sqlite")
	if err != nil {
		panic(err)
	}

	daqCan0 := NewDaqHandler(telemetry, "can0", logger)
	daqCan1 := NewDaqHandler(telemetry, "can1", logger)

	// set up CAN bus managers and start them
	manager0 := canlink.NewBusManager(logger, &can0)
	manager0.Register(daqCan0)
	manager0.Start(context.Background())

	manager1 := canlink.NewBusManager(logger, &can1)
	manager1.Register(daqCan1)
	manager1.Start(context.Background())

	// call telemetry upload once after one second
	uploadTimer := time.NewTimer(time.Second)

	for {
		select {
		case <-uploadTimer.C:
			// invoke the upload hook, whose current implementation returns without uploading
			err = telemetry.Upload()
			if err != nil {
				fmt.Printf("failed to upload telemetry data: %v\n", err)
			}
		}
	}
}
