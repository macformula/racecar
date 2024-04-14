"""
Author: Samuel Parent
Date: 2024-04-13
"""

from can_generator import CanGenerator
import yaml
import argparse
import logging
import os

SCRIPTS_TO_PROJECTS_DIR = "../../firmware/projects"
SCRIPTS_TO_DBCS = "../../firmware/dbcs"
SCRIPTS_TO_TEMPLATES_DIR = "templates"
CONFIG_FILE_PATH = "config.yaml"
CAN_MESSAGES_TEMPLATE_FILENAME = "can_messages.h.jinja2"
MSG_REGISTRY_FILENAME = "msg_registry.h.jinja2"
OUTPUT_DIR = "generated/can"

logging.basicConfig(level="INFO", format="%(levelname)-8s| (%(name)s) %(message)s")

def read_yaml_file(yaml_file):
    with open(yaml_file, "r") as file:
        return yaml.safe_load(file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="DBC to C code generator")
    parser.add_argument("--project", type=str, required=True, help="Name of the project")

    try:
        args = parser.parse_args()
    except argparse.ArgumentError:
        parser.print_help()
        exit(1)

    project_dir = args.project
    this_script_dir = os.path.dirname(os.path.abspath(__file__))

    # Change directory to the project folder
    os.chdir(os.path.join(this_script_dir, SCRIPTS_TO_PROJECTS_DIR, project_dir))

    config = read_yaml_file(CONFIG_FILE_PATH)

    our_node = config["canGen"]["ourNode"].upper()
    bus_name = config["canGen"]["busName"].capitalize()
    dbc_files = config["canGen"]["dbcFiles"]
    output_dir = OUTPUT_DIR

    dbc_file_paths = [
        os.path.join(this_script_dir, SCRIPTS_TO_DBCS, dbc) for dbc in dbc_files
    ]

    can_msg_template_path = os.path.join(
        this_script_dir, SCRIPTS_TO_TEMPLATES_DIR, CAN_MESSAGES_TEMPLATE_FILENAME
    )
    msg_registry_template_path = os.path.join(
        this_script_dir, SCRIPTS_TO_TEMPLATES_DIR, MSG_REGISTRY_FILENAME
    )

    can_generator = CanGenerator(
        can_messages_template_path=can_msg_template_path,
        msg_registry_template_path=msg_registry_template_path,
    )

    can_generator.generate_code(
        dbc_files=dbc_file_paths,
        our_node=our_node,
        bus_name=bus_name,
        output_dir=output_dir,
    )
