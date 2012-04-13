cmake_minimum_required(VERSION 2.8)

include(AmxConfig)

include_directories("./")
include_directories("./amx")

add_subdirectory("amx")
list(APPEND PROFILER_DEPS amx)

add_subdirectory("amx_profiler")
list(APPEND PROFILER_DEPS amx_profiler)

add_library(profiler MODULE
	"amx_name.cpp"
	"amx_name.h"
	"amxplugin.cpp"
	"config_reader.cpp"
	"config_reader.h"
	"jump-x86.cpp"
	"jump-x86.h"
	"plugin.h"
	"plugincommon.h"
	"plugin.cpp"
	"plugin.rc"
	"version.h"
)

target_link_libraries(profiler ${PROFILER_DEPS})

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

set_property(TARGET profiler PROPERTY PREFIX "")

if(UNIX OR MINGW)
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		COMPILE_FLAGS " -m32 -Wno-attributes")
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		LINK_FLAGS " -m32 -Wl,--no-undefined")
	if(WIN32)
		# Link with static runtime
		set_property(TARGET profiler APPEND_STRING PROPERTY 
			LINK_FLAGS " -static-libgcc -static-libstdc++")
	else()
		add_definitions(-DLINUX)
	endif()
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

string(REGEX REPLACE "[.-]" "," PROFILER_VERSION_RC ${PROFILER_VERSION})
# Append one or two ",0" to the end to make windres happy
string(REGEX REPLACE "^([0-9]+,[0-9]+)$" "\\1,0,0" PROFILER_VERSION_RC ${PROFILER_VERSION_RC})
string(REGEX REPLACE "^([0-9]+,[0-9]+,[0-9]+)$" "\\1,0" PROFILER_VERSION_RC ${PROFILER_VERSION_RC})

# Generate version.h and plugin.rc
set(PLUGIN_NAME        "profiler")
set(PLUGIN_DESCRIPTION "Profiler plugin for SA-MP server")
set(PLUGIN_COPYRIGHT   "Copyright (c) 2011-2012 Zeex")
set(PLUGIN_VERSION     ${PROFILER_VERSION})              # PROFILER_VERSION is defined by top-level CMakeLists
set(PLUGIN_VERSION_RC  ${PROFILER_VERSION_RC})
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.rc.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.rc"
	@ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h"
	@ONLY)

# CPack rules begin here #
install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES "README.md" DESTINATION ".")
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROFILER_VERSION}-win32")
	set(CPACK_GENERATOR "ZIP")
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROFILER_VERSION}-linux")
	set(CPACK_GENERATOR "TGZ")
endif()
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
include(CPack)
