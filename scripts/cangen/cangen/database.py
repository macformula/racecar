from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Dict, List, Optional, Type
from abc import ABC


class Endian(Enum):
    BIG = auto()
    LITTLE = auto()


@dataclass
class Bus:
    name: str = None
    baud_rate: float = None
    nodes: List[Node] = field(default_factory=list)
    messages: List[Message] = field(default_factory=list)
    enum_types: Dict[str, Type[Enum]] = field(default_factory=dict)
    description: Optional[str] = None


@dataclass
class Node:
    name: str = None
    description: Optional[str] = None


@dataclass
class Message:
    name: str = None  # 1-32 chars, only [A-z], digits, and underscores
    id: int = None  # 0-2047 (11-bits) or 0-536870911 (29-bits for extended IDs)
    is_extended_id: bool = None
    length: int = None  # 0-8 bytes
    sender: Node = None
    signals: List[Signal] = field(default_factory=list)
    receivers: List[Node] = field(default_factory=list)
    frequency: float = None
    description: Optional[str] = None


@dataclass
class Signal(ABC):
    name: str = ""  # 1-32 chars, only [A-z], digits, and underscores
    start_bit: int = None  # bit 0-63
    length: int = None  # 1-64 bits
    endianness: Endian = None
    description: Optional[str] = None
    additional_receivers: Optional[List[Node]] = None


@dataclass
class IntegralSignal(Signal):
    is_signed: bool = None
    value_range: tuple[int, int] = None


@dataclass
class BooleanSignal(Signal):
    pass


@dataclass
class EnumSignal(Signal):
    enum_config: str = ""


@dataclass
class FloatingSignal(Signal):
    is_signed: bool = None
    value_range: tuple[float, float] = None
    scale: float = None
    offset: float = None
    unit: str = None
