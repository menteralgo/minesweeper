cmake_minimum_required(VERSION 3.1)

project(Minesweeper)
add_library(Core minesweeper.c surface.c game.c image.c)

configure_file(../media/Atlas.ppm ../build/Atlas.ppm COPYONLY)

if (UNIX)
	add_executable(Minesweeper platform/xmain.c platform/xview.c)
	target_link_libraries(Minesweeper PRIVATE Core xcb xcb-image)
endif (UNIX)