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
		fmt.Println("Usage: go run can_receiver.go <INTERFACE NAME>")
		os.Exit(2)
	}
	var interfaceName = os.Args[1]

	canInterface, err := net.InterfaceByName(interfaceName)

	if err != nil {
		fmt.Printf("Could not connect to %s. Check that you have added it properly.\n", interfaceName)
		os.Exit(1)
	}

	// Establish connection to CAN bus
	connection, err := can.NewReadWriteCloserForInterface(canInterface)

	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	can_bus := can.NewBus(connection)
	can_bus.SubscribeFunc(func(frm can.Frame) {
		printCANFrame(interfaceName, frm)
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
func printCANFrame(interfaceName string, frm can.Frame) {
	dataToPrint := frm.Data[:frm.Length]
	fmt.Printf("%s   %-4X   [%d]   % -42X\n", interfaceName, frm.ID, frm.Length, dataToPrint)
}
