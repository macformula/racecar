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

    @property
    def bit_count(self):
        fixed_length = 44  # fixed length of CAN frame
        variable_length = self.data_length * 8.0  # accounted for bit stuffing
        total_frame_length = variable_length + fixed_length
        return total_frame_length


def calculate_bus_load(messages: list[CANMessage], can_speed: int) -> float:
    """Calculates the bus load percentage based on CAN messages and bus speed

    Parameters
    ----------
    messages : list[CANMessage]
        A list of CANMessage objects, each containing `bit_count` and `frequency` attributes
    can_speed : int
        The CAN bus speed in bits per second (bps).

    Returns
    -------
    float
        Returns bus load as a percentage
    """
    if can_speed < 0:
        print(f"Invalid CAN speed: {can_speed}")
        return 0

    total_bits = sum(message.bit_count * message.frequency for message in messages)

    if total_bits < 0:
        return 0

    bus_load = ((total_bits) * 1.2) / can_speed * 100
    return round(bus_load, 2)
