from __future__ import annotations
import yaml

DEFAULT_OUTPUT_DIR = "generated/can"


class Bus:
    def __init__(self, bus: dict):
        self.dbc_file_path: str = bus.pop("dbcFile")
        self.bus_name: str = bus.pop("busName").capitalize()

        if bus:
            raise ValueError(
                f"{bus.keys()} field/s not expected in configuration file for bus {self.bus_name}."
            )


class Config:
    @staticmethod
    def from_yaml(config_file_name: str) -> Config:
        with open(config_file_name, "r") as file:
            config = yaml.safe_load(file)
        return Config(config.pop("canGen"))

    def __init__(self, config: dict):
        self.node = config.pop("ourNode")
        self.output_dir = config.pop("outputPath", DEFAULT_OUTPUT_DIR)

        self.busses = [Bus(bus) for bus in config.pop("busses")]

        if config:
            raise ValueError(
                f"{config.keys()} field/s not expected in configuration file from node."
            )
