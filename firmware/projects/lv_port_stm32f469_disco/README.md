This is a rough solution. The CMake is a mess so you have to do some manual work.

1. Open `platforms/stm32/cubemx/board_config.ioc` in STM32CubeMX
2. Click generate code.
3. Replace contents of the generated linker script with the contents of `lv_port_stm32f469_disco/STM32F469NIHX_FLASH.ld`
4. Build the project

    ```
    make PROJECT=lv_port_stm32f469_disco PLATFORM=stm32
    ```

5. Flash the binary to the board with STM32CubeProgrammer.
