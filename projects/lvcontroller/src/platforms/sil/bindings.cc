/// @author Noah Jaye
/// @date 2025-10-31

#include "bindings.hpp"

#include <dirent.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "cobs.hpp"
#include "lvcontroller.pb.h"
#include "mcal/sil/analog_input.hpp"
#include "mcal/sil/analog_output.hpp"
#include "mcal/sil/can.hpp"
#include "mcal/sil/gpio.hpp"
#include "pb_decode.h"
#include "pb_encode.h"
#include "periph/gpio.hpp"
lvcontroller_Output outputs = lvcontroller_Output_init_zero;

lvcontroller_Input inputs = lvcontroller_Input_init_zero;
std::thread wt;
std::thread rt;
std::thread ct;

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

// Other IO
DigitalOutput brake_light_en{&outputs.brake_light_en};
AnalogInput suspension_travel3{&inputs.suspension_travel3};
AnalogInput suspension_travel4{&inputs.suspension_travel4};
CanBase veh_can_base{"vcan0"};

}  // namespace mcal
int port = 11001;
static bool write_callback(pb_ostream_t* stream, const uint8_t* buf,
                           size_t count) {
    int fd = (intptr_t)stream->state;
    bool result = send(fd, buf, count, 0) == count;
    return result;
}

static bool read_callback(pb_istream_t* stream, uint8_t* buf, size_t count) {
    int fd = (intptr_t)stream->state;
    int result;

    if (count == 0) return true;

    result = recv(fd, buf, count, 0);
    std::cout << "Buffer: " << buf << std::endl;
    std::cout << "Result: " << result << std::endl;
    if (result == 0) stream->bytes_left = 0; /* EOF */
    std::cout << "Bytes left: " << stream->bytes_left << std::endl;
    return result == count;
}

void writerThread(int sockfd) {
    while (true) {
        pb_ostream_t ostream = {
            .callback = &write_callback,
            .state = (void*)(intptr_t)sockfd,
            .max_size = SIZE_MAX,
            .bytes_written = 0,
        };
        // transmit that the message has started
        int msg_start[1] = {1};

        // TODO rm this
        static bool flipper = true;
        mcal::tssi_green_signal.Set(flipper);
        mcal::front_controller_en.Set(!flipper);
        mcal::imu_gps_en.Set(flipper);
        flipper = !flipper;
        // TODO until this

        send(sockfd, &msg_start, sizeof(msg_start), 0);
        if (pb_encode(&ostream, &lvcontroller_Output_msg, &outputs) == 0) {
            std::cout << "Error with the writer encoding: "
                      << PB_GET_ERROR(&ostream) << std::strerror(errno)
                      << std::endl;
        }
        // transmit that the message has ended
        int msg_end[1] = {2};
        send(sockfd, &msg_end, sizeof(msg_end), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
    close(sockfd);
}
void readerThread(int sockfd) {
    uint8_t cobsBuffer[2048] = {0};
    // Input_size is a useful thing to know about
    macfe::cobs::Decoder decoder(cobsBuffer);
    while (true) {
        std::cout << "Reading" << std::endl;
        /*pb_istream_t istream = {
            .callback = &read_callback,
            .state = (void*)(intptr_t)sockfd,
            .bytes_left = SIZE_MAX,
        };*/
        uint8_t byte;
        ssize_t result = recv(sockfd, &byte, 1, 0);
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
            std::cout << "imd_fault" << inputs.imd_fault << std::endl;
            std::cout << "bms_fault" << inputs.bms_fault << std::endl;
        }
    }
    std::cout << "This shouldn't print in the reader" << std::endl;
    close(sockfd);
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
            std::cerr << "Error to follow" << std::endl;
            std::cerr << "Error with the connection: " << std::strerror(errno)
                      << std::endl;
            close(sockfd);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
    // wt = std::thread(writerThread, sockfd);
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

// Other IO
DigitalOutput& brake_light_en = mcal::brake_light_en;
AnalogInput& suspension_travel3 = mcal::suspension_travel3;
AnalogInput& suspension_travel4 = mcal::suspension_travel4;
CanBase& veh_can_base = mcal::veh_can_base;  // clang-format on

void Initialize() {
    std::cout << "Initializing SIL\n";
    connectServer();
}

// copied luai
int GetTick() {
    using namespace std::chrono;
    long now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();
    static long start = now;  // only set on first call

    return now - start;
}

void DelayMS(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// copied luai
void SoftwareReset() {
    std::cout << "Performing software reset. Will not proceed." << std::endl;
    while (true) continue;
}

// ask for most recent struct from outer program
//  https://pubs.opengroup.org/onlinepubs/009695199/basedefs/sys/select.h.html

}  // namespace bindings
