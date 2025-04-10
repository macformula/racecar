"""
Author: Samuel Parent, Andrew Iammancini, Blake Freer
"""

from __future__ import annotations

import hashlib
import logging
import os
import re
import shutil
import time
from dataclasses import dataclass
from typing import List, Tuple

import numpy as np
from cantools.database import Database, Message, Signal
from jinja2 import Environment, PackageLoader

from .config import Bus, Config

logger = logging.getLogger(__name__)


TEMPLATE_FILE_NAMES = ["messages.hpp.jinja2", "bus.hpp.jinja2"]


@dataclass
class CppMessage:
    m: Message
    signals: List[CppSignal]

    def __init__(self, msg: Message):
        self.m = msg
        self.signals = [CppSignal(s) for s in self.m.signals]


@dataclass
class CppSignal:
    s: Signal  # for accessing most fields
    name: str
    type: str
    raw_type: str
    raw_type_bits: str
    masks_shifts: List[MaskShift]
    is_enum: bool

    def __init__(self, sig: Signal):
        logger.debug(f"Processing (sig: {sig.name})")

        self.s = sig
        self.name = _camel_to_snake(self.s.name)
        self.is_enum = self.s.choices is not None

        self.choose_datatypes()
        self.masks_shifts = self.choose_masks_shifts()

    def choose_datatypes(self) -> None:
        # Raw datatype is based on CAN layout
        self.raw_type_bits, self.raw_type = self.get_raw_type()
        # Actual datatype depends on more information
        self.type = self.get_datatype()

    def get_raw_type(self) -> Tuple[int, str]:
        for size in [8, 16, 32, 64]:
            if self.s.length <= size:
                sign_prefix = "" if self.s.is_signed else "u"
                return size, "{}int{}_t".format(sign_prefix, size)

        raise ValueError(f"Invalid number of bits ({self.s.length}).")

    def get_datatype(self) -> str:
        """Get the datatype of the physical signal value."""
        if self.is_enum:
            # need suffix to avoid clash between enum type name and signal name
            self.validate_enum()
            return self.s.name + "_t"

        elif self.s.length == 1:
            return "bool"

        elif self.s.is_float or self.s.scale != 1 or self.s.offset != 0:
            return "float"

        else:
            return self.raw_type

    def validate_enum(self):
        if self.s.offset != 0:
            raise ValueError(
                f"Enum field {self.s.name} must have offset=0 (has {self.s.offset})."
            )
        if self.s.scale != 1:
            raise ValueError(
                f"Enum field {self.s.name} must have scale=1 (has {self.s.scale})."
            )

    def choose_masks_shifts(self):
        match self.s.byte_order:
            case "little_endian":
                return CppSignal.get_mask_shift_little(self.s.length, self.s.start)
            case "big_endian":
                return CppSignal.get_mask_shift_big(self.s.length, self.s.start)
            case _:
                raise ValueError(f"Invalid byteorder {self.s.byte_order}.")

    @staticmethod
    def get_mask_shift_little(length: int, start: int) -> List[MaskShift]:
        idx = np.arange(64)
        mask_bool = (idx >= start) & (idx < start + length)
        mask_bytes = np.packbits(mask_bool, bitorder="little")
        logger.debug("Little endian mask generated.")

        num_zeros = start
        shift_amounts = np.arange(0, 64, 8, dtype=int) - num_zeros
        logger.debug("Little endian shift amounts calculated.")

        return [
            MaskShift(mask=m, shift=s, byte=i)
            for i, (m, s) in enumerate(zip(mask_bytes, shift_amounts))
            if m > 0
        ]

    @staticmethod
    def get_mask_shift_big(length: int, start: int) -> List[MaskShift]:
        EIGHT_BITS = 8
        EIGHT_BYTES = 8
        TOTAL_BITS = EIGHT_BITS * EIGHT_BYTES

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

        return [
            MaskShift(mask=m, shift=s, byte=i)
            for i, (m, s) in enumerate(zip(mask_bytes, shift_amounts))
            if m > 0
        ]


@dataclass
class MaskShift:
    mask: int
    shift: int
    byte: int


def _parse_dbc_files(dbc_file: str) -> Database:
    logger.info(f"Parsing DBC files: {dbc_file}")
    can_db = Database()

    with open(dbc_file, "r") as f:
        can_db.add_dbc(f)
        logger.info(f"Successfully added DBC: {dbc_file}")

    return can_db


def _extract_dbc_hash_version(dbc_file: str) -> int:
    """Generates a 64 bit integer hash of the dbc file"""
    with open(dbc_file, "r") as f:
        dbc_file_string = f.read()
        hash_hexadecimal = hashlib.sha256(dbc_file_string.encode()).hexdigest()[:16]
        return int(hash_hexadecimal, 16)


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
    raw_rx_msgs, raw_tx_msgs = _filter_messages_by_node(can_db.messages, bus.node)

    rx_msgs = [CppMessage(m) for m in raw_rx_msgs]
    tx_msgs = [CppMessage(m) for m in raw_tx_msgs]

    context = {
        "date": time.strftime("%Y-%m-%d"),
        "rx_msgs": rx_msgs,
        "tx_msgs": tx_msgs,
        "bus_name": bus.bus_name,
        "node_name": bus.node,
        "hash_version": _extract_dbc_hash_version(bus.dbc_file_path),
    }

    logger.debug("Generating code for can messages and msg registry.")

    env = Environment(
        loader=PackageLoader(__package__), trim_blocks=True, lstrip_blocks=True
    )
    env.filters["hex"] = hex
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
    Ensure the project folder contains a config.yaml file which specifies the relative
    path to its corresponding dbc file.
    """
    os.chdir(project_folder_name)
    config = Config.from_yaml("config.yaml")

    _prepare_output_directory(config.output_dir)

    for bus in config.busses:
        _generate_code(bus, config.output_dir)
