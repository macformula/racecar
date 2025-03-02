from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Any, Dict, Type, TypeVar, Generic, Optional
import re

T = TypeVar("T")


class ConfigType(Generic[T], ABC):
    def __init__(
        self, type_: Type[T], required: bool = True, default: Optional[T] = None
    ):
        self.type = type_
        self.required = required
        self.default = default

    @abstractmethod
    def validate(self, value: Any, context: dict = None) -> T:
        pass


class StringType(ConfigType[str]):
    def __init__(
        self, min_length: int = 0, max_length: int = None, pattern: str = None, **kwargs
    ):
        super().__init__(str, **kwargs)
        self.min_length = min_length
        self.max_length = max_length
        self.pattern = pattern

    def validate(self, value: Any, context: dict = None) -> str:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        if not isinstance(value, str):
            raise ValueError(f"Expected string, got {type(value)}")
        if len(value) < self.min_length:
            raise ValueError(f"String length must be at least {self.min_length}")
        if self.max_length and len(value) > self.max_length:
            raise ValueError(f"String length must be at most {self.max_length}")
        if self.pattern and not re.match(self.pattern, value):
            raise ValueError(f"String does not match pattern {self.pattern}")
        return value


class IntegerType(ConfigType[int]):
    def __init__(self, min_value: int = None, max_value: int = None, **kwargs):
        super().__init__(int, **kwargs)
        self.min_value = min_value
        self.max_value = max_value

    def validate(self, value: Any, context: dict = None) -> int:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        if not isinstance(value, (int, float)):
            raise ValueError(f"Expected number, got {type(value)}")
        val = int(value)
        if self.min_value is not None and val < self.min_value:
            raise ValueError(f"Value must be at least {self.min_value}")
        if self.max_value is not None and val > self.max_value:
            raise ValueError(f"Value must be at most {self.max_value}")
        return val


class FloatType(ConfigType[float]):
    def __init__(self, min_value: float = None, max_value: float = None, **kwargs):
        super().__init__(float, **kwargs)
        self.min_value = min_value
        self.max_value = max_value

    def validate(self, value: Any, context: dict = None) -> float:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None

        # Convert string to float if needed
        # This is needed for YAML parsing interpreting numerical values as strings (e.g. 1e-06)
        if isinstance(value, str):
            try:
                value = float(value)
            except ValueError:
                raise ValueError(f"Cannot convert string '{value}' to float")

        if not isinstance(value, (int, float)):
            raise ValueError(f"Expected number, got {type(value)}")
        val = float(value)
        if self.min_value is not None and val < self.min_value:
            raise ValueError(f"Value must be at least {self.min_value}")
        if self.max_value is not None and val > self.max_value:
            raise ValueError(f"Value must be at most {self.max_value}")
        return val


class BooleanType(ConfigType[bool]):
    def __init__(self, **kwargs):
        super().__init__(bool, **kwargs)

    def validate(self, value: Any, context: dict = None) -> bool:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        if isinstance(value, bool):
            return value
        raise ValueError(f"Expected boolean, got {type(value)}")


class ListType(ConfigType[list]):
    def __init__(self, item_type, **kwargs):
        super().__init__(list, **kwargs)
        self.item_type = item_type

    def validate(self, value: Any, context: dict = None) -> list:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        if not isinstance(value, list):
            raise ValueError(f"Expected list, got {type(value)}")
        validated_list = []
        for idx, item in enumerate(value):
            try:
                if hasattr(self.item_type, "validate") and callable(
                    self.item_type.validate
                ):
                    validated_item = self.item_type.validate(item, context=context)
                else:
                    validated_item = self.item_type(item)
                validated_list.append(validated_item)
            except Exception as e:
                raise ValueError(f"Error validating item at index {idx}: {e}")
        return validated_list


class DictType(ConfigType[dict]):
    def __init__(self, key_type, value_type, **kwargs):
        super().__init__(dict, **kwargs)
        self.key_type = key_type
        self.value_type = value_type

    def validate(self, value: Any, context: dict = None) -> dict:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        if not isinstance(value, dict):
            raise ValueError(f"Expected dict, got {type(value)}")
        validated = {}
        for k, v in value.items():
            if hasattr(self.key_type, "validate") and callable(self.key_type.validate):
                validated_key = self.key_type.validate(k, context=context)
            else:
                validated_key = self.key_type(k)
            if hasattr(self.value_type, "validate") and callable(
                self.value_type.validate
            ):
                validated_val = self.value_type.validate(v, context=context)
            else:
                validated_val = self.value_type(v)
            validated[validated_key] = validated_val
        return validated


