
file(GLOB TOKENIZER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer/*.cpp")
file(GLOB PARSER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp")
file(GLOB CODEGEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/codegen/*.cpp")

add_executable(
	asme
    main.cpp
	${TOKENIZER_SRC}
	${PARSER_SRC}
	${CODEGEN_SRC}
)

target_link_libraries(asme PRIVATE CuteEngine)
target_compile_options(
    asme
    PRIVATE 
    -Wall -Wextra -g
)

target_include_directories(
    asme
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(
    asme
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)