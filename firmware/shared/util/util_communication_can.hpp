/// @author Clara Wong
/// @date 2023-10-23
///@brief CAN class based on the STM HAL CAN implementation

#ifndef CAN_COMMUNICATION_H
#define CAN_COMMUNICATION_H

#include "..\racecar\drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal_can.h"
#include "..\racecar\drivers\STM32F7xx_HAL_Driver\Inc\stm32f7xx_hal.h"
#include "communication.hpp"
#include <iostream>

namespace util
{
    class CANCommunication : public CommunicationBase
    {
    public:
        CANCommunication(CAN_HandleTypeDef *hcan);
        ~CANCommunication() override = default;

        // send a CAN message
        bool Send(const int8_t byte_to_send) noexcept override;

        // receive a CAN message
        bool Recv(uint8_t &byte_to_recv) override;

        void Select() override;
        void Deselect() override;

    private:
        // pointer to HAL CAN handler
        CAN_HandleTypeDef *hcan;
    }
}
#endif