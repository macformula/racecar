import os
import logging
import math
import re
import cantools
import datetime
from typing import List, Tuple, Dict
from jinja2 import Environment

logger = logging.getLogger(__name__)


class CanGen:
    """
    This class provides functionalities for CAN code generation based on DBC files.
    """

    def __init__(
        self,
        msg_registry_template_path: str,
        can_messages_template_path: str,
    ):
        """
        Initializes the CanGen object.

        Args:
            dbc_files (list): List of paths to DBC files.
            our_node (str): The CAN node for which to generate code.
            log_level (int, optional): Logging level for the class.
            Defaults to logging.DEBUG.
        """

        self.msg_registry_template_path = msg_registry_template_path
        self.can_messages_template_path = can_messages_template_path

        # Constants
        self.EIGHT_BITS = 8
        self.EIGHT_BYTES = 8
        self.TOTAL_BITS = self.EIGHT_BITS * self.EIGHT_BYTES
        self.MSG_REGISTRY_FILE_NAME = "msg_registry.h"
        self.CAN_MESSAGES_FILE_NAME = "can_messages.h"

    def __parse_dbc_files(self, dbc_files: List[str]) -> cantools.database.Database:
        logger.info(f"Parsing DBC files: {dbc_files}")
        can_db = cantools.database.Database()

        for dbc_file in dbc_files:
            if not str(dbc_file).endswith(".dbc"):
                logger.error(f"File provided is not a .dbc: {dbc_file}")
                continue
            if not os.path.isfile(dbc_file):
                logger.error(f"File provided not found: {dbc_file}")
                continue
            with open(dbc_file, "r") as f:
                can_db.add_dbc(f)
                logger.info(f"Successfully added DBC: {dbc_file}")

        return can_db

    def __filter_messages_by_node(
        self, messages: List[cantools.database.Message], node: str
    ) -> Tuple[List[cantools.database.Message], List[cantools.database.Message]]:
        rx_msgs, tx_msgs = [], []
        for msg in messages:
            if node in msg.senders:
                tx_msgs.append(msg)
            if node in msg.receivers:
                rx_msgs.append(msg)

        logger.info(
            f"Filtered messages by node: {node}. Num msgs: rx = {len(rx_msgs)}, tx = "
            f"{len(tx_msgs)}"
        )

        return rx_msgs, tx_msgs

    def __get_masks_shifts(
        self, msgs: List[cantools.database.Message]
    ) -> Dict[str, Dict[str, Tuple[List[int], List[int]]]]:
        masks_shifts_dict = {}

        for msg in msgs:
            masks_shifts_dict[msg.name] = {}
            for sig in msg.signals:
                if sig.byte_order == "little_endian":
                    logger.debug(
                        f"Using little-endian byte order "
                        f"(msg: {msg.name}, sig: {sig.name})"
                    )
                    mask, num_trailing_zeros = self.__little_endian_mask(
                        sig.length, sig.start
                    )
                    shift_amounts = self.__little_endian_shift_amounts(
                        num_trailing_zeros
                    )
                elif sig.byte_order == "big_endian":
                    logger.debug(
                        f"Using big-endian byte order "
                        f"(msg: {msg.name}, sig: {sig.name})"
                    )
                    mask, num_trailing_zeros = self.__big_endian_mask(
                        sig.length, sig.start
                    )
                    shift_amounts = self.__big_endian_shift_amounts(num_trailing_zeros)
                else:
                    logger.error(f"Invalid byte order ({sig.byte_order})")
                    exit(1)
                masks_shifts_dict[msg.name][sig.name] = (
                    [int(byte) for byte in mask],
                    shift_amounts,
                )

        return masks_shifts_dict

    def __big_endian_mask(self, length: int, start: int) -> Tuple[bytearray, int]:
        start_flipped = (start // self.EIGHT_BITS) * self.EIGHT_BITS + (
            self.EIGHT_BITS - 1 - (start % self.EIGHT_BITS)
        )  # Flip bit direction (less confusing)
        pos = start_flipped
        end = pos + length - 1

        mask = bytearray(self.EIGHT_BYTES)
        while pos < self.TOTAL_BITS:
            if pos <= end:
                mask[pos // self.EIGHT_BYTES] |= 1 << (
                    self.EIGHT_BITS - 1 - pos % self.EIGHT_BITS
                )
                pos += 1
            else:
                break

        logger.info("Big endian mask generated")

        return mask, (self.TOTAL_BITS - 1 - end)

    def __little_endian_mask(self, length: int, start: int) -> Tuple[bytearray, int]:
        mask = bytearray(self.EIGHT_BYTES)
        pos = start
        end = start + length
        while pos < self.TOTAL_BITS:
            if pos < end:
                mask[pos // self.EIGHT_BYTES] |= 1 << (pos % self.EIGHT_BITS)
                pos += 1
            else:
                break

        logger.info("Little endian mask generated")

        return mask, start

    def __big_endian_shift_amounts(self, num_zeroes: int) -> List[int]:
        shift_amounts = [0] * self.EIGHT_BYTES
        for i in range(0, self.EIGHT_BYTES):
            shift_amounts[self.EIGHT_BYTES - 1 - i] = (i * self.EIGHT_BITS) - num_zeroes

        logger.info("Big endian shift amounts calculated")

        return shift_amounts

    def __little_endian_shift_amounts(self, num_zeroes: int) -> List[int]:
        shift_amounts = [0] * self.EIGHT_BYTES
        for i in range(0, self.EIGHT_BYTES):
            shift_amounts[i] = i * self.EIGHT_BITS - num_zeroes

        logger.info("Little endian shift amounts calculated")

        return shift_amounts

    def __get_signal_types(self, can_db, allow_floating_point=True):
        sig_types = {}

        for message in can_db.messages:
            sig_types[message.name] = {}
            for signal in message.signals:
                num_bits = signal.length
                sign = ""

                if signal.scale > 1:
                    num_bits = signal.length + math.ceil(math.log2(signal.scale))

                if not signal.is_signed:
                    sign = "u"

                if (
                    isinstance(signal.scale, float) or signal.is_float
                ) and allow_floating_point:
                    if num_bits <= 32:
                        sig_types[message.name][signal.name] = "float"
                        continue
                    else:
                        sig_types[message.name][signal.name] = "double"
                        continue
                if num_bits == 1:
                    sig_types[message.name][signal.name] = "bool"
                elif num_bits <= 8:
                    sig_types[message.name][signal.name] = sign + "int8_t"
                    continue
                elif num_bits <= 16:
                    sig_types[message.name][signal.name] = sign + "int16_t"
                    continue
                elif num_bits <= 32:
                    sig_types[message.name][signal.name] = sign + "int32_t"
                    continue
                else:
                    sig_types[message.name][signal.name] = sign + "int64_t"
                    continue

        logger.info("Signal types retrieved")

        return sig_types

    def __camel_to_snake(self, text):
        """
        Converts UpperCamelCase to snake_case.
        """

        s1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", text)

        return re.sub("([a-z0-9])([A-Z])", r"\1_\2", s1).lower()

    def __decimal_to_hex(self, decimal_value):
        """
        Converts a non-negative decimal integer to a lowercase hexadecimal string.

        Args:
            decimal_value: The non-negative decimal integer to convert.

        Returns:
            The hexadecimal representation of the decimal value as a lowercase string.

        Raises:
            TypeError: If the input is not an integer.
            ValueError: If the input is negative.
        """

        if not isinstance(decimal_value, int):
            raise TypeError("Input must be an integer")
        if decimal_value < 0:
            raise ValueError("Input must be a non-negative integer")

        hex_digits = "0123456789ABCDEF"
        hex_string = ""
        while decimal_value > 0:
            remainder = decimal_value % 16
            hex_string = hex_digits[remainder] + hex_string
            decimal_value //= 16

        return "0x" + hex_string.lower()

    def __generate_from_jinja2_template(
        self, template_path: str, output_path: str, context_dict: dict
    ):
        # Read the template string from a file
        with open(template_path, "r") as file:
            template_str = file.read()

        # Create the environment with trim_blocks and lstrip_blocks settings
        env = Environment(trim_blocks=True, lstrip_blocks=True)

        # Register the camel_to_snake filter
        env.filters["camel_to_snake"] = self.__camel_to_snake
        env.filters["decimal_to_hex"] = self.__decimal_to_hex

        # Load the template from the string content
        template = env.from_string(template_str)

        # Render the template with the context
        rendered_code = template.render(**context_dict)

        # Write the rendered code to a file
        with open(output_path, "w") as output_file:
            output_file.write(rendered_code)

        logger.info(f"Rendered code written to '{output_path}'")

    def generate_code(
        self, dbc_files: List[str], our_node: str, bus_name: str, output_dir: str
    ):
        """
        Parses DBC files, extracts information, and generates code using Jinja2
        templates.

        This method performs all the necessary steps to generate CAN code based on the
        provided DBC files and node information. It utilizes helper functions and Jinja2
        templates (not included here) to create the final code.
        """

        logger.info("Generating code")

        can_db = self.__parse_dbc_files(dbc_files)

        signal_types = self.__get_signal_types(can_db)
        temp_signal_types = self.__get_signal_types(can_db, allow_floating_point=False)

        rx_msgs, tx_msgs = self.__filter_messages_by_node(can_db.messages, our_node)

        unpack_masks_shifts = self.__get_masks_shifts(rx_msgs)
        pack_masks_shifts = self.__get_masks_shifts(tx_msgs)

        context = {
            "date": datetime.date.today().strftime("%Y-%m-%d"),
            "rx_msgs": rx_msgs,
            "tx_msgs": tx_msgs,
            "signal_types": signal_types,
            "temp_signal_types": temp_signal_types,
            "unpack_info": unpack_masks_shifts,
            "pack_info": pack_masks_shifts,
            "bus_name": bus_name,
        }

        # Replace these lines with your Jinja2 template logic
        logger.info("Generating code for can messages")
        self.__generate_from_jinja2_template(
            self.can_messages_template_path,
            output_dir + "/" + self.CAN_MESSAGES_FILE_NAME,
            context,
        )

        logger.info("Generating code for msg registry")
        self.__generate_from_jinja2_template(
            self.msg_registry_template_path,
            output_dir + "/" + self.MSG_REGISTRY_FILE_NAME,
            context,
        )

        logger.info("Code generation complete")
