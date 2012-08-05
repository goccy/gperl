include(CheckTypeSize)
cmake_minimum_required(VERSION 2.6)

project(gperl)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/lib/greadline/include)
include_directories(${INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

SET(CMAKE_C_FLAGS         "-Wall")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -g")
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")
SET(CMAKE_CXX_FLAGS         "-Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")

set(SOURCE
	src/main.cpp
	src/gperl.cpp
	src/token.cpp
	src/parse.cpp
	src/ast.cpp
	src/graph.cpp
	src/compiler.cpp
	src/memory.cpp
	src/vm.cpp
	src/gen_vm.cpp
	src/string.cpp
	src/array.cpp
	src/undef.cpp
	src/jit.cpp
)

check_type_size("void *" SIZEOF_VOIDP)
check_type_size(long     SIZEOF_LONG)
check_type_size(int      SIZEOF_INT)
check_type_size(float    SIZEOF_FLOAT)
check_type_size(double   SIZEOF_DOUBLE)

FIND_FILE(HAS_DOTGIT .git PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
if("${HAS_DOTGIT}" MATCHES ".git")
	find_package(Git)
	if(GIT_EXECUTABLE)
		execute_process(
				COMMAND ${GIT_EXECUTABLE} rev-parse -q --short=8 HEAD
			OUTPUT_VARIABLE GIT_SHA1
				OUTPUT_STRIP_TRAILING_WHITESPACE
				)
	set(GPERL_REVISION "0x${GIT_SHA1}")
	endif(GIT_EXECUTABLE)
endif()

add_definitions(-DDSTATIC_TYPING_MODE)
add_definitions(-DHAVE_CONFIG_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
		${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(lib/greadline)
add_executable(gperl ${SOURCE})
target_link_libraries(gperl greadline)

install(TARGETS gperl RUNTIME DESTINATION bin)

MESSAGE(STATUS)
MESSAGE(STATUS "-----------------------------------------------------------------" )
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${uppercase_CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}}")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE(STATUS "-----------------------------------------------------------------" )
MESSAGE(STATUS)
