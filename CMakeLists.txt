cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	"${CMAKE_SOURCE_DIR}/cmake/Modules")

include(GetGitRevisionDescription)

find_package(Git)
if(GIT_FOUND)
	# Get latest version tag from Git history
	git_describe(description --match v[0-9]*.[0-9]**)
	string(REGEX REPLACE "\\-g[0-9a-f]+$" "" description ${description})
	string(REGEX REPLACE "^v(.*)" "\\1" PROFILER_VERSION ${description})
endif()

if(PROFILER_VERSION STREQUAL "")
	# Read version from VERSION.txt (updated on each release)
	message(STATUS "Failed to get current version from Git, will read VERSION.txt")
	file(READ "${profiler_SOURCE_DIR}/VERSION.txt" PROFILER_VERSION)
	string(STRIP ${PROFILER_VERSION} PROFILER_VERSION)
endif()

add_subdirectory("src")

message(STATUS "You're going to build Profiler v${PROFILER_VERSION}")
