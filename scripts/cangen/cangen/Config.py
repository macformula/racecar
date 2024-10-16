import yaml
from .Bus import Bus


class Config:
    OUTPUT_PATH = "generated/can"

    def __init__(self, config_file_name: str) -> None:
        with open(config_file_name, "r") as file:
            config = yaml.safe_load(file)

        # Save the config file info as the Config instances attribute
        self.config = config["canGen"]
        # Create a bus object for all busses in the config file
        self.busses = [
            Bus(bus["dbcFile"], self.config["ourNode"], bus["busName"].capitalize())
            for bus in self.config["busses"]
        ]
