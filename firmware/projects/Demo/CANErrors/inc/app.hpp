/// @author Luai Bashar
/// @date 2024-11-10
/// @brief Functions and types that will be used in CANErrors main

#pragma once

#include "../generated/can/error_can_messages.hpp"
#include "shared/comms/can/can_bus.hpp"

/***************************************************************
    App-level objects
***************************************************************/

// Defines all possible errors to set
enum class Error {
    kError0 = 0,
    kError1 = 1,
    kError2 = 2,
    kError3 = 3,
    kError4 = 4,
    kError5 = 5,
    kError6 = 6,
    kError7 = 7,
    kError8 = 8,
    kError9 = 9,
    kError10 = 10,
    kError11 = 11,
    kError12 = 12,
    kError13 = 13,
    kError14 = 14,
    kError15 = 15,
    kError16 = 16,
    kError17 = 17,
    kError18 = 18,
    kError19 = 19,
    kError20 = 20,
    kError21 = 21,
    kError22 = 22,
    kError23 = 23,
    kError24 = 24,
    kError25 = 25,
    kError26 = 26,
    kError27 = 27,
    kError28 = 28,
    kError29 = 29,
    kError30 = 30,
    kError31 = 31,
    kError32 = 32,
    kError33 = 33,
    kError34 = 34,
    kError35 = 35,
    kError36 = 36,
    kError37 = 37,
    kError38 = 38,
    kError39 = 39,
    kError40 = 40,
    kError41 = 41,
    kError42 = 42,
    kError43 = 43,
    kError44 = 44,
    kError45 = 45,
    kError46 = 46,
    kError47 = 47,
    kError48 = 48,
    kError49 = 49,
    kError50 = 50,
    kError51 = 51,
    kError52 = 52,
    kError53 = 53,
    kError54 = 54,
    kError55 = 55,
    kError56 = 56,
    kError57 = 57,
    kError58 = 58,
    kError59 = 59,
    kError60 = 60,
    kError61 = 61,
    kError62 = 62,
    kError63 = 63
};

// Interface to set/send all possible errors for the system
class ErrorHandler {
public:
    // Sets the error based on the error index given
    void SetError(Error error) {
        uint64_t error_index = static_cast<uint64_t>(error);
        error_message_.errors |= (1ULL << error_index);
    }

    // Sends the error message through the provided CAN bus
    // TODO: Error message is only for a specific bus, change this when autogen
    // code is created
    void SendError(shared::can::CanBus error_can_bus) {
        error_can_bus.Send(error_message_);

        // Reset after a send to not send duplicate errors
        ResetError();
    }

private:
    // Object that holds a 64 bit int, each bit representing an error
    generated::can::TMS_ERROR error_message_{};

    // Resets all errors back to untriggered
    void ResetError() {
        error_message_.errors = 0;
    }
};