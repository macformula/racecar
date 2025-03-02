from __future__ import annotations
import json
import yaml
from enum import Enum
from typing import Dict, Any
from database import (
    Bus,
    Node,
    Message,
    Signal,
    IntegralSignal,
    BooleanSignal,
    EnumSignal,
    FloatingSignal,
)


def _parse_signal(signal_data: Dict[str, Any], enum_types: Dict[str, Enum]) -> Signal:
    signal_type = signal_data.get("signal_type")
    if not signal_type:
        raise ValueError("Signal type is required")

    # Create appropriate signal class based on signal type
    signal_classes = {
        "integral": IntegralSignal,
        "boolean": BooleanSignal,
        "enum": EnumSignal,
        "floating": FloatingSignal,
    }

    # Create the signal instance with appropriate context for validation
    parse_context = None
    if signal_type == "enum":
        parse_context = {"enum_types": enum_types}

    return signal_classes[signal_type](signal_data, context=parse_context)


def _parse_message(
    message_data: Dict[str, Any], nodes: Dict[str, Node], enum_types: Dict[str, Enum]
) -> Message:
    # Parse and create a Message object with its signals and node references.

    signal_data_list = message_data.get("signals", [])
    message_data_copy = {k: v for k, v in message_data.items() if k != "signals"}

    message = Message(message_data_copy, context={"nodes": nodes.keys()})

    message.signals = [_parse_signal(s, enum_types) for s in signal_data_list]
    message.sender = nodes[message_data["sender"]]
    message.receivers = [nodes[receiver] for receiver in message_data["receivers"]]

    return message


def parse_bus(data: Dict[str, Any]) -> Bus:
    # Nodes, enum types and bus data are parsed separately as they are passed as down for validation with messages/ signals
    nodes = {node_data["name"]: Node(node_data) for node_data in data.get("nodes", [])}

    enum_types = {
        enum_name: Enum(enum_name, enum_values)
        for enum_name, enum_values in data.get("enum_types", {}).items()
    }

    bus_data = {
        k: v for k, v in data.items() if k not in ["nodes", "enum_types", "messages"]
    }

    bus = Bus(bus_data)

    # Parse messages with node and enum references
    bus.messages = [
        _parse_message(msg, nodes, enum_types) for msg in data.get("messages", [])
    ]

    bus.nodes = list(nodes.values())
    bus.enum_types = enum_types

    return bus


def parse_file(file_path: str) -> Bus:
    with open(file_path, "r") as f:
        if file_path.endswith((".yaml", ".yml")):
            data = yaml.safe_load(f)
        elif file_path.endswith((".json")):
            data = json.load(f)
        else:
            raise ValueError(f"Unsupported file extension: {file_path}")
    return parse_bus(data)


# Example usage:
# python scripts/cangen/cangen/file_parser.py firmware/projects/veh.json
if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python database_file_parser.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    bus = parse_file(file_path)
    print(f"Bus: {bus.name}")
    print(f"Baud Rate: {bus.baud_rate}")
    print(f"\nNodes ({len(bus.nodes)}):")
    for node in bus.nodes:
        print(f"  - {node.name}")
    print(f"\nEnum Types: ({len(bus.enum_types)})")
    for enum_name, enum_type in bus.enum_types.items():
        print(f"  {enum_name} ({len(enum_type.__members__)}):")
        for member in enum_type.__members__.values():
            print(f"    {member}: {member.value}")
    print(f"\nMessages ({len(bus.messages)}):")
    for msg in bus.messages:
        print(f"  - {msg.name} (ID: {msg.id}, Length: {msg.length})")
        print(f"    Sender: {msg.sender.name}")
        print(f"    Receivers: {[receiver.name for receiver in msg.receivers]}")
        print(f"    Signals:")
        for sig in msg.signals:
            print(f"      - {sig.name} ({sig.length} bits)")
