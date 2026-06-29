
file(GLOB CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")
file(GLOB TOKENIZER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer/*.cpp")
file(GLOB PARSER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp")
file(GLOB RESOLVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/resolver/*.cpp")
file(GLOB CODEGEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/codegen/*.cpp")
file(GLOB UTILS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp")

add_executable(
	cute-asm
    main.cpp
	${CORE_SRC}
	${TOKENIZER_SRC}
	${PARSER_SRC}
	${RESOLVER_SRC}
	${CODEGEN_SRC}
	${UTILS_SRC}
)

target_link_libraries(cute-asm PRIVATE CuteEngine)
target_compile_options(
    cute-asm
    PRIVATE 
    -Wall -Wextra -g
)

target_include_directories(
    cute-asm
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(
    cute-asm
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)