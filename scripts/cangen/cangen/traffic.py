"""
Author: Manush Patel
Date: 2024-11-24
https://macformula.github.io/racecar/firmware/can-traffic/
"""

from dataclasses import dataclass


@dataclass
class CANMessage:
    data_length: int
    frequency: int


def data_length(data_bytes):
    fixed_length = 44  # fixed length of CAN frame
    variable_length = data_bytes * 9.6  # accounted for bit stuffing
    total_frame_length = variable_length + fixed_length
    return total_frame_length


def calculate_bus_load(messages, can_speed):
    total_bits = 0
    for message in messages:
        message_length = data_length(
            message.data_length
        )  # collect frame length of message
        total_bits += message_length * message.frequency
    bus_load = total_bits / can_speed * 100  # bus load formula
    return bus_load


if __name__ == "__main__":
    messages = [
        CANMessage(data_length=8, frequency=100),
        CANMessage(data_length=5, frequency=50),
        CANMessage(data_length=5, frequency=500),
    ]
    can_speed = 500000
    bus_load = calculate_bus_load(messages, can_speed)
    print(f"Calculated Bus Load is: {bus_load:.2f}%")
