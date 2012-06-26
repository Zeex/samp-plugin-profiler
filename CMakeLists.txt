cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	"${CMAKE_SOURCE_DIR}/cmake/Modules")

include(AmxConfig)
include(GetGitRevisionDescription)

include_directories(.)
include_directories(amx)

add_library(profiler MODULE
	amx/amx.h
	amx/amxaux.c
	amx/amxaux.h
	amx/amxdbg.c
	amx/amxdbg.h
	amx/getch.h
	amx/osdefs.h
	amx/sclinux.h
	amx_profiler/c_interface.cpp
	amx_profiler/c_interface.h
	amx_profiler/call_graph.cpp
	amx_profiler/call_graph.h
	amx_profiler/call_graph_writer.h
	amx_profiler/call_graph_writer_gv.cpp
	amx_profiler/call_graph_writer_gv.h
	amx_profiler/call_stack.cpp
	amx_profiler/call_stack.h
	amx_profiler/chrono.h
	amx_profiler/debug_info.cpp
	amx_profiler/debug_info.h
	amx_profiler/function.cpp
	amx_profiler/function.h
	amx_profiler/function_call.cpp
	amx_profiler/function_call.h
	amx_profiler/function_statistics.cpp
	amx_profiler/function_statistics.h
	amx_profiler/profile_writer_html.cpp
	amx_profiler/profile_writer_html.h
	amx_profiler/native_function.cpp
	amx_profiler/native_function.h
	amx_profiler/normal_function.cpp
	amx_profiler/normal_function.h
	amx_profiler/profile_writer.h
	amx_profiler/profiler.cpp
	amx_profiler/profiler.h
	amx_profiler/public_function.cpp
	amx_profiler/public_function.h
	amx_profiler/profile_writer_text.cpp
	amx_profiler/profile_writer_text.h
	amx_profiler/performance_counter.cpp
	amx_profiler/performance_counter.h
	amx_profiler/time_interval.h
	amx_profiler/time_res.h
	amx_profiler/profile_writer_xml.cpp
	amx_profiler/profile_writer_xml.h
	plugin/amx_path.cpp
	plugin/amx_path.h
	plugin/amxplugin.cpp
	plugin/config_reader.cpp
	plugin/config_reader.h
	plugin/jump-x86.cpp
	plugin/jump-x86.h
	plugin/plugin.h
	plugin/plugincommon.h
	plugin/plugin.cpp
	plugin/plugin.rc
)

source_group(amx REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/amx/[^/\\]+\\..*)
source_group(amx_profiler REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/amx_profiler/[^/\\]+\\..*)
source_group(plugin REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/plugin/[^/\\]+\\..*)

find_package(Boost 1.47 COMPONENTS chrono system)
if(Boost_FOUND)
	add_definitions(-DHAVE_BOOST=${Boost_VERSION})
	# Disable Boost auto-link feature 
	add_definitions(-DBOOST_ALL_NO_LIB)
	# Workaround swprintf() error related to lexical_cast<> on MinGW
	add_definitions(-DBOOST_LCAST_NO_WCHAR_T)
	if(Boost_CHRONO_FOUND)
		add_definitions(-DHAVE_BOOST_CHRONO)
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
	set(def_file "${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin.def")
	if(MSVC)
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " /DEF:${def_file}")
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
	elseif(MINGW)
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " -Wl,--kill-at --def ${def_file}")
	endif()
endif()

git_describe(description --match v[0-9]*.[0-9]**)
if(description)
	string(REGEX REPLACE "\\-g[0-9a-f]+$" "" description ${description})
	string(REGEX REPLACE "^v(.*)" "\\1" version ${description})
else()
	message(STATUS "Failed to get version from Git, will read VERSION.txt")
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.txt" version)
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
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin.rc.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin.rc"
	@ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/version.h"
	@ONLY)

message(STATUS "You're going to build Profiler v${version}")

install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES "README.md" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

