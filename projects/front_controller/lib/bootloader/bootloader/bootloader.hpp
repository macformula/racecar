#include ""

namespace bootloader {

int Run(void);
void WriteFirmwaretoFlash() __attribute__((section(".RamFunc")), noinline);

}  // namespace bootloader