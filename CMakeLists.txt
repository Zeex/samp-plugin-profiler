project(profiler)

cmake_minimum_required(VERSION 2.8.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Make sure we have proper C++11 support.
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
	
include(AmxConfig)
include(GetGitRevisionDescription)

include_directories(.)
include_directories(amx)

set(SOURCES
	amx/amx.h
	amx/amxaux.c
	amx/amxaux.h
	amx/amxdbg.c
	amx/amxdbg.h
	amx/getch.h
	amx/osdefs.h
	amx/sclinux.h
	amx_profiler/call_graph.cpp
	amx_profiler/call_graph.h
	amx_profiler/call_graph_writer.h
	amx_profiler/call_graph_writer_gv.cpp
	amx_profiler/call_graph_writer_gv.h
	amx_profiler/call_stack.cpp
	amx_profiler/call_stack.h
	amx_profiler/debug_info.cpp
	amx_profiler/debug_info.h
	amx_profiler/duration.h
	amx_profiler/function.cpp
	amx_profiler/function.h
	amx_profiler/function_call.cpp
	amx_profiler/function_call.h
	amx_profiler/function_statistics.cpp
	amx_profiler/function_statistics.h
	amx_profiler/native_function.cpp
	amx_profiler/native_function.h
	amx_profiler/normal_function.cpp
	amx_profiler/normal_function.h
	amx_profiler/performance_counter.cpp
	amx_profiler/performance_counter.h
	amx_profiler/profile_writer.cpp
	amx_profiler/profile_writer.h
	amx_profiler/profile_writer_html.cpp
	amx_profiler/profile_writer_html.h
	amx_profiler/profile_writer_text.cpp
	amx_profiler/profile_writer_text.h
	amx_profiler/profile_writer_xml.cpp
	amx_profiler/profile_writer_xml.h
	amx_profiler/profiler.cpp
	amx_profiler/profiler.h
	amx_profiler/public_function.cpp
	amx_profiler/public_function.h
	amx_profiler/statistics.cpp
	amx_profiler/statistics.h
	plugin/amx_path.cpp
	plugin/amx_path.h
	plugin/amxplugin.cpp
	plugin/config_reader.cpp
	plugin/config_reader.h
	plugin/hook.cpp
	plugin/hook.h
	plugin/plugin.h
	plugin/plugincommon.h
	plugin/plugin.cpp
	plugin/plugin.def
	plugin/plugin.rc
	plugin/pluginversion.h
)

if(WIN32)
	list(APPEND SOURCES plugin/hook_win32.cpp)
elseif(UNIX)
	list(APPEND SOURCES plugin/hook_unix.cpp)
else()
	message(FATAL_ERROR Unsupported operating system)
endif()

source_group(amx REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/amx/[^/\\]+\\..*)
source_group(amx_profiler REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/amx_profiler/[^/\\]+\\..*)
source_group(plugin REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/plugin/[^/\\]+\\..*)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

if(UNIX OR MINGW)
	# Remove the "lib" prefix of the output filename added in GCC/MinGW builds.
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
endif()

if(UNIX OR MINGW)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY 
		COMPILE_FLAGS " -m32 -Wno-attributes")
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY 
		LINK_FLAGS " -m32 -Wl,--no-undefined")
	if(WIN32)
		# Link with static runtime
		set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY 
			LINK_FLAGS " -static-libgcc -static-libstdc++")
	endif()
endif()

if(UNIX AND NOT WIN32)
	add_definitions(-DLINUX)
endif()

if(WIN32)
	add_definitions(-DWIN32 -D_WIN32)
	if(MSVC)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
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

set(PROJECT_VERSION    ${version})
set(PROJECT_VERSION_RC ${version_rc})

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin.rc.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin.rc"
	@ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/pluginversion.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin/pluginversion.h"
	@ONLY)

message(STATUS "You're going to build ${PROJECT_NAME} v${version}")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ".")
install(FILES "README.md" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
