"""Print an encoded DashCommand sample without CAN hardware."""
from pathlib import Path

import cantools


db = cantools.database.load_file(Path(__file__).resolve().parents[1] / "dbc/veh.dbc")
message = db.get_message_by_name("DashCommand")
data = message.encode({
    "Speed": 30,
    "HvSocPercent": 50,
    "ConfigReceived": 1,
    "HvStarted": 1,
    "MotorStarted": 1,
    "DriveStarted": 1,
    "Errored": 0,
    "HvPrechargePercent": 50,
})
print(data.hex())
