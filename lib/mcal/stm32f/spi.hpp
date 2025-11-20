/// @author Priyal Taneja
/// @date 2025-10-19

#pragma once

#include <cstdint>

#include "periph/spi.hpp"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED

namespace mcal::stm32f {

class SpiMaster : public macfe::periph::SpiMaster {
private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;
    uint32_t timeout_ms_;
    bool initialized_;

    void ChipSelectLow() {
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
    }

    void ChipSelectHigh() {
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
    }

public:
    /// constructor
    /// @param hspi pointer to STM32 HAL SPI handle
    /// @param cs_port gpio port for chip select pin (e.g., GPIOA)
    /// @param cs_pin gpio pin number for chip select (e.g., GPIO_PIN_4)
    /// @param timeout_ms timeout for SPI operations in milliseconds (default:
    /// 100ms)
    SpiMaster(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin,
              uint32_t timeout_ms = 100)
        : hspi_(hspi),
          cs_port_(cs_port),
          cs_pin_(cs_pin),
          timeout_ms_(timeout_ms),
          initialized_(false) {}

    /// initialize the SPI peripheral
    /// must be called before using any transmission methods
    /// typically called from bindings::Initialize()
    void Init() {
        if (initialized_) {
            return;
        }

        // initialize chip select pin as output, set high (inactive)
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = cs_pin_;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(cs_port_, &GPIO_InitStruct);

        // set CS high (inactive) by default
        ChipSelectHigh();

        initialized_ = true;
    }

    void Transmit(uint8_t* tx_data, size_t length) override {
        if (!initialized_ || hspi_ == nullptr) {
            return;
        }

        ChipSelectLow();
        HAL_SPI_Transmit(hspi_, tx_data, length, timeout_ms_);
        ChipSelectHigh();
    }

    void Receive(uint8_t* rx_data, size_t length) override {
        if (!initialized_ || hspi_ == nullptr) {
            return;
        }

        ChipSelectLow();
        HAL_SPI_Receive(hspi_, rx_data, length, timeout_ms_);
        ChipSelectHigh();
    }

    void TransmitReceive(uint8_t* tx_data, uint8_t* rx_data,
                         size_t length) override {
        if (!initialized_ || hspi_ == nullptr) {
            return;
        }

        ChipSelectLow();
        HAL_SPI_TransmitReceive(hspi_, tx_data, rx_data, length, timeout_ms_);
        ChipSelectHigh();
    }

    /// check if the spi peripheral has been initialized
    bool IsInitialized() const {
        return initialized_;
    }

    /// get the underlying hal spi handle
    SPI_HandleTypeDef* GetHandle() const {
        return hspi_;
    }
};

}  // namespace mcal::stm32f

#endif  // HAL_SPI_MODULE_ENABLED