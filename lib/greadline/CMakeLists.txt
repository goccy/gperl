cmake_minimum_required(VERSION 2.6)

project(greadline)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(greadline
	src/greadline.c
	src/keyword.c
	src/term.c
	src/complete.c
	src/history.c
	src/mem.c
)
