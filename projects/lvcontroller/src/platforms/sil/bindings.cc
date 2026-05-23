/// @author Noah Jaye
/// @date 2025-10-31

#include "bindings.hpp"

#include <netinet/in.h>
#include <sys/socket.h>

#include <chrono>
#include <thread>

#include "cobs.hpp"
#include "lvcontroller.pb.h"
#include "mcal/sil/analog_input.hpp"
#include "mcal/sil/analog_output.hpp"
#include "mcal/sil/can.hpp"
#include "mcal/sil/gpio.hpp"
#include "mcal/sil/pwm.hpp"
#include "pb_decode.h"
#include "pb_encode.h"
lvcontroller_Output outputs = lvcontroller_Output_init_zero;
lvcontroller_Input inputs = lvcontroller_Input_init_zero;
std::thread wt;
std::thread rt;

namespace mcal {
using namespace mcal::sil;
// Tractive System Status Indicator
DigitalOutput tssi_en{&outputs.tssi_en};
DigitalOutput tssi_red_signal{&outputs.tssi_red_signal};
DigitalOutput tssi_green_signal{&outputs.tssi_green_signal};
DigitalInput imd_fault{&inputs.imd_fault};
DigitalInput bms_fault{&inputs.bms_fault};

// Powertrain Cooling
DigitalOutput powertrain_pump1_en{&outputs.powertrain_pump1_en};
DigitalOutput powertrain_pump2_en{&outputs.powertrain_pump2_en};
DigitalOutput powertrain_fan1_en{&outputs.powertrain_fan1_en};
DigitalOutput powertrain_fan2_en{&outputs.powertrain_fan2_en};
PWMOutput powertrain_fan_pwm{&outputs.powertrain_fan_pwm};

// Motor Controller (i.e. Inverters)
DigitalOutput motor_controller_en{&outputs.motor_controller_en};
DigitalOutput motor_ctrl_precharge_en{&outputs.motor_ctrl_precharge_en};
DigitalOutput motor_ctrl_switch_en{&outputs.motor_ctrl_switch_en};

// Subsystems
DigitalOutput accumulator_en{&outputs.accumulator_en};
DigitalOutput front_controller_en{&outputs.front_controller_en};
DigitalOutput imu_gps_en{&outputs.imu_gps_en};
DigitalOutput shutdown_circuit_en{&outputs.shutdown_circuit_en};

// DCDC System  Measurement;
DigitalOutput dcdc_en{&outputs.dcdc_en};
DigitalOutput dcdc_sense_select{&outputs.dcdc_sense_select};
AnalogInput dcdc_sense{&inputs.dcdc_sense};

// HSD Sensing
AnalogInput hsd1_isense{&inputs.hsd1_isense};
DigitalOutput hsd1_isense_en{&outputs.hsd1_isense_en};
DigitalOutput hsd1_sel0{&outputs.hsd1_sel0};
DigitalOutput hsd1_sel1{&outputs.hsd1_sel1};
hsd::HSD4Channel hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};

AnalogInput hsd2_isense{&inputs.hsd2_isense};
DigitalOutput hsd2_isense_en{&outputs.hsd2_isense_en};
DigitalOutput hsd2_sel{&outputs.hsd2_sel};
hsd::HSD2Channel hsd2{hsd2_isense, hsd2_isense_en, hsd2_sel};

AnalogInput hsd3_isense{&inputs.hsd3_isense};
DigitalOutput hsd3_isense_en{&outputs.hsd3_isense_en};
DigitalOutput hsd3_sel{&outputs.hsd3_sel};
hsd::HSD2Channel hsd3{hsd3_isense, hsd3_isense_en, hsd3_sel};

AnalogInput hsd4_isense{&inputs.hsd4_isense};
DigitalOutput hsd4_isense_en{&outputs.hsd4_isense_en};
DigitalOutput hsd4_sel{&outputs.hsd4_sel};
hsd::HSD2Channel hsd4{hsd4_isense, hsd4_isense_en, hsd4_sel};

AnalogInput hsd5_isense{&inputs.hsd5_isense};
DigitalOutput hsd5_isense_en{&outputs.hsd5_isense_en};
DigitalOutput hsd5_sel{&outputs.hsd5_sel};
hsd::HSD2Channel hsd5{hsd5_isense, hsd5_isense_en, hsd5_sel};

AnalogInput hsd6_isense{&inputs.hsd6_isense};
DigitalOutput hsd6_isense_en{&outputs.hsd6_isense_en};
DigitalOutput hsd6_sel{&outputs.hsd6_sel};
hsd::HSD2Channel hsd6{hsd6_isense, hsd6_isense_en, hsd6_sel};

// Other IO
DigitalOutput brake_light_en{&outputs.brake_light_en};
AnalogInput suspension_travel3{&inputs.suspension_travel3};
AnalogInput suspension_travel4{&inputs.suspension_travel4};
CanBase veh_can_base{"vcan0"};

}  // namespace mcal
int port = 11001;

