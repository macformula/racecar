#include "battery_monitor.h"

#include <iostream>

// Constructor
BatteryMonitor::BatteryMonitor() : current_status(BmStatus::INIT) {}

// Update method
BmOutput BatteryMonitor::update(const BmInput& input, int time_ms) {
    BmOutput output;
    output.contactor = {false, false, false};

    std::cout << "[DEBUG] Current state: " << static_cast<int>(current_status)
              << ", Time: " << time_ms << " ms\n";

    static int snapshot_time_ms = 0;

    switch (current_status) {
        case BmStatus::INIT:
            if (input.cmd == BmCmd::HV_STARTUP &&
                input.precharge_contactor_states == false &&
                input.hv_pos_contactor_states == false &&
                input.hv_neg_contactor_states == false &&
                input.hvil_status == true) {
                current_status = BmStatus::IDLE;
                std::cout << "[DEBUG] Transitioning from INIT to IDLE\n";
            }
            break;

        case BmStatus::IDLE:
            if (input.pack_soc < 30) {
                current_status = BmStatus::LOW_SOC;
                std::cout << "[DEBUG] Transitioning from IDLE to LOW_SOC\n";
                break;
            }

            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout
                    << "[DEBUG] Transitioning from IDLE to HVIL_INTERRUPT\n";
                break;
            }

            if (input.precharge_contactor_states == false &&
                input.hv_neg_contactor_states == true &&
                input.hv_pos_contactor_states == false) {
                current_status = BmStatus::STARTUP;
                std::cout << "[DEBUG] Transitioning from IDLE to STARTUP\n";
                break;
            }
            break;

        case BmStatus::STARTUP:
            if (input.pack_soc < 30) {
                current_status = BmStatus::LOW_SOC;
                std::cout << "[DEBUG] Transitioning from STARTUP to LOW_SOC\n";
                break;
            }

            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout
                    << "[DEBUG] Transitioning from STARTUP to HVIL_INTERRUPT\n";
                break;
            }

            if (input.precharge_contactor_states == true &&
                input.hv_neg_contactor_states == true &&
                input.hv_pos_contactor_states == false) {
                current_status = BmStatus::INIT_PRECHARGE;
                std::cout
                    << "[DEBUG] Transitioning from STARTUP to INIT_PRECHARGE\n";
                break;
            }

            break;

        case BmStatus::INIT_PRECHARGE:
            if (input.pack_soc < 30) {
                current_status = BmStatus::LOW_SOC;
                std::cout
                    << "[DEBUG] Transitioning from INIT_PRECHARGE to LOW_SOC\n";

                break;
            }
            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout << "[DEBUG] Transitioning from INIT_PRECHARGE to "
                             "HVIL_INTERRUPT\n";
                break;
            }
            if (input.precharge_contactor_states == true &&
                input.hv_neg_contactor_states == true &&
                input.hv_pos_contactor_states == true) {
                current_status = BmStatus::PRECHARGE;
                std::cout << "[DEBUG] Transitioning from INIT_PRECHARGE to "
                             "PRECHARGE\n";
                break;
            }

            break;

        case BmStatus::PRECHARGE:
            if (input.pack_soc < 30) {
                current_status = BmStatus::LOW_SOC;
                std::cout
                    << "[DEBUG] Transitioning from PRECHARGE to LOW_SOC\n";
                break;
            }

            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout << "[DEBUG] Transitioning from PRECHARGE to "
                             "HVIL_INTERRUPT\n";
                break;
            }
            if (input.precharge_contactor_states == false &&
                input.hv_neg_contactor_states == true &&
                input.hv_pos_contactor_states == true) {
                current_status = BmStatus::RUNNING;
                std::cout
                    << "[DEBUG] Transitioning from PRECHARGE to RUNNING\n";
                break;
            }

            break;

        case BmStatus::RUNNING:
            if (input.pack_soc < 30) {
                current_status = BmStatus::LOW_SOC;
                std::cout << "[DEBUG] Transitioning from RUNNING to LOW_SOC\n";
                break;
            }
            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout
                    << "[DEBUG] Transitioning from RUNNING to HVIL_INTERRUPT\n";
                break;
            }
            if (input.precharge_contactor_states != false ||
                input.hv_neg_contactor_states != true ||
                input.hv_pos_contactor_states != true) {
                std::cout << "Invalid\n";
                // TODO: Make state transition
                break;
            }
            break;

        case BmStatus::HVIL_INTERRUPT:
            if (time_ms - snapshot_time_ms >= 5000) {
                // TODO: do we need to check every 5 seconds
                if (input.hvil_status) {
                    std::cout << "[DEBUG] HVIL restored after interruption\n";
                    current_status = BmStatus::INIT;
                    break;
                }
            }
            break;

        case BmStatus::LOW_SOC:
            if (input.hvil_status == false) {
                current_status = BmStatus::HVIL_INTERRUPT;
                snapshot_time_ms = time_ms;
                std::cout
                    << "[DEBUG] Transitioning from RUNNING to HVIL_INTERRUPT\n";
                break;
            }
            break;

        default:
            std::cout << "[ERROR] Unknown state encountered!\n";
            break;
    }

    // Set output status and commands based on current state
    output.status = current_status;
    output.contactor = bmControlUpdate(current_status);

    std::cout << "[DEBUG] Output status: " << static_cast<int>(output.status)
              << "\n";
    return output;
}

