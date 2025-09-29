"""
Author: Samuel Parent
Date: 2024-04-13
"""

import argparse
import logging

from .can_generator import generate_can_for_project

logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")
logger = logging.getLogger("cangen")


def parse():
    parser = argparse.ArgumentParser(description="DBC to C code generator")
    parser.add_argument("project", type=str, help="Path to project root")
    parser.add_argument(
        "-v",
        "--verbose",
        dest="is_verbose",
        action="store_true",
        help="Enable verbose output",
    )

    # If parsing fails (ex incorrect or no arguments provided) then this code will be 2
    args = parser.parse_args()

    return args


def main():
    args = parse()

    logger.setLevel(logging.DEBUG if args.is_verbose else logging.INFO)

    generate_can_for_project(args.project)


if __name__ == "__main__":
    main()
