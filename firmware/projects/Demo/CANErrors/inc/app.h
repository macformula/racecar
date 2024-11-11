/// @author Luai Bashar
/// @date 2024-11-10
/// @brief Functions and types that will be used in CANErrors main

#pragma once

#include "../generated/can/error_can_messages.h"
#include "shared/comms/can/can_bus.h"

/***************************************************************
    App-level objects
***************************************************************/

// Defines all possible errors to set
enum Error {
    Error0 = 0,
    Error1 = 1,
    Error2 = 2,
    Error3 = 3,
    Error4 = 4,
    Error5 = 5,
    Error6 = 6,
    Error7 = 7,
    Error8 = 8,
    Error9 = 9,
    Error10 = 10,
    Error11 = 11,
    Error12 = 12,
    Error13 = 13,
    Error14 = 14,
    Error15 = 15,
    Error16 = 16,
    Error17 = 17,
    Error18 = 18,
    Error19 = 19,
    Error20 = 20,
    Error21 = 21,
    Error22 = 22,
    Error23 = 23,
    Error24 = 24,
    Error25 = 25,
    Error26 = 26,
    Error27 = 27,
    Error28 = 28,
    Error29 = 29,
    Error30 = 30,
    Error31 = 31,
    Error32 = 32,
    Error33 = 33,
    Error34 = 34,
    Error35 = 35,
    Error36 = 36,
    Error37 = 37,
    Error38 = 38,
    Error39 = 39,
    Error40 = 40,
    Error41 = 41,
    Error42 = 42,
    Error43 = 43,
    Error44 = 44,
    Error45 = 45,
    Error46 = 46,
    Error47 = 47,
    Error48 = 48,
    Error49 = 49,
    Error50 = 50,
    Error51 = 51,
    Error52 = 52,
    Error53 = 53,
    Error54 = 54,
    Error55 = 55,
    Error56 = 56,
    Error57 = 57,
    Error58 = 58,
    Error59 = 59,
    Error60 = 60,
    Error61 = 61,
    Error62 = 62,
    Error63 = 63
};

// Interface to set/send all possible errors for the system
class ErrorHandler {
    private:
        // Object that holds a 64 bit int, each bit representing an error
        generated::can::TMS_ERROR error_message{};

        // Resets all errors back to untriggered
        void resetError() {
            error_message.errors = 0;
        }
    public:
        // Sets the error based on the error index given
        void setError(Error error) {
            if (error < 0 || error > 63) {
                std::cerr << "Error index must be between 0 and 63!" << std::endl;
                return;
            }

            error_message.errors |= (1ULL << error);
        }

        // Sends the error message through the provided CAN bus
        void sendError(shared::can::CanBus error_can_bus) {
            error_can_bus.Send(error_message);

            // Reset after a send to not send duplicate errors
            resetError();
        }
};