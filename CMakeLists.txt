cmake_minimum_required(VERSION 3.16)

project(tetris-csfml)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-microsoft-anon-tag")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fms-extensions")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
endif ()

find_package(SFML 2 COMPONENTS graphics window system REQUIRED)
find_package(OpenGL)

link_directories("${CMAKE_PREFIX_PATH}/lib")

include_directories(
	.
	deps/cJSON
	deps/libf8
	deps/prefix/include
	include
	)

set(tetris-src
	deps/libf8/f8.c
	deps/cJSON/cJSON.c
	src/media/csfml.c
	src/config.c
	src/input.c
	src/target/tetris.c
	src/painter.c
	src/controller.c
	src/game.c
	src/media.c
	src/field.c
	src/slist.c
	src/idlist.c
	src/hs_table.c
	src/shape_maps.c
	src/text.c
	)

add_executable(tetris ${tetris-src})

set_target_properties(tetris PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(tetris
	csfml-graphics
	csfml-window
	csfml-system
	sfml-graphics
	sfml-window
	sfml-system
	)

if (SFML_STATIC_LIBRARIES)
	target_link_libraries(tetris ${OPENGL_LIBRARIES})
endif ()
