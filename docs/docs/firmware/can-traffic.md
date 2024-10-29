
# Calculating CAN traffic

In order to calculate the CAN load, we must declare a few variables and their importance within the CAN protocol.

## Baud (kbaud)

The baud rate is the rate at which data is transfered in a communication channel (like CAN). For example, 9600 Baud rate is 9600 bits per second

## Frequency (Hz)

Frequency is the number of messages transmitted per second for each message type. Different messages can have different frequencies.

This variation affects the total bits transferred and, in turn, the bus load. To calculate bits per second, consider each message’s frequency and whether their lengths differ.

## Total Data Per Cycle

Total data per cycle includes all the bits transmitted in one CAN message. This includes the payload and overhead bits like the start of frame, CRC, acknowledgment, control field, etc.

## The bus load is determined by

$$
\text{Total Bits Per Second} = \sum_{i=1}^n \left(\text{Frequency}_i \times \text{Message Length}_i\right)
$$

$$
\text{Bus Load (%)} = \frac{\text{Total Bits per Second}}{\text{Baud Rate}} \times 100
$$

## Sample calculation

**Given**:

Battery Status: Frequency = 100Hz, Length = 400 bits

Motor Control: Frequency = 50Hz, Length = 100 bits

Baud Rate: 500 kbaud (500,000 bits transferred per second)

$$
\text{Total Bits Per Second} = (100 \times 400) + (50 \times 100) = 40,000 + 5,000 = 45,000 \text{ bits per second}
$$

$$
\text{Bus Load} = \left(\frac{45,000}{500,000}\right) \times 100 = 9\%
$$
