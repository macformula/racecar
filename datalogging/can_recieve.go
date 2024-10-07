package main

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"syscall"

	"github.com/brutella/can"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: go run can_reciever.go <INTERFACE NAME>")
		return
	}
	var interfce_name = os.Args[1]

	intrfce, err := net.InterfaceByName(interfce_name)

	if err != nil {
		fmt.Printf("Could not connect to %s. Check that you have added it properly.\n", interfce_name)
		os.Exit(1)
	}

	// Establish connection to CAN bus
	connection, _ := can.NewReadWriteCloserForInterface(intrfce)
	can_bus := can.NewBus(connection)
	can_bus.SubscribeFunc(func(frm can.Frame) {
		printCANFrame(interfce_name, frm)
	})
	can_bus.ConnectAndPublish()

	// Intercepts interrupt to perform cleanup actions before exiting program
	signalChan := make(chan os.Signal, 1)
	signal.Notify(signalChan, os.Interrupt)
	signal.Notify(signalChan, syscall.SIGTERM)

	go func() {
		<-signalChan
		fmt.Println("\nExiting...")
		can_bus.Disconnect()
		os.Exit(1)
	}()
}

// Formats and outputs CAN frames to match output of candump
func printCANFrame(intrfce_name string, frm can.Frame) {
	dataToPrint := frm.Data[:frm.Length]
	fmt.Printf("%s   %-4X   [%d]   % -42X\n", intrfce_name, frm.ID, frm.Length, dataToPrint)
}
