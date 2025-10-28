package main

import (
	"fmt"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	"go.einride.tech/can/pkg/generated"
	"go.uber.org/zap"
)

type DbcMessagesDescriptor interface {
	UnmarshalFrame(f can.Frame) (generated.Message, error)
}

type DataAcquisitionHandler struct {
	md        DbcMessagesDescriptor
	l         *zap.Logger
	telemetry *TelemetryHandler
}

func NewDaqHandler(md DbcMessagesDescriptor, telemetry *TelemetryHandler, l *zap.Logger) *DataAcquisitionHandler {
	return &DataAcquisitionHandler{
		md:        md,
		l:         l,
		telemetry: telemetry,
	}
}

func (d *DataAcquisitionHandler) Name() string {
	return fmt.Sprintf("DAQ Handler")
}

func (d *DataAcquisitionHandler) Handle(broadcastChan chan canlink.TimestampedFrame, stopChan chan struct{}) error {
	for {
		select {
		case <-stopChan:
			d.l.Info("stopping handle")
		case receivedFrame := <-broadcastChan:
			// CAN frame received, parse it and queue it for transmission or file caching

			// We wont do any additional parsing for now, but it might be useful later
			/*
			 * msg, err := d.md.UnmarshalFrame(receivedFrame.Frame)
			 * if err != nil {
			 *	return errors.Wrap(err, "daq: handle:")
			 * }
			 */

			fmt.Printf("daq: received frame at %s\n", receivedFrame.Time.String())
			err := d.telemetry.Enqueue(receivedFrame)
			if err != nil {
				fmt.Printf("daq: failed to enqueue frame: %s\n", err.Error())
			}
		default:
		}
	}
}
