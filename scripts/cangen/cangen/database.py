from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Dict, List, Set, Optional, Type
from abc import ABC


class Endian(Enum):
    BIG = auto()
    LITTLE = auto()


@dataclass
class Bus:
    name: str
    baud_rate: float
    nodes: Set[Node]
    messages: List[Message]
    enum_types: Dict[str, Type[Enum]] = field(default_factory=dict)
    description: Optional[str] = None


@dataclass
class Node:
    name: str
    description: Optional[str] = None


@dataclass
class Message:
    name: str  # 1-32 chars, only [A-z], digits, and underscores
    id: int  # 0-2047 (11-bits) or 0-536870911 (29-bits for extended IDs)
    is_extended_id: bool
    length: int  # 0-8 bytes
    sender: Node
    signals: List[Signal]
    receivers: Set[Node]
    frequency: float
    description: Optional[str] = None


@dataclass
class Signal(ABC):
    name: str  # 1-32 chars, only [A-z], digits, and underscores
    start_bit: int  # bit 0-63
    length: int  # 1-64 bits
    endianness: Endian
    additional_receivers: Optional[Set[Node]] = (
        None  # Currently all signals in a message are sent to the same receivers, but additional receivers can be specified for each signal if needed
    )
    description: Optional[str] = None


@dataclass
class IntegralSignal(Signal):
    is_signed: bool
    value_range: tuple[int, int]


@dataclass
class BooleanSignal(Signal):
    pass


@dataclass
class EnumSignal(Signal):
    enum_config: str


@dataclass
class FloatingSignal(Signal):
    is_signed: bool
    value_range: tuple[float, float]
    scale: float
    offset: float
    unit: str
