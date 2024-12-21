from cangen.traffic import calculate_bus_load


def test_no_messages():
    assert calculate_bus_load([], 500000) == 0
