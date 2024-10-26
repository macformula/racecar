openocd -f interface/stlink.cfg -f target/stm32f7x.cfg & arm-none-eabi-gdb -ex "target remote localhost:3333" -ex "load" /Users/cameron.beneteau/code/MacFormulaElectric/repos/racecar/firmware/build/Demo/Blink/stm32f767/main.elf
# arm-none-eabi-gdb /Users/cameron.beneteau/code/MacFormulaElectric/repos/racecar/firmware/build/Demo/Blink/stm32f767/main.elf
