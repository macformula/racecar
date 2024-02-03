CUBEMX_PATH =
IOC_FILE = 

CUBEMX_GEN_SCRIPT = cubemx_script.txt

GenerateCode: Makefile

Makefile: $(IOC_FILE)
	echo "config load \"$(IOC_FILE)\"" > $(CUBEMX_GEN_SCRIPT)
	echo "project generate ./" >> $(CUBEMX_GEN_SCRIPT)
	echo "exit" >> $(CUBEMX_GEN_SCRIPT)

	java -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"

	rm $(CUBEMX_GEN_SCRIPT)

	echo "" >> Makefile
	echo 'objects: $$(OBJECTS)' >> Makefile