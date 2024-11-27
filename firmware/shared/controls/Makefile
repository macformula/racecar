FIRMWARE_PATH = ../../

%: %.cc
	g++ $< -o $@ --std=c++23 -Wall -Werror -I $(FIRMWARE_PATH)
	./$@