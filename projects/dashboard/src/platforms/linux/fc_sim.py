"""
Simulate the FrontController interaction with the Dashboard over CAN.

Only sends DashCommand and receives DashStatus

NOTE: This script must be run from the projects/dashboard directory.
      Use ./run_simulator.sh or ./run.sh to run it correctly.
      Running it directly from this directory will fail due to incorrect relative paths.
"""

from __future__ import annotations

import warnings
from time import sleep
from typing import Callable

import can
import cantools
import cantools.database
import numpy as np

dbc = cantools.database.load_file("../veh.dbc")
fc_msg = dbc.get_message_by_name("DashCommand")
dash_msg = dbc.get_message_by_name("DashStatus")


class Simulation:
    class Restart(BaseException):
        pass

    def __init__(self):
        self.reset_fcstatus()

        self.went_past_logo = False
        self.start_bus()

    def reset_fcstatus(self):
        self.FcStatus = {
            "ConfigReceived": False,
            "HvStarted": False,
            "MotorStarted": False,
            "DriveStarted": False,
            "HvPrechargePercent": 0,
            "HvSocPercent": 0,
            "Speed": 0,
            "Reset": False,
            "Errored": False,
        }

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
            data=fc_msg.encode(self.FcStatus),
        )

        # Continuously send the FcStatus from another thread. We can modify the message data
        # from this thread.
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", UserWarning)

            def modify_data(msg):
                msg.data = fc_msg.encode(self.FcStatus)

            self.bus.send_periodic(msg, period=0.1, modifier_callback=modify_data)

    def get_dash_state(self):
        msg = self.bus.recv()

        ds = dbc.decode_message(msg.arbitration_id, msg.data)

        # Automatically restart the simulation when the developer restarts the
        # Dashboard program.
        if ds["State"] == "LOGO":
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
        DELAY = 0.1  # between screens

        self.reset_fcstatus()

        ds = self.wait_for_dash(
            "Waiting for Profile Selection",
            lambda ds: ds["State"] == "WAIT_SELECTION_ACK",
        )
        print(f"Profile:\t{ds['Profile']}")
        sleep(DELAY)
        self.FcStatus["ConfigReceived"] = True

        self.wait_for_dash(
            "Waiting for cue to start HV",
            lambda ds: ds["State"] == "STARTING_HV",
        )

        # Simulate precharging
        for i in range(0, 100, 5):
            self.FcStatus["HvPrechargePercent"] = i
            sleep(0.01)

        self.FcStatus["HvStarted"] = True

        self.wait_for_dash(
            "Waiting for cue to start Motor",
            lambda ds: ds["State"] == "STARTING_MOTORS",
        )
        sleep(DELAY)
        self.FcStatus["MotorStarted"] = True

        sleep(DELAY)
        print("Brakes pressed")
        self.FcStatus["DriveStarted"] = True

        print("Speeding up")
        hv_soc = 0
        for speed in np.arange(0, 72, 0.1):
            self.FcStatus["Speed"] = speed
            self.FcStatus["HvSocPercent"] = hv_soc
            hv_soc = min(100, hv_soc + 0.5)
            sleep(0.005)

        sleep(5)
        self.FcStatus["Errored"] = True

        self.wait_for_dash(
            "Waiting for shutdown signal", lambda ds: ds["State"] == "SHUTDOWN"
        )

        self.FcStatus["Errored"] = False

        sleep(DELAY)
        self.FcStatus["Reset"] = True

        self.wait_for_dash(
            "Waiting for dash to reset", lambda ds: ds["State"] == "LOGO"
        )
        self.FcStatus["Reset"] = False

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
