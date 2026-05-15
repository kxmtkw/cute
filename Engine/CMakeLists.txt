
file(GLOB CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/core/*.c")
file(GLOB IMAGE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/image/*.c")
file(GLOB ATOM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/atom/*.c")

add_library(
	CuteEngine
	STATIC

    ${CORE_SRC}
	${IMAGE_SRC}
	${ATOM_SRC}
)

target_compile_options(
    CuteEngine
    PRIVATE 
    -Wall -Wextra -g
)

target_include_directories(
    CuteEngine
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)


target_include_directories(
    CuteEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)