
file(GLOB TOKENIZER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer/*.c")
file(GLOB PARSER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.c")
file(GLOB CODEGEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/codegen/*.c")

add_executable(
	asm
    main.cpp
	${TOKENIZER_SRC}
	${PARSER_SRC}
	${CODEGEN_SRC}
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