class EnumType(ConfigType[dict]):
    def __init__(self, required: bool = True, default: Optional[dict] = None, **kwargs):
        super().__init__(dict, required=required, default=default, **kwargs)

    def validate(self, value: Any, context: dict = None) -> dict:
        if value is None:
            if self.default is not None:
                return self.default
            elif self.required:
                raise ValueError("A value is required but none was provided.")
            else:
                return None
        # Enums are treated as a dict during validation and converted to an enum during parsing
        if not isinstance(value, dict):
            raise ValueError(f"Expected a dict for enum definition, got {type(value)}")
        validated = DictType(StringType(), IntegerType()).validate(
            value, context=context
        )
        return validated


class Config(ABC):
    @classmethod
    def from_dict(cls, data: Dict[str, Any], context: dict = None) -> Config:
        instance = cls()
        for key, config_attr in cls.__dict__.items():
            if isinstance(config_attr, ConfigType):
                # Get the value from data or None if doesn't exist
                value = data.get(key, None)
                setattr(instance, key, config_attr.validate(value, context=context))
        return instance


class SignalConfig(Config):
    name = StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    start_bit = IntegerType(min_value=0, max_value=63)
    length = IntegerType(min_value=1, max_value=64)
    endianness = (
        StringType()
    )  # Validated as string, will be converted to Endian enum after validation
    description = StringType(required=False)
    additional_receivers = ListType(StringType(), required=False)

    # Identifies the type of signal to determine which Signal class to use
    signal_type = StringType()
    # Fields for specific signal types
    is_signed = BooleanType(required=False)
    value_range = ListType(FloatType(), required=False)
    enum_config = StringType(required=False)
    scale = FloatType(required=False)
    offset = FloatType(required=False)
    unit = StringType(required=False)

    @classmethod
    def validate(cls, data: Dict[str, Any], context: dict = None) -> Dict[str, Any]:
        validated = super().from_dict(data, context=context).__dict__
        if context is not None and "enum_types" in context:
            enum_types = context["enum_types"]
            enum_key = validated.get("enum_config")
            if enum_key is not None and enum_key not in enum_types:
                raise ValueError(
                    f"Signal '{validated.get('name')}' references undefined enum_config '{enum_key}'. "
                    f"Available enum_types: {list(enum_types.keys())}"
                )
        return validated


class NodeConfig(Config):
    name = StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    description = StringType(required=False)

    @classmethod
    def validate(self, data: Dict[str, Any]) -> Dict[str, Any]:
        validated = super().from_dict(data).__dict__
        return validated


class MessageConfig(Config):
    name = StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    is_extended_id = BooleanType()
    id = IntegerType(min_value=0, max_value=536870911)
    length = IntegerType(min_value=0, max_value=8)
    sender = StringType(
        min_length=1
    )  # Nodes are validated as strings, will be converted to Node objects after validation
    signals = ListType(SignalConfig)
    receivers = ListType(
        StringType(min_length=1)
    )  # Nodes are validated as strings, will be converted to Node objects after validation
    frequency = FloatType(min_value=0.0)
    description = StringType(required=False)

    @classmethod
    def validate(cls, data: Dict[str, Any], context: dict = None) -> Dict[str, Any]:
        validated = super().from_dict(data, context=context).__dict__
        # Additional validation for extended ID:
        if not validated["is_extended_id"] and validated["id"] > 2047:
            raise ValueError("Standard CAN ID must be between 0 and 2047")

        # Validate that the sender and receivers are valid nodes
        if validated["sender"] not in context["nodes"]:
            raise ValueError(
                f"Message '{validated.get('name')}' has sender '{validated.get('sender')}' which is not a valid node"
            )
        for receiver in validated["receivers"]:
            if receiver not in context["nodes"]:
                raise ValueError(
                    f"Message '{validated.get('name')}' has receiver '{receiver}' which is not a valid node"
                )
        return validated


class BusConfig(Config):
    name = StringType(min_length=1, max_length=32, pattern=r"^[A-Za-z0-9_]+$")
    baud_rate = FloatType(min_value=1000, max_value=1000000, required=False)
    messages = ListType(MessageConfig)
    description = StringType(required=False)
    # enum_types and nodes are separately validated

    @classmethod
    def validate(cls, data: Dict[str, Any], context: dict = None) -> Dict[str, Any]:
        validated = super().from_dict(data, context=context).__dict__
        return validated


class EnumTypesConfig(Config):
    enum_types = DictType(StringType(), EnumType())

    @classmethod
    def validate(cls, data: Dict[str, Any]) -> Dict[str, dict]:
        validated = super().from_dict(data).__dict__
        return validated["enum_types"]
