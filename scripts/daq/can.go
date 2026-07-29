package main

import (
	"fmt"
	"time"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	"go.einride.tech/can/pkg/generated"
	"go.uber.org/zap"
)

const (
	// MAX_FRAME_INTERVAL is the milliseconds between using identical CAN frames
	MAX_FRAME_INTERVAL = 100
)

type DbcMessagesDescriptor interface {
	UnmarshalFrame(f can.Frame) (generated.Message, error)
}

type DataAcquisitionHandler struct {
	md              DbcMessagesDescriptor
	telemetry       *TelemetryHandler
	busName         string
	l               *zap.Logger
	frameArrivalMap map[uint32]time.Time
}

func NewDaqHandler(md DbcMessagesDescriptor, telemetry *TelemetryHandler, busName string, l *zap.Logger) *DataAcquisitionHandler {
	return &DataAcquisitionHandler{
		md:              md,
		l:               l,
		telemetry:       telemetry,
		busName:         busName,
		frameArrivalMap: map[uint32]time.Time{},
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

			fmt.Printf("daq: received frame: %s\n", receivedFrame.Frame.String())
			lastSeen, exists := d.frameArrivalMap[receivedFrame.Frame.ID]

			// Frame was already seen less than MAX_FRAME_INTERVAL ms ago, ignore it
			if exists && time.Now().Sub(lastSeen).Milliseconds() < MAX_FRAME_INTERVAL {
				break
			}

			err := d.telemetry.Enqueue(receivedFrame, d.busName)
			if err != nil {
				fmt.Printf("daq: failed to enqueue frame: %s\n", err.Error())
			}
		default:
		}
	}
}
