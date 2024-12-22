from cangen.traffic import calculate_bus_load, CANMessage
import pytest


def test_no_messages():
    assert calculate_bus_load(
        [
            CANMessage(data_length=8, frequency=100),
            CANMessage(data_length=5, frequency=50),
        ],
        500000,
    ) == pytest.approx(3.60, rel=1e-2)

    # zero frequency or negative frequency
    assert calculate_bus_load(
        [
            CANMessage(data_length=8, frequency=0),
            CANMessage(data_length=10, frequency=-100),
        ],
        500000,
    ) == pytest.approx(0, rel=1e-2)

    # bus overload
    assert calculate_bus_load(
        [
            CANMessage(data_length=64, frequency=100),
            CANMessage(data_length=32, frequency=200),
        ],
        100000,
    ) == pytest.approx(138.72, rel=1e-5)

    # high traffic - high frequency messages
    assert calculate_bus_load(
        [
            CANMessage(data_length=8, frequency=1000),
            CANMessage(data_length=4, frequency=800),
        ],
        1000000,
    ) == pytest.approx(20.26, rel=1e-5)

    # zero can speed
    try:
        assert calculate_bus_load(
            [
                CANMessage(data_length=8, frequency=1000),
                CANMessage(data_length=4, frequency=800),
            ],
            0,
        ) == pytest.approx(0, rel=1e-5)
    except ZeroDivisionError:
        print("Cannot divide by zero baud")

    # negative can speed
    assert (
        calculate_bus_load(
            [
                CANMessage(data_length=8, frequency=1000),
                CANMessage(data_length=4, frequency=800),
            ],
            -500000,
        )
    ) == pytest.approx(0, rel=1e-5)

    # no message
    assert (calculate_bus_load([], 500000)) == pytest.approx(0, rel=1e-5)
