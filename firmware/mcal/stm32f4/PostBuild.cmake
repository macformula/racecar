# Blake Freer
# December 25, 2023 (Merry Christmas!)

add_custom_command(
	TARGET main
	POST_BUILD
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:main>
	COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:main> ${CMAKE_PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:main> ${CMAKE_PROJECT_NAME}.bin
	# COMMAND ${CMAKE_OBJDUMP} -D -C $<TARGET_FILE:main> > main.s
)