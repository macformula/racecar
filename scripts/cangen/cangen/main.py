"""
Author: Samuel Parent
Date: 2024-04-13
"""

import argparse
import logging

from .can_generator import generate_can_for_project

logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")
logger = logging.getLogger(__name__)


def parse():
    parser = argparse.ArgumentParser(description="DBC to C code generator")
    parser.add_argument("project", type=str, help="Name of the project")
    group = parser.add_mutually_exclusive_group()
    group.add_argument(
        "-v",
        "--verbose",
        dest="is_verbose",
        action="store_true",
        help="Enable verbose output",
    )
    group.add_argument(
        "--cmake-log-level",
        dest="level",
        choices=["STATUS", "INFO", "VERBOSE", "DEBUG"],
        default="INFO",
        help="Log verbosity threshold",
    )

    # If parsing fails (ex incorrect or no arguments provided) then this code will be 2
    args = parser.parse_args()

    cmake_to_python_level = {
        "STATUS": "INFO",
        "INFO": "INFO",
        "VERBOSE": "DEBUG",
        "DEBUG": "DEBUG",
    }

    args.level = (
        cmake_to_python_level["VERBOSE"]
        if args.is_verbose
        else cmake_to_python_level[args.level]
    )
    return args


def main():
    args = parse()

    logger.setLevel(args.level)

    generate_can_for_project(args.project)
