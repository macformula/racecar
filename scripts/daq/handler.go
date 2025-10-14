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
	md DbcMessagesDescriptor
	l  *zap.Logger
}

func NewDaqHandler(md DbcMessagesDescriptor, l *zap.Logger) *DataAcquisitionHandler {
	return &DataAcquisitionHandler{
		md: md,
		l:  l,
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
			d.Enqueue(receivedFrame)
		default:
		}
	}
}

func (d *DataAcquisitionHandler) Enqueue(frame canlink.TimestampedFrame) error {
	return nil
}

func (d *DataAcquisitionHandler) Send() error {
	return nil
}
