import re


def _camel_to_snake(text):
    """Converts UpperCamelCase to snake_case."""

    s1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", text)
    return re.sub("([a-z0-9])([A-Z])", r"\1_\2", s1).lower()


def _decimal_to_hex(decimal_value):
    """
    Converts a non-negative decimal integer to a lowercase hexadecimal string.

    Raises:
        ValueError: If the input is negative.
    """

    if decimal_value < 0:
        raise ValueError("Input must be a non-negative integer")

    return hex(decimal_value)
