
file(GLOB TOKENIZER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer/*.cpp")

add_executable(
	asme
    main.cpp
	${TOKENIZER_SRC}
)

target_link_libraries(asme PRIVATE CuteEngine)
target_compile_options(
    asme
    PRIVATE 
    -Wall -Wextra -g
)

set_target_properties(
    asme
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)