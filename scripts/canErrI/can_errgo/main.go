package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"net"
	"golang.org/x/sys/unix"
)

const (
	canInterface = "vcan0"  // Use the virtual CAN interface
	canFrameSize = 16       // CAN frame size (for a standard CAN frame)
)

func main() {
	// Create a raw CAN socket using the unix package
	sock, err := unix.Socket(unix.AF_CAN, unix.SOCK_RAW, unix.CAN_RAW)
	if err != nil {
		log.Fatalf("Error creating CAN socket: %v", err)
	}
	defer unix.Close(sock)

	// Get the index of the interface (vcan0)
	ifi, err := net.InterfaceByName(canInterface)
	if err != nil {
		log.Fatalf("Error getting interface index: %v", err)
	}

	// Bind the socket to the CAN interface
	addr := &unix.SockaddrCAN{Ifindex: ifi.Index}
	if err := unix.Bind(sock, addr); err != nil {
		log.Fatalf("Error binding CAN socket: %v", err)
	}

	// Create a buffer to hold incoming CAN frames
	buf := make([]byte, canFrameSize)

	fmt.Println("Listening for CAN messages on", canInterface)

	for {
		// Receive CAN message
		_, err := unix.Read(sock, buf)
		if err != nil {
			log.Printf("Error receiving CAN message: %v", err)
			continue
		}

		// Extract CAN ID and data
		canID := binary.LittleEndian.Uint32(buf[0:4]) & 0x1FFFFFFF // 29-bit CAN ID (masked)
		data := buf[8:]                                           // CAN data bytes (8 bytes)

		// Print the CAN message ID and data
		fmt.Printf("CAN ID: 0x%X, Data: % X\n", canID, data)
	}
}

