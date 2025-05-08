"""
Simulate the FrontController interaction with the Dashboard over CAN.

Only sends FCDashboardStatus and receives DashboardStatus
"""

from __future__ import annotations

import warnings
from time import sleep
from typing import Callable

import can
import cantools
import cantools.database
import numpy as np

dbc = cantools.database.load_file("../../../veh.dbc")
fc_msg = dbc.get_message_by_name("FCDashboardStatus")
dash_msg = dbc.get_message_by_name("DashboardStatus")


class Simulation:
    class Restart(BaseException):
        pass

    def __init__(self):
        self.fc_status = {
            "receiveConfig": False,
            "hvStarted": False,
            "motorStarted": False,
            "driveStarted": False,
            "hv_charge_percent": 0,
            "speed": 0,
        }

        self.went_past_logo = False
        self.start_bus()

    def start_bus(self):
        try:
            self.bus = can.Bus(interface="socketcan", channel="vcan0")
        except can.CanInitializationError:
            print(
                "Failed to open SocketCAN vcan0.\n"
                "Does your system have SocketCAN? Are the modules loaded? "
                "Is the vcan0 channel up?"
            )
            exit(1)

        self.bus.set_filters(
            [{"can_id": dash_msg.frame_id, "can_mask": 0x7FF, "extended": False}]
        )

        msg = can.Message(
            arbitration_id=fc_msg.frame_id,
            is_extended_id=False,
            data=fc_msg.encode(self.fc_status),
        )

        # Continuously send the FcStatus from another thread. We can modify the message data
        # from this thread.
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", UserWarning)

            def modify_data(msg):
                msg.data = fc_msg.encode(self.fc_status)

            self.bus.send_periodic(msg, period=0.1, modifier_callback=modify_data)

    def get_dash_state(self):
        msg = self.bus.recv()

        ds = dbc.decode_message(msg.arbitration_id, msg.data)

        # Automatically restart the simulation when the developer restarts the
        # Dashboard program.
        if ds["DashState"] == "LOGO":
            if self.went_past_logo:
                raise Simulation.Restart
        else:
            self.went_past_logo = True
        return ds

    def shutdown(self):
        self.bus.shutdown()

    def wait_for_dash(self, msg: str, condition: Callable[[dict], bool]):
        """
        `condition` must be a function that take a DashStatus object and returns T/F
        """
        print(msg, end="", flush=True)

        complete = False
        while not complete:
            ds = self.get_dash_state()
            complete = condition(ds)

        print("\t\t[Done]")
        return ds

    def run(self):
        DELAY = 0.5  # between screens

        ds = self.wait_for_dash(
            "Waiting for Driver/Event Selection",
            lambda ds: ds["DashState"] == "WAIT_SELECTION_ACK",
        )
        print(f"Driver:\t{ds['Driver']}")
        print(f"Event:\t{ds['Event']}")
        sleep(DELAY)
        self.fc_status["receiveConfig"] = True

        self.wait_for_dash(
            "Waiting for cue to start HV",
            lambda ds: ds["DashState"] == "STARTING_HV",
        )

        # Simulate precharging
        for i in range(100):
            self.fc_status["hv_charge_percent"] = i
            sleep(0.01)

        self.fc_status["hvStarted"] = True

        self.wait_for_dash(
            "Waiting for cue to start Motor",
            lambda ds: ds["DashState"] == "STARTING_MOTORS",
        )
        sleep(DELAY)
        self.fc_status["motorStarted"] = True

        sleep(DELAY)
        print("Brakes pressed")
        self.fc_status["driveStarted"] = True

        print("Speeding up")
        for speed in np.arange(0, 72, 0.1):
            self.fc_status["speed"] = speed
            speed += 0.1
            sleep(0.005)

        self.wait_for_dash("Sequence complete.", lambda ds: False)


if __name__ == "__main__":
    while True:
        sim = Simulation()
        try:
            sim.run()
        except Simulation.Restart:
            sim.shutdown()
            print("\n\nRestarting Simulation\n")
            continue
        except KeyboardInterrupt:
            print("\n\nEnding simulation")
            break
