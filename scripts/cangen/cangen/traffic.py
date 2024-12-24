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
        return 44 + 8 * self.data_length

def calculate_bus_load(messages: list[CANMessage], can_speed: int) -> float:
    """Calculates the bus load percentage based on CAN messages and bus speed

    Parameters
    ----------
    messages : list[CANMessage]
    can_speed : int

    Returns
    -------
    float
        Returns bus load as a percentage
    """
    if can_speed <= 0:
        raise ValueError(f"Invalid CAN speed {can_speed}. Must be positive.")

    total_bits = sum(message.bit_count * message.frequency for message in messages) *1.2 # 1.2x is for bit stuffing

    bus_load = total_bits / can_speed * 100
    
    return bus_load
