# Blake Freer
# December 25, 2023 (Merry Christmas!)

add_custom_command(
	TARGET main
	POST_BUILD
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:main>
	COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:main> main.hex
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:main> main.bin
	COMMENT "Placed binaries in $<TARGET_FILE_DIR:main>"
)