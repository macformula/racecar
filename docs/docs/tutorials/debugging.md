<!-- Title with debugging icon -->
# Debugging

Terminal 1:
```bash
openocd -f interface/stlink.cfg -f target/stm32f7x.cfg
```

Terminal 2:
```bash
# arm-none-eabi-gdb your_program.elf
arm-none-eabi-gdb /Users/cameron.beneteau/code/MacFormulaElectric/repos/racecar/firmware/build/Demo/Blink/stm32f767/main.elf
target remote localhost:3333
```
