This bootloader will follow a application-resident CAN bootloader.

The reason for the name 'resident' is because the bootloader will live in the application binary itself.


Given requirements, the bootloader will perform as such:

bootloader::Run() will be called at the start of the bindings::Initialized(). On 3s of CAN silence, it will tear everything down and return back to normal. When a host commands it to stay, it will receive a new image over CAN, CRC-checks it, and reprograms flash from a RAM-resident flash driver, then resets. 

The risk we run with running an application resident bootloader is that every SUCCESSFUL flash is a fresh bootloader as well. 

The architecture:

Reset -> Boot 0 low -> boots into flash at 0x08000000



NEED TO LOOK AT what config HSE needs to be in order to work with our program

Yes, you can absolutely make these changes later.In fact, modifying stm32f7xx_hal_conf.h after your project is up and running is standard practice. The file is just a header containing preprocessor #define directives, meaning the compiler reads it fresh every single time you hit "Build."  However, keep these two sequential dependency rules in mind so you don't accidentally waste time debugging later:1. The Clock Dependency (Do this before configuring CAN)You can leave HSE_VALUE at 25 MHz while you write your basic state machine code. However, you must correct it to 8 MHz before you calculate or write your CAN bit-timing initialization parameters.  Why: The HAL library uses HSE_VALUE to calculate the timing blocks for the peripheral buses. If the file claims the clock is 25 MHz but the physical board runs on an 8 MHz crystal, your actual CAN baud rate will be completely wrong, and the Raspberry Pi won't be able to establish a hardware handshake.  2. The Driver Check (Do this before calling Flash/CRC functions)You can write code that calls HAL_FLASH_Unlock() or hardware CRC functions whenever you like. But if you try to compile that code before uncommenting #define HAL_FLASH_MODULE_ENABLED and #define HAL_CRC_MODULE_ENABLED in your configuration file, the compiler will throw an immediate error stating that those functions are undefined.Summary StrategyGo ahead and focus on structuring your SRAM data buffer management and frame indexing first. Once you are ready to transition from data parsing to physical peripheral execution, open up your stm32f7xx_hal_conf.h, apply the edits, and re-compile

