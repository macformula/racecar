import os
import cantools
import logging
import math
import re

from typing import List, Tuple, Dict
from jinja2 import Environment


def big_endian_mask(length: int, start: int) -> Tuple[bytearray, int]:
    BITS_PER_BYTE = 8
    BYTE_PER_MSG = 8
    MAX_BITS = BITS_PER_BYTE*BYTE_PER_MSG
    
    start_flipped = (start // BITS_PER_BYTE) * BITS_PER_BYTE + \
        (BITS_PER_BYTE - 1 - (start%BITS_PER_BYTE)) # Flip bit direction (less confusing)
    pos = start_flipped
    end = pos+length-1

    mask = bytearray(BYTE_PER_MSG)
    while (pos < MAX_BITS):
        if (pos <= end):
            mask[pos//8] |= 1<<(7-pos%8)
            pos += 1
        else:
            break
        
    # # Convert each byte in the byte array to a binary string
    # binary_strings = [format(byte, '08b') for byte in mask]

    # # Join the binary strings and print
    # binary_string = ' '.join(binary_strings)
    # print("mask:", binary_string)
    # print("num zeroes:", 63-end)

    return mask, 63-end

def little_endian_mask(length: int, start: int) -> Tuple[bytearray, int]:
    mask = bytearray(8)
    pos = start
    end = start+length
    while (pos < 64):
        if (pos < end):
            mask[pos//8] |= 1<<(pos%8)
            pos += 1
        else:
            break
    # # Convert each byte in the byte array to a binary string
    # binary_strings = [format(byte, '08b') for byte in mask]

    # # Join the binary strings and print
    # binary_string = ' '.join(binary_strings)
    # print("mask:", binary_string)
    # print("num zeroes:", start%8)

    return mask, start

def big_endian_shift_amounts(num_zeroes: int) -> List[int]:
    shift_amounts = [0]*8
    for i in range(0, 8):
        shift_amounts[7-i] = i*8 - num_zeroes
    
    # print("shift amounts:", shift_amounts)

    return shift_amounts

def little_endian_shift_amounts(num_zeroes: int) -> List[int]:
    shift_amounts = [0]*8
    for i in range(0, 8):
        shift_amounts[i] = i*8 - num_zeroes
        
    print("shift amounts:", shift_amounts)

    return shift_amounts

def parse_dbc_files(dbc_filepaths: List[str]) -> cantools.database.Database:
    logger = logging.getLogger()
    can_db = cantools.database.Database()
    
    logger.info(f"adding dbc files ({dbc_filepaths})")
    for dbc_file in dbc_filepaths:
        if not str(dbc_file).endswith(".dbc"):
            logger.error(f"file provided is not a dbc ({dbc_file})")
        if not os.path.isfile(dbc_file):
            logger.error(f"file provided not found ({dbc_file})")
        with open(dbc_file, 'r') as f:
            can_db.add_dbc(f)
            logger.info(f"successfully added dbc ({dbc_file})")
    
    return can_db

def filter_messages_by_node(
    messages: List[cantools.database.Message],
    node: str) -> Tuple[List[cantools.database.Message], List[cantools.database.Message]]:
    rx_msgs, tx_msgs = [], []
    for msg in messages:
        if node in msg.senders:
            tx_msgs.append(msg)
        if node in msg.receivers:
            rx_msgs.append(msg)
            
    logging.getLogger().info(
        f"filtered messages by node ({node}) " +
        f"num msgs: rx = {len(rx_msgs)}, tx = {len(tx_msgs)}")
    
    return rx_msgs, tx_msgs 


def get_signal_types(can_db, allow_floating_point=True):
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
            
            if (isinstance(signal.scale, float) or signal.is_float) and allow_floating_point:
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
                
    return sig_types

def get_masks_shifts(
    msgs: List[cantools.database.Message]) -> Dict[str, Dict[str, Tuple[List[int], List[int]]]]:
    masks_shifts_dict = {}

    for msg in msgs:
        masks_shifts_dict[msg.name] = {}
        for sig in msg.signals:
            if sig.byte_order == 'little_endian':
                print("little")
                mask, num_trailing_zeros = little_endian_mask(sig.length, sig.start)
                shift_amounts = little_endian_shift_amounts(num_trailing_zeros)
            elif sig.byte_order == 'big_endian':
                print("big")
                mask, num_trailing_zeros = big_endian_mask(sig.length, sig.start)
                shift_amounts = big_endian_shift_amounts(num_trailing_zeros)
            else:
                print("ERROR: invalid byte order")
                exit(1)
            masks_shifts_dict[msg.name][sig.name] = ([int(byte) for byte in mask], shift_amounts)
    
    return masks_shifts_dict

        
def camel_to_snake(text):
    '''
    Converts UpperCamelCase to snake_case.
    '''
    
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', text)
    
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def decimal_to_hex(decimal_value):
    '''
    Converts a non-negative decimal integer to a lowercase hexadecimal string.

    Args:
        decimal_value: The non-negative decimal integer to convert.

    Returns:
        The hexadecimal representation of the decimal value as a lowercase string.

    Raises:
        TypeError: If the input is not an integer.
        ValueError: If the input is negative.
    '''

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

def generate_from_jinja2_template(template_path: str, output_path: str, context_dict: dict):
    # Read the template string from a file
    with open(template_path, 'r') as file:
        template_str = file.read()

    # Create the environment with trim_blocks and lstrip_blocks settings
    env = Environment(trim_blocks=True, lstrip_blocks=True)

    # Register the camel_to_snake filter (assuming it's defined elsewhere)
    env.filters['camel_to_snake'] = camel_to_snake
    env.filters['decimal_to_hex'] = decimal_to_hex

    # Load the template from the string content
    template = env.from_string(template_str)
    
    # Render the template with the context
    rendered_code = template.render(**context_dict)

    # Write the rendered code to a file
    with open(output_path, 'w') as output_file:
        output_file.write(rendered_code)

    print(f"Rendered code written to '{output_path}'")