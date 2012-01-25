cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	"${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckHeader)
include(GetGitRevisionDescription)

check_header(alloca.h)
check_header(inttypes.h)
check_header(malloc.h)
check_header(stdint.h)

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
	src/amx_profiler/call_stack.cpp
	src/amx_profiler/call_stack.h
	src/amx_profiler/debug_info.cpp
	src/amx_profiler/debug_info.h
	src/amx_profiler/function.cpp
	src/amx_profiler/function.h
	src/amx_profiler/function_call.cpp
	src/amx_profiler/function_call.h
	src/amx_profiler/function_info.cpp
	src/amx_profiler/function_info.h
	src/amx_profiler/html_profile_writer.cpp
	src/amx_profiler/html_profile_writer.h
	src/amx_profiler/native_function.cpp
	src/amx_profiler/native_function.h
	src/amx_profiler/normal_function.cpp
	src/amx_profiler/normal_function.h
	src/amx_profiler/profile_writer.h
	src/amx_profiler/profiler.cpp
	src/amx_profiler/profiler.h
	src/amx_profiler/public_function.cpp
	src/amx_profiler/public_function.h
	src/amx_profiler/text_profile_writer.cpp
	src/amx_profiler/text_profile_writer.h
	src/amx_profiler/performance_counter.cpp
	src/amx_profiler/performance_counter.h
	src/amx_profiler/time_interval.h
	src/amx_profiler/xml_profile_writer.cpp
	src/amx_profiler/xml_profile_writer.h
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
	target_link_libraries(profiler ${Boost_LIBRARIES})
endif()

set_property(TARGET profiler PROPERTY PREFIX "")

if(UNIX OR MINGW)
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		COMPILE_FLAGS " -m32 -Wno-attributes")
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		LINK_FLAGS " -m32 -Wl,--no-undefined")
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++0x)
	if(UNIX)
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " -lrt")
	elseif(WIN32)
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

# Generate version.h to be included from source files
file(WRITE "src/version.h" 
"/* This file is auto-generated, do not edit */
#define PROFILER_VERSION \"${version}\"
#define PROFILER_VERSION_RC ${version_rc}
")

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

