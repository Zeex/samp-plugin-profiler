cmake_minimum_required (VERSION 2.8)

project (profiler)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set (VERSION_MAJOR 2)
set (VERSION_MINOR 1)
set (VERSION_PATCH 3)

add_definitions (-DVERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

include (CheckIncludeFile)

check_include_file (alloca.h HAVE_ALLOCA_H)
if (HAVE_ALLOCA_H)
    add_definitions (-DHAVE_ALLOCA_H)
endif (HAVE_ALLOCA_H)
check_include_file (inttypes.h HAVE_INTTYPES_H)
if (HAVE_INTTYPES_H)
    add_definitions (-DHAVE_INTTYPES_H)
endif (HAVE_INTTYPES_H)
check_include_file (malloc.h HAVE_MALLOC_H)
if (HAVE_MALLOC_H)
    add_definitions (-DHAVE_MALLOC_H)
endif (HAVE_MALLOC_H)
check_include_file (stdint.h HAVE_STDINT_H)     
if (HAVE_STDINT_H)
    add_definitions (-DHAVE_STDINT_H)
endif (HAVE_STDINT_H)

find_package (Stlsoft REQUIRED)

include_directories ("src/amx") # amx.h includes <sclinux.h>
include_directories (${STLSOFT_INCLUDE_DIR})

if (UNIX OR MINGW)
    set (CMAKE_C_FLAGS   "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined -static-libstdc++")
endif (UNIX OR MINGW)

if (UNIX AND NOT WIN32)
    add_definitions (-DLINUX)
endif (UNIX AND NOT WIN32)

if (WIN32)
    add_definitions (-DWIN32)
    set (MODULE_DEFINITION_FILE "${profiler_SOURCE_DIR}/src/plugin.def")
    if (MSVC)
        set (CMAKE_MODULE_LINKER_FLAGS "/DEF:${MODULE_DEFINITION_FILE}")
    elseif (MINGW)
        set (CMAKE_MODULE_LINKER_FLAGS "-Wl,--kill-at --def ${MODULE_DEFINITION_FILE}")
    endif (MSVC)
endif (WIN32)

set (CMAKE_SHARED_MODULE_PREFIX "") # Don't prefix the output .so with "lib"

add_library (profiler MODULE
    "src/amx/amxaux.c"
    "src/amx/amxdbg.c"
    "src/amxname.cpp"
    "src/amxplugin.cpp"
    "src/debuginfo.cpp"
    "src/fileutils.cpp"
    "src/jump.cpp"
    "src/logprintf.cpp"
    "src/perfcounter.cpp"
    "src/plugin.cpp"
    "src/profiler.cpp"
    "src/servercfg.cpp"
)

target_link_Libraries (profiler)
