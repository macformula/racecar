"""
Author: Samuel Parent
Date: 2024-04-13
"""

import logging
import math
import os
import re
import time
from typing import Dict, List, Tuple

import numpy as np
from cantools.database import Database, Message, Signal
from jinja2 import Environment

logger = logging.getLogger(__name__)

EIGHT_BITS = 8
EIGHT_BYTES = 8
TOTAL_BITS = EIGHT_BITS * EIGHT_BYTES
MSG_REGISTRY_FILE_NAME = "_msg_registry.h"
CAN_MESSAGES_FILE_NAME = "_can_messages.h"


def _assert_valid_dbc(filename: str):
    """Raise an error if filename is not a valid and existant dbc file."""

    if not os.path.isfile(filename):
        raise FileNotFoundError(f"Could not find a file at {filename}.")

    _, extension = os.path.splitext(filename)
    if extension != ".dbc":
        raise ValueError(f"{filename} is not a .dbc file.")

    logger.debug(f"{filename} is a valid dbc file.")


def _parse_dbc_files(dbc_files: List[str]) -> Database:
    logger.debug(f"Parsing DBC files: {dbc_files}")
    can_db = Database()

    for dbc_file in dbc_files:
        _assert_valid_dbc(dbc_file)
        with open(dbc_file, "r") as f:
            can_db.add_dbc(f)
            logger.info(f"Successfully added DBC: {dbc_file}")

    return can_db


def _filter_messages_by_node(
    messages: List[Message], node: str
) -> Tuple[List[Message], List[Message]]:
    tx_msgs = [msg for msg in messages if node in msg.senders]
    rx_msgs = [msg for msg in messages if node in msg.receivers]

    logger.debug(
        f"Filtered messages by node: {node}. "
        f"Num msgs: rx = {len(rx_msgs)}, tx = {len(tx_msgs)}"
    )

    return rx_msgs, tx_msgs


def _get_mask_shift_big(
    length: int, start: int
) -> Tuple[np.ndarray[int], np.ndarray[int]]:
    q, r = divmod(start, EIGHT_BITS)
    start_flipped = q * EIGHT_BITS + EIGHT_BITS - r - 1
    end = start_flipped + length

    idx = np.arange(64)
    mask_bool = (idx >= start_flipped) & (idx < end)
    mask_bytes = np.packbits(mask_bool, bitorder="big")
    logger.debug("Big endian mask generated.")

    num_zeros = TOTAL_BITS - end
    shift_amounts = np.arange(0, 64, 8, dtype=int)[::-1] - num_zeros

    logger.debug("Big endian shift amounts calculated.")

    return mask_bytes, shift_amounts


def _get_mask_shift_little(
    length: int, start: int
) -> Tuple[np.ndarray[int], np.ndarray[int]]:

    idx = np.arange(64)
    mask_bool = (idx >= start) & (idx < start + length)
    mask_bytes = np.packbits(mask_bool, bitorder="little")
    logger.debug("Little endian mask generated.")

    num_zeros = start
    shift_amounts = np.arange(0, 64, 8, dtype=int) - num_zeros
    logger.debug("Little endian shift amounts calculated.")

    return mask_bytes, shift_amounts


def _get_masks_shifts(
    msgs: List[Message],
) -> Dict[str, Dict[str, Tuple[List[int], List[int]]]]:

    # Create a dictionary of empty dictionaries, indexed by message names
    masks_shifts_dict = {msg.name: {} for msg in msgs}

    for msg in msgs:
        for sig in msg.signals:
            logger.debug(f"Processing (msg: {msg.name}, sig: {sig.name})")

            if sig.byte_order == "little_endian":
                mask, shift = _get_mask_shift_little(sig.length, sig.start)
            elif sig.byte_order == "big_endian":
                mask, shift = _get_mask_shift_big(sig.length, sig.start)
            else:
                raise ValueError(f"Invalid byteorder {sig.byte_order}.")

            # Evaluate that function on the signal start and length
            masks_shifts_dict[msg.name][sig.name] = mask, shift

    return masks_shifts_dict


