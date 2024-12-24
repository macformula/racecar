from cangen.traffic import calculate_bus_load, CANMessage
import pytest


def test_no_messages():
    assert (calculate_bus_load([], 500000)) == pytest.approx(0, rel=1e-5)

def test_normal_messages():
    assert calculate_bus_load(
        [
            CANMessage(data_length=8, frequency=100),
            CANMessage(data_length=5, frequency=50),
        ],
        500000,
    ) == pytest.approx(3.60, rel=1e-2)

def test_bus_overload():
    assert calculate_bus_load(
        [
            CANMessage(data_length=64, frequency=100),
            CANMessage(data_length=32, frequency=200),
        ],
        100000,
    ) == pytest.approx(138.72, rel=1e-5)

def test_high_traffic():
    assert calculate_bus_load(
        [
            CANMessage(data_length=8, frequency=1000),
            CANMessage(data_length=4, frequency=800),
        ],
        1000000,
    ) == pytest.approx(20.256, rel=1e-5)

def test_zero_speed():
    with pytest.raises(ValueError):
        assert calculate_bus_load(
            [
                CANMessage(data_length=8, frequency=1000),
                CANMessage(data_length=4, frequency=800),
            ],
            0,
        ) == pytest.approx(0, rel=1e-5)

def test_negative_speed():
    with pytest.raises(ValueError):
        assert (
        calculate_bus_load(
            [
                CANMessage(data_length=8, frequency=1000),
                CANMessage(data_length=4, frequency=800),
            ],
            -500000,
        )
    ) == pytest.approx(0, rel=1e-5)
