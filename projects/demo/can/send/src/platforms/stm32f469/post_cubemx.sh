# In-Place sort the IOC file. This makes `git diff` clearer.
IOC=$(find *.ioc) && sort $IOC --version-sort --output=$IOC 

# Remove linker script for RAM debugging. We run the program from FLASH.
rm -f *_RAM.ld

