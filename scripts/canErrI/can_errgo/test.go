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


func bytesToUint64(b []byte) uint64 {
	var result uint64
	for i := 0; i < len(b) && i < 8; i++ {
		result |= uint64(b[i]) << (8 * i)
	}
	return result
}

func main() {
	// Create a raw CAN socket using the unix package
	var sock int;
	var err error;
	var mask uint64 = ^uint64(0)
	
	sock, err = unix.Socket(unix.AF_CAN, unix.SOCK_RAW, unix.CAN_RAW)
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
		predata := buf[8:]
		data := bytesToUint64(buf[8:])  & mask                                         // CAN data bytes (8 bytes)
		fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, predata)	
		fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, data)
		for k := 0; k < 64; k++ { // Iterate through all 64 bits
			if data&(1<<k) != 0 { // Check if the k-th bit is set
				fmt.Printf("error%d occurred\n", k)
			}
		}
	}
}


