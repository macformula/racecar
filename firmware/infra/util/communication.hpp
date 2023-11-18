/// @author Blake Freer, Clara Wong
/// @date 2023-10-23

#ifndef UTIL_COMMUNICATION_HPP
#define UTIL_COMMUNICATION_HPP

#include "noncopyable.hpp"
#include "..\drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal_can.h"
#include "..\drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace util {

    template<typename CANMessage>

/// @brief Base class for all communication protocols (SPI, CAN, ...)
    class CANCommunication : private util::noncopyable {
        public:
            CANCommunication(CAN_HandleTypeDef* hcan);
            ~CANCommunication() override = default;

            void Init(CAN_HandleTypeDef *hcan);

            //read message from CAN RX queue
            void Recv(uint32_t CAN_RX_FIFO);

            void Transmit(CANMessage message, uint8_t size);
        
        protected:
            CANCommunication() = default;

    };

} 

#endif