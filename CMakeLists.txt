cmake_minimum_required(VERSION 2.8.6)

project(profiler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(GetGitRevisionDescription)

# Configure Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.47 REQUIRED COMPONENTS chrono system)

function(check_include_and_define include_file define)
	check_include_file(${include_file} HAVE_${include_file})
	if(HAVE_${include_file})
		add_definitions(-D${define})
	endif()
endfunction()

function(check_cxx_include_and_define include_file define)
	check_include_file_cxx(${include_file} HAVE_${include_file})
	if(HAVE_${include_file})
		add_definitions(-D${define})
	endif()
endfunction()

check_include_and_define(alloca.h HAVE_ALLOCA_H)
check_include_and_define(inttypes.h HAVE_INTTYPES_H)
check_include_and_define(malloc.h HAVE_MALLOC_H)
check_include_and_define(stdint.h HAVE_STDINT_H)
check_cxx_include_and_define(memory HAVE_MEMORY)
check_cxx_include_and_define(tr1/memory HAVE_TR1_MEMORY)

include_directories("src/amx") # amx.h includes <sclinux.h>
include_directories(${Boost_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})

add_library(profiler MODULE
	"src/amx/amxaux.c"
	"src/amx/amxdbg.c"
	"src/amxname.cpp"
	"src/amxplugin.cpp"
	"src/configreader.cpp"
	"src/debuginfo.cpp"
	"src/fileutils.cpp"
	"src/jump.cpp"
	"src/logprintf.cpp"
	"src/perfcounter.cpp"
	"src/plugin.cpp"
	"src/profiler.cpp"
)

set_property(TARGET profiler PROPERTY PREFIX "")

target_link_Libraries(profiler ${Boost_LIBRARIES})

if(UNIX OR MINGW)
	set_property(TARGET profiler APPEND_STRING PROPERTY 
		COMPILE_FLAGS "-m32 -include stddef.h")
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

get_version_string(VERSION)
message(STATUS "Building Profiler ${VERSION}")

add_definitions(-DVERSION="${VERSION}")

install(TARGETS profiler LIBRARY DESTINATION ".")

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

