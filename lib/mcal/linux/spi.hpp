/// @author Priyal Taneja
/// @date 2025-11-06

#pragma once

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <string>

#include "periph/spi.hpp"

namespace mcal::lnx {

class SpiMaster : public macfe::periph::SpiMaster {
private:
    int fd_;
    std::string device_path_;
    uint32_t speed_hz_;
    uint8_t mode_;
    uint8_t bits_per_word_;
    bool initialized_;

public:
    /// constructor
    /// @param device_path path to spi device (e.g., "/dev/spidev0.0")
    /// @param speed_hz spi clock speed in Hz (default: 1 MHz)
    /// @param mode spi mode 0-3 (default: SPI_MODE_0)
    /// @param bits_per_word bits per word (default: 8)
    SpiMaster(std::string device_path, uint32_t speed_hz = 1000000,
              uint8_t mode = SPI_MODE_0, uint8_t bits_per_word = 8)
        : fd_(-1),
          device_path_(device_path),
          speed_hz_(speed_hz),
          mode_(mode),
          bits_per_word_(bits_per_word),
          initialized_(false) {}

    ~SpiMaster() {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    /// initialize the spi device
    /// opens the device file and configures spi parameters
    void Setup() {
        if (initialized_) {
            return;
        }

        // open spi device
        fd_ = open(device_path_.c_str(), O_RDWR);
        if (fd_ < 0) {
            return;
        }

        // set spi mode
        if (ioctl(fd_, SPI_IOC_WR_MODE, &mode_) < 0) {
            close(fd_);
            fd_ = -1;
            return;
        }

        // set bits per word
        if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word_) < 0) {
            close(fd_);
            fd_ = -1;
            return;
        }

        // set max speed
        if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz_) < 0) {
            close(fd_);
            fd_ = -1;
            return;
        }

        initialized_ = true;
    }

    void Transmit(uint8_t* tx_data, size_t length) override {
        if (!initialized_ || fd_ < 0) {
            return;
        }

        struct spi_ioc_transfer transfer = {0};
        transfer.tx_buf = reinterpret_cast<__u64>(tx_data);
        transfer.rx_buf = 0;
        transfer.len = length;
        transfer.speed_hz = speed_hz_;
        transfer.bits_per_word = bits_per_word_;

        ioctl(fd_, SPI_IOC_MESSAGE(1), &transfer);
    }

    void Receive(uint8_t* rx_data, size_t length) override {
        if (!initialized_ || fd_ < 0) {
            return;
        }

        struct spi_ioc_transfer transfer = {0};
        transfer.tx_buf = 0;
        transfer.rx_buf = reinterpret_cast<__u64>(rx_data);
        transfer.len = length;
        transfer.speed_hz = speed_hz_;
        transfer.bits_per_word = bits_per_word_;

        ioctl(fd_, SPI_IOC_MESSAGE(1), &transfer);
    }

    void TransmitReceive(uint8_t* tx_data, uint8_t* rx_data,
                         size_t length) override {
        if (!initialized_ || fd_ < 0) {
            return;
        }

        struct spi_ioc_transfer transfer = {0};
        transfer.tx_buf = reinterpret_cast<__u64>(tx_data);
        transfer.rx_buf = reinterpret_cast<__u64>(rx_data);
        transfer.len = length;
        transfer.speed_hz = speed_hz_;
        transfer.bits_per_word = bits_per_word_;

        ioctl(fd_, SPI_IOC_MESSAGE(1), &transfer);
    }
};

}  // namespace mcal::lnx