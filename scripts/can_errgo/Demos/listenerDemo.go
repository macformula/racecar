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

func IntPow(n, m int) int {
    if m == 0 {
        return 1
    }

    if m == 1 {
        return n
    }

    result := n
    for i := 2; i <= m; i++ {
        result *= n
    }
    return result
}


func main() {
	// Create a raw CAN socket using the unix package
	var sock int;
	var err error;
	var mask uint64 = ^uint64(0) // All 64 bits set to 1
	
	fmt.Printf("%d",mask)

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
		data := buf[8:] //& mask                                          // CAN data bytes (8 bytes)
		fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, data)	
		for i :=len(data) - 1; i >= 0; i--{
			bit := 1
			//fmt.Printf("CAN ID: 0x%X, Data: %08b\n",canID, data[i])
			for k :=0; k < 8; k++{
				//fmt.Printf("%d\n", IntPow(2,k))
				if (data[i] & byte(bit) != 0){
					fmt.Printf("error%d occured\n", (7-i)*8 + k)
			
		}
		bit*=2
	}

		// Print the CAN message ID and data
		//fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, data)
	}}
}

