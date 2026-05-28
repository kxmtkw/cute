
add_executable(
	asm
    main.cpp
)

target_link_libraries(asm PRIVATE CuteEngine)
target_compile_options(
    asm
    PRIVATE 
    -Wall -Wextra -g
)

set_target_properties(
    asm
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)