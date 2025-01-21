#include "battery_monitor.h"

#include <iostream>

// Constructor
BatteryMonitor::BatteryMonitor()
    : current_status(BmStatus::INIT),
      snapshot_time_ms(0),
      bm_control_status(BmControlStatus::STARTUP_CMD) {}

// Update method
BmOutput BatteryMonitor::update(const BmInput& input, int time_ms) {
    BmOutput output;

    // BM Status Update
    BmStatus status = bmStatusUpdate(input, time_ms);

    // BM Control Update
    ContactorCMD contactor_cmd = bmControlUpdate(status, time_ms);

    // Output
    return BmOutput{.status = status, .contactor = contactor_cmd};
}

// Control update method
BmStatus BatteryMonitor::bmStatusUpdate(const BmInput& input, int time_ms) {
    std::cout << "[DEBUG UPDATE] Control state: "
              << static_cast<int>(current_status) << ", Time: " << time_ms
              << " ms\n";

    if (input.pack_soc < 30) {
        current_status = BmStatus::LOW_SOC;
        std::cout << "[DEBUG UPDATE] Transitioning from IDLE to LOW_SOC\n";
    } else if (input.hvil_status == OPEN) {
        current_status = BmStatus::HVIL_INTERRUPT;
        snapshot_time_ms = time_ms;
        std::cout
            << "[DEBUG UPDATE] Transitioning from IDLE to HVIL_INTERRUPT\n";
    }

    switch (current_status) {
        case BmStatus::INIT:
            if (input.cmd == BmCmd::HV_STARTUP &&
                input.precharge_contactor_states == OPEN &&
                input.hv_pos_contactor_states == OPEN &&
                input.hv_neg_contactor_states == OPEN) {
                current_status = BmStatus::IDLE;
                std::cout << "[DEBUG UPDATE] Transitioning from INIT to IDLE\n";
            }
            break;

        case BmStatus::IDLE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                current_status = BmStatus::STARTUP;
                std::cout
                    << "[DEBUG UPDATE] Transitioning from IDLE to STARTUP\n";
                break;
            }
            break;

        case BmStatus::STARTUP:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == OPEN) {
                current_status = BmStatus::INIT_PRECHARGE;
                std::cout << "[DEBUG UPDATE] Transitioning from STARTUP to "
                             "INIT_PRECHARGE\n";
                break;
            }
            break;

        case BmStatus::INIT_PRECHARGE:
            if (input.precharge_contactor_states == CLOSED &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                current_status = BmStatus::PRECHARGE;
                std::cout << "[DEBUG UPDATE] Transitioning from INIT_PRECHARGE "
                             "to PRECHARGE\n";
                break;
            }

            break;

        case BmStatus::PRECHARGE:
            if (input.precharge_contactor_states == OPEN &&
                input.hv_neg_contactor_states == CLOSED &&
                input.hv_pos_contactor_states == CLOSED) {
                current_status = BmStatus::RUNNING;
                std::cout << "[DEBUG UPDATE] Transitioning from PRECHARGE to "
                             "RUNNING\n";
                break;
            }

            break;

        case BmStatus::RUNNING:
            if (input.precharge_contactor_states == CLOSED ||
                input.hv_neg_contactor_states == OPEN ||
                input.hv_pos_contactor_states == OPEN) {
                // std::cout << "Invalid\n";
                current_status = BmStatus::INIT;

                break;
            }
            break;

        case BmStatus::HVIL_INTERRUPT:
            if (time_ms - snapshot_time_ms >= 5000) {
                // TODO: do we need to check every 5 seconds
                if (input.hvil_status) {
                    std::cout
                        << "[DEBUG UPDATE] HVIL restored after interruption\n";
                    current_status = BmStatus::INIT;
                    break;
                }
            }
            break;

        case BmStatus::LOW_SOC:
            std::cout << "[DEBUG UPDATE] In LOW_SOC state\n";
            break;
    }

    return current_status;
}

