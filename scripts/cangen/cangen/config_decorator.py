from __future__ import annotations
from dataclasses import field
from typing import (
    Any,
    Dict,
)
from abc import ABC


# All classes decorated with @configitem should inherit from this class
class ConfigBase(ABC):
    # Default implementation that can be overridden by subclasses to validate configuration after field-level validation
    @classmethod
    def validate_config(cls, validated: Dict[str, Any], context: Dict = None) -> None:
        pass


def config(
    *, validator=None, default=None, default_factory=None, required=True, **kwargs
):
    if default is not None and default_factory is not None:
        raise ValueError("Cannot specify both default and default_factory")

    if validator is not None and not hasattr(validator, "validate"):
        raise TypeError(
            f"Invalid validator: {type(validator)}. Validators must have a 'validate' method."
        )

    if validator is not None:
        validator.required = required

    # Create field metadata with validator
    metadata = {"validator": validator}

    if default_factory is not None:
        return field(default_factory=default_factory, metadata=metadata, **kwargs)
    else:
        return field(default=default, metadata=metadata, **kwargs)


def configitem(cls):
    # Get validation configurations from field metadata
    validation_configs = {}
    for field_name, field_obj in cls.__dataclass_fields__.items():
        if field_obj.metadata and "validator" in field_obj.metadata:
            validation_configs[field_name] = field_obj.metadata["validator"]

    cls._validation_configs = validation_configs
    original_init = cls.__init__

    # Instantiating the class will automatically validate the data
    def __init__(self, *args, **kwargs):
        context = kwargs.pop("context", None)
        validated_data = cls.validate(args[0], context=context)
        original_init(self, **validated_data)

    cls.__init__ = __init__

    @classmethod
    def validate(cls, data: Dict[str, Any], context: dict = None) -> Dict[str, Any]:
        validated = {}
        data = data or {}

        # Validate each field with its config
        for field_name, config in cls._validation_configs.items():
            value = data.get(field_name)
            validated[field_name] = config.validate(value, context=context)

        # Validate after field-level validation
        cls.validate_config(validated, context)

        return validated

    cls.validate = validate

    return cls
