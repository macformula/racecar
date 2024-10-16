import os
from .can_generator import generate_code
from .Config import Config

CONFIG_FILE_NAME = "config.yaml"


def generate_can_from_dbc(project_folder_name: str):
    # Change directory to the project folder
    os.chdir(project_folder_name)

    # Parse the config file into a Config object
    config = Config(CONFIG_FILE_NAME)

    # Generate code for all busses in the projects Config
    for bus in config.busses:
        generate_code(bus)
