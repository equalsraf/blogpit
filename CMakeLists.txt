cmake_minimum_required(VERSION 2.6)
project(blogpit)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -W -Wall -fPIC -fprofile-arcs -ftest-coverage")

#
# Dependencies
#
find_package(PythonLibs)
find_package(PythonInterp)
# FIXME: remove hardcoded paths
# find_library( LIBGIT2 NAMES git2 PATHS "${CMAKE_BINARY_DIR}/../../libgit2/build" )
# FIXME: Remove the following line
# include_directories(${CMAKE_SOURCE_DIR}/../libgit2/include)
# include_directories(${CMAKE_SOURCE_DIR}/src)

if ( ${LIBGIT2} STREQUAL "LIBGIT2-NOTFOUND" )
	message(FATAL_ERROR "Unable to find libgit2, are you sure it is installed?")
endif ( ${LIBGIT2} STREQUAL "LIBGIT2-NOTFOUND" )


#
# Doxygen
# - add a "doc" target to build the documentation
#
include(FindDoxygen)
if ( ${DOXYGEN_FOUND} STREQUAL "YES" )
	add_custom_target(doc)
	add_custom_command(TARGET doc PRE_BUILD
				COMMAND ${DOXYGEN_EXECUTABLE}
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
endif ( ${DOXYGEN_FOUND} STREQUAL "YES" )


#
# source and test folders
#
ENABLE_TESTING()
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(python)