def _get_signal_datatype(signal: Signal, allow_floating_point: bool = True) -> str:
    """Get the datatype of a signal."""
    num_bits = signal.length
    if signal.scale > 1:
        num_bits += math.ceil(math.log2(signal.scale))

    is_float = signal.is_float or isinstance(signal.scale, float)
    if is_float and allow_floating_point:
        return "float" if num_bits <= 32 else "double"

    if num_bits == 1:
        return "bool"

    sign = "" if signal.is_signed else "u"
    # Check all possible integer sizes
    for size in [8, 16, 32, 64]:
        if num_bits <= size:
            return "{}int{}_t".format(sign, size)

    raise ValueError("Signal does not have a valid datatype.")


def _get_signal_types(can_db: Database, allow_floating_point=True):

    # Create a dictionary (indexed by message name) of dictionaries (indexed by signal
    # name) corresponding to the datatype of each signal within each message.
    sig_types = {
        message.name: {
            signal.name: _get_signal_datatype(signal, allow_floating_point)
            for signal in message.signals
        }
        for message in can_db.messages
    }

    logger.debug("Signal types retrieved")

    return sig_types


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


def _generate_from_jinja2_template(
    template_path: str, output_path: str, context_dict: dict
):
    # Read the template string from a file
    with open(template_path, "r") as file:
        template_str = file.read()

    # Create the environment with trim_blocks and lstrip_blocks settings
    env = Environment(trim_blocks=True, lstrip_blocks=True)

    # Register the camel_to_snake filter
    env.filters["camel_to_snake"] = _camel_to_snake
    env.filters["decimal_to_hex"] = _decimal_to_hex

    # Load the template from the string content
    template = env.from_string(template_str)

    # Render the template with the context
    rendered_code = template.render(**context_dict)

    # Write the rendered code to a file
    output_dir = os.path.dirname(output_path)
    os.makedirs(output_dir, exist_ok=True)

    # Create a git ignore for everything in the generated path. Ignore everything.
    gitignore_path = os.path.join(output_dir, ".gitignore")
    if not os.path.exists(gitignore_path):
        with open(gitignore_path, "w") as f:
            f.write("*")

    with open(output_path, "w") as output_file:
        output_file.write(rendered_code)

    logger.info(f"Rendered code written to '{os.path.abspath(output_path)}'")


def generate_code(
    dbc_files: List[str],
    our_node: str,
    bus_name: str,
    output_dir: str,
    can_messages_template_path: str,
    msg_registry_template_path: str,
):
    """
    Parses DBC files, extracts information, and generates code using Jinja2
    templates.

    This method performs all the necessary steps to generate CAN code based on the
    provided DBC files and node information. It utilizes helper functions and Jinja2
    templates (not included here) to create the final code.
    """

    logger.info("Generating code")

    can_db = _parse_dbc_files(dbc_files)

    signal_types = _get_signal_types(can_db)
    temp_signal_types = _get_signal_types(can_db, allow_floating_point=False)

    rx_msgs, tx_msgs = _filter_messages_by_node(can_db.messages, our_node)
    all_msgs = rx_msgs + tx_msgs

    unpack_masks_shifts = _get_masks_shifts(rx_msgs)
    pack_masks_shifts = _get_masks_shifts(tx_msgs)

    context = {
        "date": time.strftime("%Y-%m-%d"),
        "rx_msgs": rx_msgs,
        "tx_msgs": tx_msgs,
        "all_msgs": all_msgs,
        "signal_types": signal_types,
        "temp_signal_types": temp_signal_types,
        "unpack_info": unpack_masks_shifts,
        "pack_info": pack_masks_shifts,
        "bus_name": bus_name,
    }

    # Replace these lines with your Jinja2 template logic
    logger.debug("Generating code for can messages")
    _generate_from_jinja2_template(
        can_messages_template_path,
        os.path.join(output_dir, bus_name.lower() + CAN_MESSAGES_FILE_NAME),
        context,
    )

    logger.debug("Generating code for msg registry")
    _generate_from_jinja2_template(
        msg_registry_template_path,
        os.path.join(output_dir, bus_name.lower() + MSG_REGISTRY_FILE_NAME),
        context,
    )

    logger.info("Code generation complete")
