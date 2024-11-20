
#!/bin/bash

# Infinite loop
while true
do
    # Send CAN message
    cansend vcan0 FFF#0000000000000001
    sleep 1
    cansend vcan0 FFF#0000000000000002
    sleep 1
    cansend vcan0 FFF#0000000000000004
    sleep 1
    cansend vcan0 FFF#0000000000000008
    sleep 1
    cansend vcan0 FFF#0000000000000010
    sleep 1
    cansend vcan0 FFF#0000000000000020
    sleep 1
    cansend vcan0 FFF#0000000000000040
    sleep 1
    cansend vcan0 FFF#0000000000000080
    sleep 1
    cansend vcan0 FFF#0000000000000100
    sleep 1
    cansend vcan0 FFF#0000000000000200
    sleep 1
    cansend vcan0 FFF#0000000000000400
    sleep 1
    cansend vcan0 FFF#0000000000000800
    sleep 1
    cansend vcan0 FFF#0000000000001000
    sleep 1
    cansend vcan0 FFF#0000000000002000
    sleep 1
    cansend vcan0 FFF#0000000000004000
    sleep 1
    cansend vcan0 FFF#0000000000008000
    sleep 1
    cansend vcan0 FFF#0000000000010000
    sleep 1
    cansend vcan0 FFF#0000000000020000
    sleep 1
    cansend vcan0 FFF#0000000000040000
    sleep 1
    cansend vcan0 FFF#0000000000080000
    # Wait for 1 seconds
    sleep 1
done