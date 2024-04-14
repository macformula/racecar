"""
Author: Samuel Parent
Date: 2024-04-13
"""

import argparse
import logging
import os

import can_generator
import yaml

# Generate a set of directory paths, all based on this file's location
DIR_THIS_FILE = os.path.abspath(os.path.dirname(__file__))

DIR_FIRMWARE = os.path.join(DIR_THIS_FILE, os.pardir, os.pardir, "firmware")
DIR_PROJECTS = os.path.join(DIR_FIRMWARE, "projects")
DIR_DBCS = os.path.join(DIR_FIRMWARE, "dbcs")

CONFIG_FILE_NAME = "config.yaml"
OUTPUT_DIR = "generated/can"

DIR_TEMPLATES = os.path.join(DIR_THIS_FILE, "templates")
CAN_MESSAGES_TEMPLATE_FILENAME = "can_messages.h.jinja2"
MSG_REGISTRY_TEMPLATE_FILENAME = "msg_registry.h.jinja2"


logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")


def parse():
    parser = argparse.ArgumentParser(description="DBC to C code generator")
    parser.add_argument(
        "--project", type=str, required=True, help="Name of the project"
    )

    # If parsing fails (ex incorrect or no arguments provided) then this exits with
    # code 2.
    return parser.parse_args()


if __name__ == "__main__":

    # Change directory to the project folder
    args = parse()
    project_folder_name = args.project
    os.chdir(os.path.join(DIR_PROJECTS, project_folder_name))

    # Read & Parse the config file
    with open(CONFIG_FILE_NAME, "r") as file:
        config = yaml.safe_load(file)

    our_node = config["canGen"]["ourNode"].upper()
    bus_name = config["canGen"]["busName"].capitalize()
    dbc_files = config["canGen"]["dbcFiles"]

    dbc_file_paths = [os.path.join(DIR_DBCS, dbc) for dbc in dbc_files]

    can_messages_template_path = os.path.join(
        DIR_TEMPLATES, CAN_MESSAGES_TEMPLATE_FILENAME
    )
    msg_registry_template_path = os.path.join(
        DIR_TEMPLATES, MSG_REGISTRY_TEMPLATE_FILENAME
    )

    can_generator.generate_code(
        dbc_file_paths,
        our_node,
        bus_name,
        OUTPUT_DIR,
        can_messages_template_path,
        msg_registry_template_path,
    )
