#include "battery_monitor.h"

#include <iostream>

// Constructor
BatteryMonitor::BatteryMonitor()
    : current_status_(BmStatus::INIT),
      snapshot_time_ms_(0),
      bm_control_status_(BmControlStatus::STARTUP_CMD) {}

// Update method
BmOutput BatteryMonitor::update(const BmInput& input, int time_ms) {
    BmOutput output;

    // BM Status Update
    BmStatus status = bmStatusUpdate(input, time_ms);

    // BM Control Update
    BmControlStatus new_control_status = bmControlTransition(status, time_ms);
    ContactorCMD contactor_cmd = setContactorCMD(new_control_status);

    // Output
    return BmOutput{.status = status, .contactor = contactor_cmd};
}

// Control update method
BmStatus BatteryMonitor::bmStatusUpdate(const BmInput& input, int time_ms) {
    using enum ContactorState;
    std::cout << "[DEBUG UPDATE] Control state: "
              << static_cast<int>(current_status_) << ", Time: " << time_ms
              << " ms\n";

    if (input.pack_soc < 30) {
        current_status_ = BmStatus::LOW_SOC;
        std::cout << "[DEBUG UPDATE] Transitioning from IDLE to LOW_SOC\n";
    } else if (input.hvil_status == OPEN) {
        current_status_ = BmStatus::HVIL_INTERRUPT;
        snapshot_time_ms_ = time_ms;
        std::cout
            << "[DEBUG UPDATE] Transitioning from IDLE to HVIL_INTERRUPT\n";
    }

    switch (current_status_) {
        case BmStatus::INIT:
            if (input.cmd == BmCmd::HV_STARTUP &&
                input.precharge_contactor_states == OPEN &&
                input.hv_pos_contactor_states == OPEN &&
                input.hv_neg_contactor_states == OPEN) {
                current_status_ = BmStatus::IDLE;
                std::cout << "[DEBUG UPDATE] Transitioning from INIT to IDLE\n";
            }
            break;

        case BmStatus::IDLE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                current_status_ = BmStatus::STARTUP;
                std::cout
                    << "[DEBUG UPDATE] Transitioning from IDLE to STARTUP\n";
            }
            break;

        case BmStatus::STARTUP:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                current_status_ = BmStatus::INIT_PRECHARGE;
                std::cout << "[DEBUG UPDATE] Transitioning from STARTUP to "
                             "INIT_PRECHARGE\n";
            }
            break;

        case BmStatus::INIT_PRECHARGE:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                current_status_ = BmStatus::PRECHARGE;
                std::cout << "[DEBUG UPDATE] Transitioning from INIT_PRECHARGE "
                             "to PRECHARGE\n";
            }

            break;

        case BmStatus::PRECHARGE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                current_status_ = BmStatus::RUNNING;
                std::cout << "[DEBUG UPDATE] Transitioning from PRECHARGE to "
                             "RUNNING\n";
            }

            break;

        case BmStatus::RUNNING:
            if (input.precharge_contactor_states == CLOSED ||
                input.hv_neg_contactor_states == OPEN ||
                input.hv_pos_contactor_states == OPEN) {
                // std::cout << "Invalid\n";
                current_status_ = BmStatus::INIT;
            }
            break;

        case BmStatus::HVIL_INTERRUPT:
            if (time_ms - snapshot_time_ms_ >= 5000 &&
                input.hvil_status == CLOSED) {
                // TODO: do we need to check every 5 seconds

                std::cout
                    << "[DEBUG UPDATE] HVIL restored after interruption\n";
                current_status_ = BmStatus::INIT;
            }
            break;

        case BmStatus::LOW_SOC:
            std::cout << "[DEBUG UPDATE] In LOW_SOC state\n";
            break;
    }

    return current_status_;
}

BmControlStatus BatteryMonitor::bmControlTransition(BmStatus status,
                                                    int time_ms) {
    using enum ContactorState;
    // ContactorCMD contactor_cmd = {OPEN, OPEN, OPEN};

    BmControlStatus new_state = bm_control_status_;

    if (status == BmStatus::HVIL_INTERRUPT || status == BmStatus::LOW_SOC) {
        new_state = BmControlStatus::STARTUP_CMD;
        std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                     "CLOSE_HV_NEG to STARTUP_CMD\n";
    }

    switch (bm_control_status_) {
        case BmControlStatus::STARTUP_CMD:
            if (status == BmStatus::IDLE) {
                new_state = BmControlStatus::CLOSE_HV_NEG;
                std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                             "STARTUP_CMD to CLOSE_HV_NEG\n";
            }
            break;

        case BmControlStatus::CLOSE_HV_NEG:

            if (time_ms - snapshot_time_ms_ >= 100 &&
                status == BmStatus::STARTUP) {
                new_state = BmControlStatus::CLOSE_PRECHARGE;
                std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                             "CLOSE_HV_NEG to CLOSE_PRECHARGE\n";
            }
            break;

        case BmControlStatus::CLOSE_PRECHARGE:
            if (time_ms - snapshot_time_ms_ >= 6500 &&
                status == BmStatus::INIT_PRECHARGE) {
                new_state = BmControlStatus::CLOSE_HV_POS;
                std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                             "CLOSE_PRECHARGE to CLOSE_HV_POS\n";
            }
            break;

        case BmControlStatus::CLOSE_HV_POS:
            if (time_ms - snapshot_time_ms_ >= 100 &&
                status == BmStatus::PRECHARGE) {
                new_state = BmControlStatus::OPEN_PRECHARGE;
                std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                             "CLOSE_HV_POS to OPEN_PRECHARGE\n";
            }

            break;

        case BmControlStatus::OPEN_PRECHARGE:
            if (status != BmStatus::PRECHARGE) {
                status = BmStatus::INIT;
            }
            break;
    }

    if (new_state != bm_control_status_) {
        bm_control_status_ = new_state;
        snapshot_time_ms_ = time_ms;
    }

    return new_state;
}

ContactorCMD BatteryMonitor::setContactorCMD(
    BmControlStatus bm_control_status_) {
    using enum ContactorState;

    switch (bm_control_status_) {
        case BmControlStatus::STARTUP_CMD:
            return ContactorCMD{OPEN, OPEN, OPEN};

        case BmControlStatus::CLOSE_HV_NEG:
            return ContactorCMD{OPEN, OPEN, CLOSED};

        case BmControlStatus::CLOSE_PRECHARGE:
            return ContactorCMD{CLOSED, OPEN, CLOSED};

        case BmControlStatus::CLOSE_HV_POS:
            return ContactorCMD{CLOSED, CLOSED, CLOSED};

        case BmControlStatus::OPEN_PRECHARGE:
            return ContactorCMD{OPEN, CLOSED, CLOSED};
    }

    return ContactorCMD{OPEN, OPEN, OPEN};
}