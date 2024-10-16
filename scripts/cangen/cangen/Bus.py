from dataclasses import dataclass


@dataclass
class Bus:
    dbc_file_path: str
    our_node: str
    bus_name: str
