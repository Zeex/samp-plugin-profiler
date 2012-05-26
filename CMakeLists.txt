cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	"${CMAKE_SOURCE_DIR}/cmake/Modules")

include(AmxConfig)
include(GetGitRevisionDescription)

include_directories("src")
include_directories("src/amx")

add_library(profiler MODULE
	src/amx/amx.h
	src/amx/amxaux.c
	src/amx/amxaux.h
	src/amx/amxdbg.c
	src/amx/amxdbg.h
	src/amx/getch.h
	src/amx/osdefs.h
	src/amx/sclinux.h
	src/amx_name.cpp
	src/amx_name.h
	src/amx_profiler/c_interface.cpp
	src/amx_profiler/c_interface.h
	src/amx_profiler/call_graph.cpp
	src/amx_profiler/call_graph.h
	src/amx_profiler/call_graph_writer.h
	src/amx_profiler/call_graph_writer_gv.cpp
	src/amx_profiler/call_graph_writer_gv.h
	src/amx_profiler/call_stack.cpp
	src/amx_profiler/call_stack.h
	src/amx_profiler/chrono.h
	src/amx_profiler/debug_info.cpp
	src/amx_profiler/debug_info.h
	src/amx_profiler/function.cpp
	src/amx_profiler/function.h
	src/amx_profiler/function_call.cpp
	src/amx_profiler/function_call.h
	src/amx_profiler/function_info.cpp
	src/amx_profiler/function_info.h
	src/amx_profiler/profile_writer_html.cpp
	src/amx_profiler/profile_writer_html.h
	src/amx_profiler/native_function.cpp
	src/amx_profiler/native_function.h
	src/amx_profiler/normal_function.cpp
	src/amx_profiler/normal_function.h
	src/amx_profiler/profile_writer.h
	src/amx_profiler/profiler.cpp
	src/amx_profiler/profiler.h
	src/amx_profiler/public_function.cpp
	src/amx_profiler/public_function.h
	src/amx_profiler/profile_writer_text.cpp
	src/amx_profiler/profile_writer_text.h
	src/amx_profiler/performance_counter.cpp
	src/amx_profiler/performance_counter.h
	src/amx_profiler/time_interval.h
	src/amx_profiler/time_res.h
	src/amx_profiler/profile_writer_xml.cpp
	src/amx_profiler/profile_writer_xml.h
	src/amxplugin.cpp
	src/config_reader.cpp
	src/config_reader.h
	src/jump-x86.cpp
	src/jump-x86.h
	src/plugin.h
	src/plugincommon.h
	src/plugin.cpp
	src/plugin.rc
)

source_group(src REGULAR_EXPRESSION src/[^/\\]+\\..*)
source_group(src\\amx REGULAR_EXPRESSION src/amx/[^/\\]+\\..*)
source_group(src\\amx_profiler REGULAR_EXPRESSION src/amx_profiler/[^/\\]+\\..*)

find_package(Boost 1.47 COMPONENTS chrono date_time system)
if(Boost_FOUND)
	add_definitions(-DHAVE_BOOST=${Boost_VERSION})
	# Disable Boost auto-link feature 
	add_definitions(-DBOOST_ALL_NO_LIB)
	# Workaround swprintf() error related to lexical_cast<> on MinGW
	add_definitions(-DBOOST_LCAST_NO_WCHAR_T)
	if(Boost_CHRONO_FOUND)
		add_definitions(-DHAVE_BOOST_CHRONO)
	endif()
	if(Boost_DATE_TIME_FOUND)
		add_definitions(-DHAVE_BOOST_DATE_TIME)
	endif()
	include_directories(${Boost_INCLUDE_DIRS})
	if(CMAKE_COMPILER_IS_GNUCXX)
		# Chrono needs -lrt for clock_gettime()
		set(Boost_LIBRARIES ${Boost_LIBRARIES} -lrt)
	endif()
	target_link_libraries(profiler ${Boost_LIBRARIES})
endif()

set_property(TARGET profiler PROPERTY PREFIX "")

# Make sure we have proper C++11 support
if(CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if(GCC_VERSION VERSION_LESS 4.6)
		message(FATAL_ERROR "You need GCC 4.6 or later")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(MSVC)
	if (MSVC_VERSION LESS 1600) 
		message(FATAL_ERROR "You need MSVC 10.0 or later")
	endif()
endif()

if(UNIX OR MINGW)
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		COMPILE_FLAGS " -m32 -Wno-attributes")
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		LINK_FLAGS " -m32 -Wl,--no-undefined")
	if(WIN32)
		# Link with static runtime
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " -static-libgcc -static-libstdc++")
	endif()
endif()

if(UNIX AND NOT WIN32)
	add_definitions(-DLINUX)
endif()

if(WIN32)
	add_definitions(-DWIN32 -D_WIN32)
	set(def_file "${profiler_SOURCE_DIR}/src/plugin.def")
	if(MSVC)
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " /DEF:${def_file}")
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
	elseif(MINGW)
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " -Wl,--kill-at --def ${def_file}")
	endif()
endif()

find_package(Git)
if(GIT_FOUND)
	# Get latest version tag from Git history
	git_describe(description --match v[0-9]*.[0-9]**)
	string(REGEX REPLACE "\\-g[0-9a-f]+$" "" description ${description})
	string(REGEX REPLACE "^v(.*)" "\\1" version ${description})
endif()

if(version STREQUAL "")
	# Read version from VERSION.txt (updated on each release)
	message(STATUS "Failed to get version from Git, will read VERSION.txt")
	file(READ "${profiler_SOURCE_DIR}/VERSION.txt" version)
	string(STRIP ${version} version)
endif()

# Comma-separated version for RC
string(REGEX REPLACE "[.-]" "," version_rc ${version})
# Append one or two ",0" to the end to make windres happy
string(REGEX REPLACE "^([0-9]+,[0-9]+)$" "\\1,0,0" version_rc ${version_rc})
string(REGEX REPLACE "^([0-9]+,[0-9]+,[0-9]+)$" "\\1,0" version_rc ${version_rc})

# Generate version.h and plugin.rc
set(PLUGIN_NAME ${CMAKE_PROJECT_NAME})
set(PLUGIN_DESCRIPTION "Profiler plugin for SA-MP server")
set(PLUGIN_COPYRIGHT "Copyright (c) 2011-2012 Zeex")
set(PLUGIN_VERSION ${version})
set(PLUGIN_VERSION_RC ${version_rc})

configure_file(
	"${profiler_SOURCE_DIR}/src/plugin.rc.in"
	"${profiler_SOURCE_DIR}/src/plugin.rc"
	@ONLY)
configure_file(
	"${profiler_SOURCE_DIR}/src/version.h.in"
	"${profiler_SOURCE_DIR}/src/version.h"
	@ONLY)

message(STATUS "You're going to build Profiler v${version}")

install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES README.md DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

