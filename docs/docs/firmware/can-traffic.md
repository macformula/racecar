---
title: Calculating CAN traffic on a bus
---



In order to calculate the CAN load, we must declare a few variables and their importance within the CANbus protocol. 

1. Baud: (kbaud)

The baud rate is the rate at which data is transfered in a communication channel (like CAN). For example, 9600 Baud rate is 9600 bits per second

2. Frequency (Hz)

Frequency in the CANbus refers to how many CAN frames (or messages) are transmitted per second. So the greater the frequency, the more bits transferred, increasing the bus load. 

3. Total Data Per Cycle

Total data per cycle includes all the bits transmitted in one CAN message. This includes not only the payload but overhead bits like start of frame, CRC, acknowledgement, control field etc. 



## The bus load is determined by: 

![Bus Load Formula](https://latex.codecogs.com/png.latex?\text{Bus%20Load%20(\%)}%20=%20\frac{\text{Number%20of%20Bits%20per%20Frame}%20\times%20\text{Number%20of%20Frames%20per%20Second}}{\text{CAN%20Speed}}%20\times%20100)

![Bus Load Formula](https://latex.codecogs.com/png.latex?\text{Bus%20Load%20(\%)}%20=%20\frac{\text{Total%20Bits%20per%20Second}}{\text{CAN%20Speed}}%20\times%20100)

## Sample calculation: 

Total data/cycle: 1524 (bits per cycle)

Frequency: 200 (cycles per second)

Baud Rate: 500 kbaud (500,000 bits transferred per second)

![Solved Bus Load Formula](https://latex.codecogs.com/png.latex?\text{Bus%20Load%20(\%)}%20=%20\frac{1,524%20\times%20200}{500,000}%20\times%20100%20=%2060.96\%)

