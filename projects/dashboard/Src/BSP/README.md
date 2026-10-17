Select files from the ST Board Support Packages.

We copied the files (instead of adding as submodules) since the BSP file structures are poorly designed and hard to compile.

The latest versions (v2.X.X) of each submodule are not compatible with each other.

At time of writing, we are using

- 32f469idiscovery-bsp v2.2.0

    <https://github.com/STMicroelectronics/32f469idiscovery-bsp>

- otm8009a v1.0.7
    
    <https://github.com/STMicroelectronics/stm32-otm8009a>

- nt35510 v1.0.3

    <https://github.com/STMicroelectronics/stm32-nt35510>

Both the otm and nt packages expect an external `_IO_DELAY(uint32_t)` method. This is defined in `32f469idiscovery-bsp/stm32469i_discovery.c`, but that file also expects us to use the I2C drivers, which we do not. We added the `io_delay.cc` to bypass this.
