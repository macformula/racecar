package main

import (
	"context"
	"fmt"
	vehcan "mac-daq/generated"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
)

func main() {
	can0, err := socketcan.DialContext(context.Background(), "can", "can0")
	if err != nil {
		panic(err)
	}

	can1, err := socketcan.DialContext(context.Background(), "can", "can1")
	if err != nil {
		panic(err)
	}

	logger, _ := zap.NewDevelopment()

	telemetry, err := NewTelemetryHandler("./can_cache.sqlite")
	if err != nil {
		panic(err)
	}

	daqCan0 := NewDaqHandler(vehcan.Messages(), telemetry, "can0", logger)
	daqCan1 := NewDaqHandler(vehcan.Messages(), telemetry, "can1", logger)

	manager0 := canlink.NewBusManager(logger, &can0)
	manager0.Register(daqCan0)
	manager0.Start(context.Background())

	manager1 := canlink.NewBusManager(logger, &can1)
	manager1.Register(daqCan1)
	manager1.Start(context.Background())

	uploadTimer := time.NewTimer(time.Second)
	for {
		select {
		case <-uploadTimer.C:
			err = telemetry.Upload()
			if err != nil {
				fmt.Printf("failed to upload telemetry data: %v\n", err)
			}
		}
	}
}