void writerThread(int fd) {
    while (true) {
        uint8_t cobsBuffer[2048] = {0};
        pb_ostream_t ostream =
            pb_ostream_from_buffer(cobsBuffer, sizeof(cobsBuffer));
        if (pb_encode(&ostream, &lvcontroller_Output_msg, &outputs) == 0) {
            std::cout << "Error with the outputs encoding: "
                      << PB_GET_ERROR(&ostream) << std::strerror(errno)
                      << std::endl;
        }
        uint8_t encodedBuffer[2048] = {0};
        int msg_size = macfe::cobs::Encode(cobsBuffer, ostream.bytes_written,
                                           encodedBuffer);
        send(fd, encodedBuffer, msg_size, 0);
    }
    close(fd);
}
void readerThread(int fd) {
    uint8_t cobsBuffer[2048] = {0};
    macfe::cobs::Decoder decoder(cobsBuffer);
    while (true) {
        uint8_t byte;
        ssize_t result = recv(fd, &byte, 1, 0);
        if (decoder.Decode(&byte, result)) {
            lvcontroller_Input inputs_temp;
            pb_istream_t istream =
                pb_istream_from_buffer(decoder.buffer, decoder.length);
            if (pb_decode(&istream, &lvcontroller_Input_msg, &inputs_temp)) {
                inputs = inputs_temp;
            } else {
                std::cerr << "Failed to decode inputs" << std::endl;
            }
            decoder = macfe::cobs::Decoder(cobsBuffer);
        }
    }
    close(fd);
}
void connectServer() {
    int sockfd = 0;
    bool has_connected = false;
    while (!has_connected) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr{};
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        servaddr.sin_port = htons(port);
        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) ==
            0) {
            has_connected = true;
        } else {
            std::cerr << "Error with the connection: " << std::strerror(errno)
                      << std::endl;
            close(sockfd);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
    wt = std::thread(writerThread, sockfd);
    rt = std::thread(readerThread, sockfd);
}
namespace bindings {
using namespace macfe::periph;

// Tractive System Status Indicator
DigitalOutput& tssi_en = mcal::tssi_en;
DigitalOutput& tssi_red_signal = mcal::tssi_red_signal;
DigitalOutput& tssi_green_signal = mcal::tssi_green_signal;
DigitalInput& imd_fault = mcal::imd_fault;
DigitalInput& bms_fault = mcal::bms_fault;

// Powertrain Cooling
DigitalOutput& powertrain_pump1_en = mcal::powertrain_pump1_en;
DigitalOutput& powertrain_pump2_en = mcal::powertrain_pump2_en;
DigitalOutput& powertrain_fan1_en = mcal::powertrain_fan1_en;
DigitalOutput& powertrain_fan2_en = mcal::powertrain_fan2_en;
PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

// Motor Controller (Inverters)
DigitalOutput& motor_controller_en = mcal::motor_controller_en;
DigitalOutput& motor_ctrl_precharge_en = mcal::motor_ctrl_precharge_en;
DigitalOutput& motor_ctrl_switch_en = mcal::motor_ctrl_switch_en;

// Subsystems
DigitalOutput& accumulator_en = mcal::accumulator_en;
DigitalOutput& front_controller_en = mcal::front_controller_en;
DigitalOutput& imu_gps_en = mcal::imu_gps_en;
DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

// DCDC System
DigitalOutput& dcdc_en = mcal::dcdc_en;
DigitalOutput& dcdc_sense_select = mcal::dcdc_sense_select;
AnalogInput& dcdc_sense = mcal::dcdc_sense;

// HSD Sensing
AnalogInput& hsd1_isense = mcal::hsd1_isense;
DigitalOutput& hsd1_isense_en = mcal::hsd1_isense_en;
DigitalOutput& hsd1_sel0 = mcal::hsd1_sel0;
DigitalOutput& hsd1_sel1 = mcal::hsd1_sel1;

AnalogInput& hsd2_isense = mcal::hsd2_isense;
DigitalOutput& hsd2_isense_en = mcal::hsd2_isense_en;
DigitalOutput& hsd2_sel = mcal::hsd2_sel;

AnalogInput& hsd3_isense = mcal::hsd3_isense;
DigitalOutput& hsd3_isense_en = mcal::hsd3_isense_en;
DigitalOutput& hsd3_sel = mcal::hsd3_sel;

AnalogInput& hsd4_isense = mcal::hsd4_isense;
DigitalOutput& hsd4_isense_en = mcal::hsd4_isense_en;
DigitalOutput& hsd4_sel = mcal::hsd4_sel;

AnalogInput& hsd5_isense = mcal::hsd5_isense;
DigitalOutput& hsd5_isense_en = mcal::hsd5_isense_en;
DigitalOutput& hsd5_sel = mcal::hsd5_sel;

AnalogInput& hsd6_isense = mcal::hsd6_isense;
DigitalOutput& hsd6_isense_en = mcal::hsd6_isense_en;
DigitalOutput& hsd6_sel = mcal::hsd6_sel;

hsd::HSD4Channel& hsd1 = mcal::hsd1;
hsd::HSD2Channel& hsd2 = mcal::hsd2;
hsd::HSD2Channel& hsd3 = mcal::hsd3;
hsd::HSD2Channel& hsd4 = mcal::hsd4;
hsd::HSD2Channel& hsd5 = mcal::hsd5;
hsd::HSD2Channel& hsd6 = mcal::hsd6;

// Other IO
DigitalOutput& brake_light_en = mcal::brake_light_en;
AnalogInput& suspension_travel3 = mcal::suspension_travel3;
AnalogInput& suspension_travel4 = mcal::suspension_travel4;
CanBase& veh_can_base = mcal::veh_can_base;  // clang-format on

void Initialize() {
    std::cout << "Initializing SIL\n";
    connectServer();
}

int GetTick() {
    using namespace std::chrono;
    static long start =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();  // only set on first call
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
               .count() -
           start;
}
// TODO Send PWM over threads, finish conversion
void DelayMS(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void SoftwareReset() {
    std::cout << "Performing software reset. Will not proceed." << std::endl;
    while (true) continue;
}

}  // namespace bindings
