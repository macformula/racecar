This bootloader will follow a application-resident CAN bootloader.

The reason for the name 'resident' is because the bootloader will live in the application binary itself.


Given requirements, the bootloader will perform as such:

bootloader::Run() will be called at the start of the bindings::Initialized(). On 3s of CAN silence, it will tear everything down and return back to normal. When a host commands it to stay, it will receive a new image over CAN, CRC-checks it, and reprograms flash from a RAM-resident flash driver, then resets. 

The risk we run with running an application resident bootloader is that every SUCCESSFUL flash is a fresh bootloader as well. 

The architecture:

Reset -> Boot 0 low -> boots into flash at 0x08000000