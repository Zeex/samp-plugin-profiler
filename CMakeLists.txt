cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(GetGitRevisionDescription)

# Configure Boost
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
	"src/jump-x86.cpp"
	"src/jump-x86.h"
	"src/logprintf.cpp"
	"src/logprintf.h"
	"src/perfcounter.cpp"
	"src/perfcounter.h"
	"src/plugin.cpp"
	"src/plugin.h"
	"src/plugin.rc"
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

git_describe(description --match v[0-9]*.[0-9]*.[0-9]*)
if(NOT description STREQUAL "GIT-NOTFOUND" AND NOT description STREQUAL "")
	string(REGEX REPLACE "v([0-9]+)\\.([0-9]+)\\.([0-9]+)-?([0-9]*)-?g?([a-f0-9]*)" 
		"\\1;\\2;\\3;\\4;\\5" version_components ${description})
	list(GET version_components 0 version_major)
	list(GET version_components 1 version_minor)
	list(GET version_components 2 version_patch)
	list(GET version_components 3 version_commit)
	if(version_commit STREQUAL "")
		set(version_commit 0)
		set(dotted_version ${version_major}.${version_minor}.${version_patch})
	else()
		set(dotted_version ${version_major}.${version_minor}.${version_patch}.${version_commit})
	endif()
else()
	message(WARNING "Can't extract version info: Git not found")
	set(version_major  0)
	set(version_minor  0)
	set(version_patch  0)
	set(version_commit 0)
	set(dotted_version "0.0.0")
endif()

file(WRITE "src/version.h" 
"/* Auto-generated, do not edit */
#define VERSION_MAJOR ${version_major}
#define VERSION_MINOR ${version_minor}
#define VERSION_PATCH ${version_patch}
#define VERSION_COMMIT ${version_commit}
#define VERSION_STRING \"${dotted_version}\"
")

message(STATUS "You're going to build Profiler v${dotted_version}")

install(TARGETS profiler LIBRARY DESTINATION ".")
install(FILES "README.txt" "LICENSE.txt" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${dotted_version}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${dotted_version}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

