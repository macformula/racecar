package main

import (
	"context"
	"github.com/macformula/hil/canlink"
	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
	vehcan "mac-daq/generated"
)

func main() {
	conn, err := socketcan.DialContext(context.Background(), "can", "van0")
	if err != nil {
		panic(err)
	}

	logger, err := zap.NewDevelopment()
	daq := NewDaqHandler(vehcan.Messages(), logger)

	manager := canlink.NewBusManager(logger, &conn)
	manager.Register(daq)
	manager.Start(context.Background())

	for {
	}
}
