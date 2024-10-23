"""
Author: Samuel Parent
Date: 2024-04-13
"""

import argparse
import logging

from .can_generator import generate_can_from_dbc

logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")
logger = logging.getLogger(__name__)


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

    cmake_to_python_level = {
        "STATUS": "INFO",
        "INFO": "INFO",
        "VERBOSE": "DEBUG",
        "DEBUG": "DEBUG",
    }
    logger.setLevel(cmake_to_python_level[args.level])

    generate_can_from_dbc(args.project)
