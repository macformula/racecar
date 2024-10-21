import yaml


class Bus:
    def __init__(self, bus: dict):
        self.bus = bus

        self.dbc_file_path: str = self.bus.pop("dbcFile")
        self.bus_name: str = self.bus.pop("busName").capitalize()

        if not self.has_bus_been_consumed():
            print(self.bus)
            print(
                f"{self.bus.keys()} field/s not expected in configuration file for bus {self.bus_name}."
            )
            raise ValueError

    # Checks to see if all fields of inputted bus object have been used
    def has_bus_been_consumed(self) -> bool:
        return not self.bus


class Config:
    @staticmethod
    def load_yaml(config_file_name: str) -> dict:
        with open(config_file_name, "r") as file:
            config = yaml.safe_load(file)
        return config.pop("canGen")

    def __init__(self, config_file_name: str):
        self.config = Config.load_yaml(config_file_name)
        self.node = self.config.pop("ourNode")

        # Create a bus object for all busses in the config file
        self.busses = [Bus(bus) for bus in self.config.pop("busses")]

        # Checks to make sure config object is empty, all fields have been popped
        if not self.has_config_been_consumed():
            print(
                f"{self.config.keys()} field/s not expected in configuration file from node."
            )
            raise ValueError

    # Checks to see if the instances config object is empty,
    # which implies every field in config has been used
    def has_config_been_consumed(self) -> bool:
        return not self.config
