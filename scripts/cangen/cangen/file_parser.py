from __future__ import annotations
import json
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
    Endian,
)
from database_config import BusConfig, NodeConfig, EnumTypesConfig


def _parse_signal(signal_data: Dict[str, Any], enum_types: Dict[str, Enum]) -> Signal:
    base_attrs = {
        "name": signal_data["name"],
        "start_bit": signal_data["start_bit"],
        "length": signal_data["length"],
        "endianness": Endian[signal_data["endianness"]],
        "description": signal_data.get("description"),
        "additional_receivers": signal_data.get("additional_receivers"),
    }

    signal_type = signal_data["signal_type"]
    if signal_type == "integral":
        return IntegralSignal(
            **base_attrs,
            is_signed=signal_data.get("is_signed", False),
            value_range=signal_data.get("value_range", (0, 0)),
        )
    elif signal_type == "boolean":
        return BooleanSignal(**base_attrs)
    elif signal_type == "enum":
        return EnumSignal(**base_attrs, enum_config=signal_data["enum_config"])
    elif signal_type == "floating":
        return FloatingSignal(
            **base_attrs,
            is_signed=signal_data.get("is_signed", False),
            value_range=signal_data.get("value_range"),
            scale=signal_data.get("scale"),
            offset=signal_data.get("offset"),
            unit=signal_data.get("unit"),
        )
    else:
        raise ValueError(f"Unknown signal type: {signal_type}")


def _parse_message(
    message_data: Dict[str, Any], nodes: Dict[str, Node], enum_types: Dict[str, Enum]
) -> Message:
    return Message(
        name=message_data["name"],
        id=message_data["id"],
        is_extended_id=message_data["is_extended_id"],
        length=message_data["length"],
        sender=nodes[message_data["sender"]],
        signals=[_parse_signal(s, enum_types) for s in message_data["signals"]],
        receivers=[nodes[receiver] for receiver in message_data["receivers"]],
        frequency=message_data["frequency"],
        description=message_data.get("description"),
    )


def parse_bus(data: Dict[str, Any]) -> Bus:
    validated_nodes = [NodeConfig.validate(node) for node in data["nodes"]]
    nodes = {
        node["name"]: Node(name=node["name"], description=node.get("description"))
        for node in validated_nodes
    }

    validated_enum_types = EnumTypesConfig.validate(data)
    enum_types = {
        enum_name: Enum(enum_name, validated_enum_types[enum_name])
        for enum_name in validated_enum_types
    }

    validated_bus = BusConfig.validate(
        data, context={"nodes": nodes, "enum_types": enum_types}
    )
    return Bus(
        name=validated_bus["name"],
        baud_rate=validated_bus["baud_rate"],
        nodes=list(nodes.values()),
        messages=[
            _parse_message(message, nodes, enum_types)
            for message in validated_bus["messages"]
        ],
        enum_types=enum_types,
        description=validated_bus.get("description"),
    )


def parse_file(file_path: str) -> Bus:
    with open(file_path, "r") as f:
        data = json.load(f)
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
        for name, member in enum_type.__members__.items():
            print(f"    {member}: {member.value}")
    print(f"\nMessages ({len(bus.messages)}):")
    for msg in bus.messages:
        print(f"  - {msg.name} (ID: {msg.id}, Length: {msg.length})")
        print(f"    Sender: {msg.sender}")
        print(f"    Signals:")
        for sig in msg.signals:
            print(f"      - {sig.name} ({sig.length} bits)")
