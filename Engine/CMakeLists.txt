
file(GLOB ENGINE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.c")
file(GLOB IMAGE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/image/*.c")

add_library(
	CuteEngine
	STATIC

    ${ENGINE_SRC}
	${IMAGE_SRC}
)

target_compile_options(
    CuteEngine
    PRIVATE 
    -Wall -Wextra -g
)

target_include_directories(
    CuteEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
    CuteEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)