ContactorCMD BatteryMonitor::bmControlUpdate(BmStatus status, int time_ms) {
    ContactorCMD contactor_cmd = {OPEN, OPEN, OPEN};

    if (current_status == BmStatus::HVIL_INTERRUPT ||
        current_status == BmStatus::LOW_SOC) {
        bm_control_status = BmControlStatus::STARTUP_CMD;
        std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                     "CLOSE_HV_NEG to STARTUP_CMD\n";
    }

    switch (bm_control_status) {
        case BmControlStatus::STARTUP_CMD:
            if (current_status == BmStatus::IDLE) {
                bm_control_status = BmControlStatus::CLOSE_HV_NEG;
                snapshot_time_ms = time_ms;
                std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                             "STARTUP_CMD to CLOSE_HV_NEG\n";
                break;
            }
            break;

        case BmControlStatus::CLOSE_HV_NEG:

            if (time_ms - snapshot_time_ms >= 100) {
                if (current_status == BmStatus::STARTUP) {
                    bm_control_status = BmControlStatus::CLOSE_PRECHARGE;
                    snapshot_time_ms = time_ms;
                    std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                                 "CLOSE_HV_NEG to CLOSE_PRECHARGE\n";
                    break;
                }
            }
            break;

        case BmControlStatus::CLOSE_PRECHARGE:
            if (time_ms - snapshot_time_ms >= 6500) {
                if (current_status == BmStatus::INIT_PRECHARGE) {
                    bm_control_status = BmControlStatus::CLOSE_HV_POS;
                    snapshot_time_ms = time_ms;
                    std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                                 "CLOSE_PRECHARGE to CLOSE_HV_POS\n";
                    break;
                }
            }
            break;

        case BmControlStatus::CLOSE_HV_POS:
            if (time_ms - snapshot_time_ms >= 100) {
                if (current_status == BmStatus::PRECHARGE) {
                    bm_control_status = BmControlStatus::OPEN_PRECHARGE;
                    snapshot_time_ms = time_ms;
                    std::cout << "[DEBUG BM CONTROL UPDATE] Transitioning from "
                                 "CLOSE_HV_POS to OPEN_PRECHARGE\n";
                    break;
                }
            }

            break;

        case BmControlStatus::OPEN_PRECHARGE:
            if (contactor_cmd.precharge_contactor_cmd == CLOSED ||
                contactor_cmd.hv_neg_contactor_cmd == OPEN ||
                contactor_cmd.hv_pos_contactor_cmd == OPEN ||
                current_status != BmStatus::PRECHARGE) {
                // std::cout << "Invalid\n";
                current_status = BmStatus::INIT;
                break;
            }

            break;
    }

    // Entry Actions
    switch (bm_control_status) {
        case BmControlStatus::STARTUP_CMD:
            contactor_cmd.precharge_contactor_cmd = OPEN;
            contactor_cmd.hv_neg_contactor_cmd = OPEN;
            contactor_cmd.hv_pos_contactor_cmd = OPEN;
            break;

        case BmControlStatus::CLOSE_HV_NEG:
            contactor_cmd.precharge_contactor_cmd = OPEN;
            contactor_cmd.hv_neg_contactor_cmd = CLOSED;
            contactor_cmd.hv_pos_contactor_cmd = OPEN;
            break;

        case BmControlStatus::CLOSE_PRECHARGE:
            contactor_cmd.precharge_contactor_cmd = CLOSED;
            contactor_cmd.hv_neg_contactor_cmd = CLOSED;
            contactor_cmd.hv_pos_contactor_cmd = OPEN;
            break;

        case BmControlStatus::CLOSE_HV_POS:
            contactor_cmd.precharge_contactor_cmd = CLOSED;
            contactor_cmd.hv_neg_contactor_cmd = CLOSED;
            contactor_cmd.hv_pos_contactor_cmd = CLOSED;
            break;

        case BmControlStatus::OPEN_PRECHARGE:
            contactor_cmd.precharge_contactor_cmd = OPEN;
            contactor_cmd.hv_neg_contactor_cmd = CLOSED;
            contactor_cmd.hv_pos_contactor_cmd = CLOSED;
            break;
    }

    return contactor_cmd;
}