ContactorCMD BatteryMonitor::bmControlUpdate(BmStatus status, int time_ms) {
    enum class BmControlStatus {
        STARUP_CMD,
        CLOSE_HV_NEG,
        CLOSE_PRECHARGE,
        CLOSE_HV_POS,
        OPEN_PRECHARGE,
    };

    ContactorCMD contactor_cmd = {false, false, false};

    static BmControlStatus bm_control_status = BmControlStatus::STARUP_CMD;
    static int snapshot_time_ms = 0;

    switch (bm_control_status) {
        case BmControlStatus::STARUP_CMD:

            contactor_cmd.precharge_contactor_cmd = false;
            contactor_cmd.hv_neg_contactor_cmd = false;
            contactor_cmd.hv_pos_contactor_cmd = false;

            if (current_status == BmStatus::IDLE) {
                bm_control_status = BmControlStatus::CLOSE_HV_NEG;
                snapshot_time_ms = time_ms;
                break;
            }
            break;

        case BmControlStatus::CLOSE_HV_NEG:

            contactor_cmd.precharge_contactor_cmd = false;
            contactor_cmd.hv_neg_contactor_cmd = true;
            contactor_cmd.hv_pos_contactor_cmd = false;

            if (current_status == BmStatus::HVIL_INTERRUPT ||
                current_status == BmStatus::LOW_SOC) {
                bm_control_status = BmControlStatus::STARUP_CMD;
            }

            if (time_ms - snapshot_time_ms >= 100) {
                if (current_status == BmStatus::STARTUP) {
                    bm_control_status = BmControlStatus::CLOSE_PRECHARGE;
                    snapshot_time_ms = time_ms;
                    break;
                }
            }

            break;

        case BmControlStatus::CLOSE_PRECHARGE:

            contactor_cmd.precharge_contactor_cmd = true;
            contactor_cmd.hv_neg_contactor_cmd = true;
            contactor_cmd.hv_pos_contactor_cmd = false;

            if (current_status == BmStatus::HVIL_INTERRUPT ||
                current_status == BmStatus::LOW_SOC) {
                bm_control_status = BmControlStatus::STARUP_CMD;
            }

            if (time_ms - snapshot_time_ms >= 6500) {
                if (current_status == BmStatus::INIT_PRECHARGE) {
                    bm_control_status = BmControlStatus::CLOSE_HV_POS;
                    snapshot_time_ms = time_ms;
                    break;
                }
            }

            break;

        case BmControlStatus::CLOSE_HV_POS:

            contactor_cmd.precharge_contactor_cmd = true;
            contactor_cmd.hv_neg_contactor_cmd = true;
            contactor_cmd.hv_pos_contactor_cmd = true;

            if (current_status == BmStatus::HVIL_INTERRUPT ||
                current_status == BmStatus::LOW_SOC) {
                bm_control_status = BmControlStatus::STARUP_CMD;
            }

            if (time_ms - snapshot_time_ms >= 100) {
                if (current_status == BmStatus::PRECHARGE) {
                    bm_control_status = BmControlStatus::OPEN_PRECHARGE;
                    snapshot_time_ms = time_ms;
                    break;
                }
            }

            break;

        case BmControlStatus::OPEN_PRECHARGE:
            contactor_cmd.precharge_contactor_cmd = false;
            contactor_cmd.hv_neg_contactor_cmd = true;
            contactor_cmd.hv_pos_contactor_cmd = true;

            if (current_status == BmStatus::HVIL_INTERRUPT ||
                current_status == BmStatus::LOW_SOC) {
                bm_control_status = BmControlStatus::STARUP_CMD;
            }

            break;

        default:
            std::cout << "[ERROR] Unknown state encountered!\n";
            break;
    }

    return contactor_cmd;
}
