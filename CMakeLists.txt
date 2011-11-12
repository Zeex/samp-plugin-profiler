cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(GetGitRevisionDescription)

# Configure Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.47 REQUIRED COMPONENTS chrono filesystem system)

# Disable the auto-link feature 
add_definitions(-DBOOST_ALL_NO_LIB)

function(check_include_and_define include_file define)
	check_include_file(${include_file} HAVE_${include_file})
	if(HAVE_${include_file})
		add_definitions(-D${define})
	endif()
endfunction()

check_include_and_define(alloca.h HAVE_ALLOCA_H)
check_include_and_define(inttypes.h HAVE_INTTYPES_H)
check_include_and_define(malloc.h HAVE_MALLOC_H)
check_include_and_define(stdint.h HAVE_STDINT_H)

include_directories("src/amx") # amx.h includes <sclinux.h>
include_directories(${Boost_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})

add_library(profiler MODULE
	"src/amx/amx.h"
	"src/amx/amxaux.c"
	"src/amx/amxaux.h"
	"src/amx/amxdbg.c"
	"src/amx/amxdbg.h"
	"src/amxname.cpp"
	"src/amxname.h"
	"src/amxplugin.cpp"
	"src/configreader.cpp"
	"src/configreader.h"
	"src/debuginfo.cpp"
	"src/debuginfo.h"
	"src/jump.cpp"
	"src/jump.h"
	"src/logprintf.cpp"
	"src/logprintf.h"
	"src/perfcounter.cpp"
	"src/perfcounter.h"
	"src/plugin.cpp"
	"src/plugin.h"
	"src/plugincommon.h"
	"src/printers.cpp"
	"src/printers.h"
	"src/profile.h"
	"src/profiler.cpp"
	"src/profiler.h"
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

function(get_version_string _version)
	git_describe(_describe)
	string(REGEX REPLACE "([0-9\\.]+)-g([a-f0-9]+)" "\\1" 
		version ${_describe})
	set(${_version} ${version} PARENT_SCOPE)
endfunction()

get_version_string(version)
message(STATUS "You're going to build profiler ${version}")

set_property(SOURCE "src/plugin.cpp" APPEND_STRING PROPERTY 
	COMPILE_DEFINITIONS "VERSION=\"${version}\"")

install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES "README.txt" "LICENSE.txt" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

