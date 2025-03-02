from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Dict, List, Optional, Type
from abc import ABC
from config_decorator import configitem, config, ConfigBase
from database_config import StringType, FloatType, IntegerType, BooleanType, ListType


class Endian(Enum):
    BIG = auto()
    LITTLE = auto()


@configitem
@dataclass
class Bus(ConfigBase):
    name: str = config(
        validator=StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    )
    baud_rate: float = config(
        validator=FloatType(min_value=1000, max_value=1000000),
        required=False,
        default=None,
    )
    nodes: List[Node] = field(default_factory=list)
    messages: List[Message] = field(default_factory=list)
    enum_types: Dict[str, Type[Enum]] = field(default_factory=dict)
    description: Optional[str] = config(
        validator=StringType(), required=False, default=None
    )


@configitem
@dataclass
class Node(ConfigBase):
    name: str = config(
        validator=StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    )
    description: Optional[str] = config(
        validator=StringType(), required=False, default=None
    )


@configitem
@dataclass
class Message(ConfigBase):
    name: str = config(
        validator=StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    )
    id: int = config(validator=IntegerType(min_value=0, max_value=536870911))
    is_extended_id: bool = config(validator=BooleanType())
    length: int = config(validator=IntegerType(min_value=0, max_value=8))
    sender: Node = config(validator=StringType(min_length=1))
    signals: List[Signal] = field(default_factory=list)
    receivers: List[Node] = config(
        validator=ListType(StringType(min_length=1)), default_factory=list
    )
    frequency: float = config(validator=FloatType(min_value=0.0))
    description: Optional[str] = config(
        validator=StringType(), required=False, default=None
    )

    @classmethod
    def validate_config(cls, validated, context=None):
        # Validate standard CAN ID range
        if not validated["is_extended_id"] and validated["id"] > 2047:
            raise ValueError("Standard CAN ID must be between 0 and 2047")

        # Validate sender and receivers exist in nodes
        if context and "nodes" in context:
            if validated["sender"] not in context["nodes"]:
                raise ValueError(
                    f"Message '{validated.get('name')}' has sender '{validated.get('sender')}' which is not a valid node"
                )
            for receiver in validated["receivers"]:
                if receiver not in context["nodes"]:
                    raise ValueError(
                        f"Message '{validated.get('name')}' has receiver '{receiver}' which is not a valid node"
                    )


@configitem
@dataclass
class Signal(ConfigBase, ABC):
    name: str = config(
        validator=StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$"),
        default="",
    )
    start_bit: int = config(validator=IntegerType(min_value=0, max_value=63))
    length: int = config(validator=IntegerType(min_value=1, max_value=64))
    endianness: Endian = config(validator=StringType())
    description: Optional[str] = config(
        validator=StringType(), required=False, default=None
    )
    additional_receivers: Optional[List[Node]] = config(
        validator=ListType(StringType()), required=False, default=None
    )
    signal_type: str = config(validator=StringType(), required=True)

    @classmethod
    def validate_config(cls, validated, context=None):
        # Validate signal type
        if validated["signal_type"] not in ["integral", "boolean", "enum", "floating"]:
            raise ValueError(f"Invalid signal type: {validated['signal_type']}")


@configitem
@dataclass
class IntegralSignal(Signal):
    is_signed: bool = config(validator=BooleanType(), required=False, default=None)
    value_range: List[int] = config(
        validator=ListType(IntegerType()), required=False, default=None
    )


@configitem
@dataclass
class BooleanSignal(Signal):
    pass


@configitem
@dataclass
class EnumSignal(Signal):
    enum_config: str = config(validator=StringType(min_length=1), default="")

    @classmethod
    def validate_config(cls, validated, context=None):
        super().validate_config(validated, context)

        # Set signal type for enum signals
        validated["signal_type"] = "enum"

        # Validate enum_config exists in context
        enum_config = validated.get("enum_config", None)
        if not enum_config:
            raise ValueError("Enum config is required for enum signals")

        if context and "enum_types" in context:
            if enum_config not in context["enum_types"]:
                raise ValueError(
                    f"Enum signal '{validated.get('name')}' references unknown enum_config '{enum_config}'"
                )


@configitem
@dataclass
class FloatingSignal(Signal):
    is_signed: bool = config(validator=BooleanType(), required=False, default=None)
    value_range: List[float] = config(
        validator=ListType(FloatType()), required=False, default=None
    )
    scale: float = config(validator=FloatType(), required=False, default=None)
    offset: float = config(validator=FloatType(), required=False, default=None)
    unit: str = config(validator=StringType(), required=False, default=None)
