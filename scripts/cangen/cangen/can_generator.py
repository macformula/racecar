"""
Author: Samuel Parent
Date: 2024-04-13
"""

import logging
import math
import os
import re
import shutil
import time
from typing import Dict, List, Tuple

import numpy as np
from cantools.database import Database, Message, Signal
from jinja2 import Environment, PackageLoader

from .config import Bus, Config

logger = logging.getLogger(__name__)

EIGHT_BITS = 8
EIGHT_BYTES = 8
TOTAL_BITS = EIGHT_BITS * EIGHT_BYTES

TEMPLATE_FILE_NAMES = ["messages.hpp.jinja2", "bus.hpp.jinja2"]


def _parse_dbc_files(dbc_file: str) -> Database:
    logger.info(f"Parsing DBC files: {dbc_file}")
    can_db = Database()

    with open(dbc_file, "r") as f:
        can_db.add_dbc(f)
        logger.info(f"Successfully added DBC: {dbc_file}")

    return can_db


def _normalize_node_name(node_name: str) -> str:
    return node_name.upper()


def _filter_messages_by_node(
    messages: List[Message], node: str
) -> Tuple[List[Message], List[Message]]:
    normalized_node_name = _normalize_node_name(node)

    tx_msgs = [
        msg
        for msg in messages
        if normalized_node_name in map(_normalize_node_name, msg.senders)
    ]
    rx_msgs = [
        msg
        for msg in messages
        if normalized_node_name in map(_normalize_node_name, msg.receivers)
    ]

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


def _create_output_file_name(
    output_dir: str, bus_name: str, template_file_name: str
) -> str:
    return os.path.join(
        output_dir, bus_name.lower() + "_" + template_file_name.removesuffix(".jinja2")
    )


def _generate_code(bus: Bus, output_dir: str):
    """
    Parses DBC files, extracts information, and generates code using Jinja2
    templates.

    This method performs all the necessary steps to generate CAN code based on the
    provided DBC files and node information. It utilizes helper functions and Jinja2
    templates (not included here) to create the final code.
    """

    logger.info("Generating code")

    can_db = _parse_dbc_files(bus.dbc_file_path)
    rx_msgs, tx_msgs = _filter_messages_by_node(can_db.messages, bus.node)

    context = {
        "date": time.strftime("%Y-%m-%d"),
        "rx_msgs": rx_msgs,
        "tx_msgs": tx_msgs,
        "all_msgs": rx_msgs + tx_msgs,
        "signal_types": _get_signal_types(can_db),
        "temp_signal_types": _get_signal_types(can_db, allow_floating_point=False),
        "unpack_info": _get_masks_shifts(rx_msgs),
        "pack_info": _get_masks_shifts(tx_msgs),
        "bus_name": bus.bus_name,
        "node_name": bus.node,
    }

    logger.debug("Generating code for can messages and msg registry.")

    env = Environment(
        loader=PackageLoader(__package__), trim_blocks=True, lstrip_blocks=True
    )
    env.filters["decimal_to_hex"] = hex
    env.filters["camel_to_snake"] = _camel_to_snake

    for template_file_name in TEMPLATE_FILE_NAMES:
        template = env.get_template(template_file_name)
        rendered_code = template.render(**context)

        output_file_name = _create_output_file_name(
            output_dir, bus.bus_name, template_file_name
        )
        with open(output_file_name, "w") as output_file:
            output_file.write(rendered_code)
        logger.info(f"Rendered code written to '{os.path.abspath(output_file_name)}'")

        logger.info("Code generation complete")


def _prepare_output_directory(output_dir: str):
    """Deletes previously generated files and creates a gitignore for the directory"""
    if os.path.exists(output_dir):
        logger.info("Deleting previously generated code")
        shutil.rmtree(output_dir)

    logger.info("Creating generated/can folder")
    os.makedirs(output_dir, exist_ok=True)

    gitignore_path = os.path.join(output_dir, ".gitignore")
    with open(gitignore_path, "w") as f:
        f.write("*")


def generate_can_for_project(project_folder_name: str):
    """Generates C code for a given project.
    Ensure the project folder contains a config.yaml file which specifies the relative path to its corresponding dbc file.
    """
    os.chdir(project_folder_name)
    config = Config.from_yaml("config.yaml")

    _prepare_output_directory(config.output_dir)

    for bus in config.busses:
        _generate_code(bus, config.output_dir)
