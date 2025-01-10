from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Dict, List, Set, Optional, Type, Union

class SignalType(Enum):
    INTEGRAL = auto()
    FLOATING = auto()

class Endian(Enum):
    BIG = auto()
    LITTLE = auto()

@dataclass
class Signal:
    name: str # 1-32 chars, only [A-z], digits, and underscores
    start_bit: int # bit 0-63
    length: int # 1-64 bits
    endianness: Endian
    is_signed: bool
    minimum: Union[int, float]
    maximum: Union[int, float]
    unit: Optional[str] = None
    signal_type: SignalType
    receivers: Set[Node]
    description: Optional[str] = None

    # Not needed for INTEGRAL signals
    scale: Optional[float] = None
    offset: Optional[float] = None

@dataclass
class Message:
    name: str # 1-32 chars, only [A-z], digits, and underscores
    id: int # 0-2047 (11-bits) or 0-536870911 (29-bits for extended IDs)
    length: int # 0-8 bytes
    sender: Node
    signals: List[Signal]
    frequency: int
    description: Optional[str] = None

@dataclass
class Node:
    name: str
    description: Optional[str] = None

@dataclass
class Bus:
    name: str
    baud_rate: int
    nodes: Set[Node]
    messages: List[Message]
    description: Optional[str] = None

@dataclass
class Database:
    busses: Set[Bus]
    enum_types: Dict[str, Type[Enum]] = field(default_factory=dict) 
    description: Optional[str] = None
