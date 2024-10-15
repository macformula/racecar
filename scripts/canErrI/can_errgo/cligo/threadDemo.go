package main

import (
	"fmt"
	"time"
)



func printHello(seconds float32){
	for {
	time.Sleep(time.Duration(seconds * 1000) * time.Millisecond)
	fmt.Printf("Hello")
	}
}

func printWorld(seconds float32){
	for {
	time.Sleep(time.Duration(seconds * 1000) * time.Millisecond)
	fmt.Printf("World\n")
	}
}

func main(){
	go printHello(4)
	go printWorld(1)
	for{	
	}
}
