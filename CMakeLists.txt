cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	"${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckHeader)
include(GetGitRevisionDescription)

# Configure Boost
find_package(Boost 1.47 REQUIRED COMPONENTS chrono filesystem system)

# Disable the auto-link feature 
add_definitions(-DBOOST_ALL_NO_LIB)

check_header(alloca.h)
check_header(inttypes.h)
check_header(malloc.h)
check_header(stdint.h)

include_directories("lib") 
include_directories("lib/amx")
include_directories(${Boost_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})

add_library(profiler MODULE
	"lib/amx/amx.h"
	"lib/amx/amxaux.c"
	"lib/amx/amxaux.h"
	"lib/amx/amxdbg.c"
	"lib/amx/amxdbg.h"
	"lib/amx/getch.h"
	"lib/amx/osdefs.h"
	"lib/amx/sclinux.h"
	"lib/amxplugin.cpp"
	"lib/plugin.h"
	"lib/plugincommon.h"
	"src/abstract_printer.h"
	"src/amx_name.cpp"
	"src/amx_name.h"
	"src/call_stack.cpp"
	"src/call_stack.h"
	"src/config_reader.cpp"
	"src/config_reader.h"
	"src/debug_info.cpp"
	"src/debug_info.h"
	"src/function.cpp"
	"src/function.h"
	"src/html_printer.cpp"
	"src/html_printer.h"
	"src/jump_x86.cpp"
	"src/jump_x86.h"
	"src/plugin.cpp"
	"src/plugin.rc"
	"src/profile.h"
	"src/profiler.cpp"
	"src/profiler.h"
	"src/text_printer.cpp"
	"src/text_printer.h"
	"src/timer.cpp"
	"src/timer.h"
	"src/xml_printer.cpp"
	"src/xml_printer.h"
)

set_property(TARGET profiler PROPERTY PREFIX "")

target_link_Libraries(profiler ${Boost_LIBRARIES})

if(UNIX OR MINGW)
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		COMPILE_FLAGS "-m32 -Wno-attributes -include stddef.h")
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		LINK_FLAGS "-m32 -Wl,--no-undefined -static-libstdc++ -lrt")
endif()

if(UNIX AND NOT WIN32)
	add_definitions(-DLINUX)
endif()

if(WIN32)
	add_definitions(-DWIN32 -D_WIN32)
	set(def_file "${profiler_SOURCE_DIR}/src/plugin.def")
	if(MSVC)
		set_property(TARGET profiler APPEND_STRING PROPERTY LINK_FLAGS "/DEF:${def_file}")
	elseif(MINGW)
		set_property(TARGET profiler APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--kill-at --def ${def_file}")
	endif()
endif()

find_package(Git)
if(GIT_FOUND)
	# Get latest version tag from Git history
	git_describe(description --match v[0-9]*.[0-9]*.[0-9]*)
	string(REPLACE "-" "." description ${description})
	string(REGEX REPLACE "\\.g[0-9a-f]+$" "" description ${description})
	string(REGEX REPLACE "^v(.*)" "\\1" version ${description})
	if(NOT version MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+\\.?[0-9]*$")
		set(version "")
	endif()
endif()

if(version STREQUAL "")
	# Read version from VERSION.txt (updated on each release)
	message(STATUS "Failed to get version from Git, will read VERSION.txt")
	file(READ "${crashdetect_SOURCE_DIR}/VERSION.txt" version)
	string(STRIP ${version} version)
	if(NOT version MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
		message(FATAL_ERROR "Invalid version")
	endif()
endif()

# Comma-separated version for RC
string(REPLACE "." "," version_rc ${version})

# Generate version.h to be included from source files
file(WRITE "src/version.h" 
"/* This file is auto-generated, do not edit */
#define PROFILER_VERSION \"${version}\"
#define PROFILER_VERSION_RC ${version_rc}
")

message(STATUS "You're going to build Profiler v${version}")

install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES "README.txt" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

