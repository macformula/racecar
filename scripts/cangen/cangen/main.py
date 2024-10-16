"""
Author: Samuel Parent
Date: 2024-04-13
"""

import argparse
import logging

from .generate_can_from_dbc import generate_can_from_dbc

logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")

CONFIG_FILE_NAME = "config.yaml"


def parse():
    parser = argparse.ArgumentParser(description="DBC to C code generator")
    parser.add_argument("project", type=str, help="Name of the project")
    parser.add_argument(
        "--log-level",
        dest="level",
        choices=["STATUS", "INFO", "VERBOSE", "DEBUG"],
        default="INFO",
        help="Log verbosity threshold",
    )

    # If parsing fails (ex incorrect or no arguments provided) then this exits with
    # code 2.
    return parser.parse_args()


def main():
    args = parse()

    # Map CMAKE log levels -> Python log levels
    log_level_mappings = {
        "STATUS": "INFO",
        "INFO": "INFO",
        "VERBOSE": "DEBUG",
        "DEBUG": "DEBUG",
    }
    # Set log level threshold to specified verbosity
    logging.getLogger().setLevel(log_level_mappings[args.level])

    generate_can_from_dbc(args.project)
