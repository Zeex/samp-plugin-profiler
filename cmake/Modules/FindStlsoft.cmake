#----------------------------------------------------------------------------------
# (C) 2009 Sergey Nikulov
#
# Usage as follows:
#  - just add FIND_PACKAGE(Stlsoft) in your CMakeLists.txt, then check variables
#----------------------------------------------------------------------------------
# STLSOFT_FOUND       - TRUE/FALSE - Found/not found
# STLSOFT_INCLUDE_DIR - path to include
# STLSOFT_VER_MAJOR
# STLSOFT_VER_MINOR
# STLSOFT_VER_REVISION
#----------------------------------------------------------------------------------
# TODO: need to implement FIND_PACKAGE(Stlsoft [here required version string])
#----------------------------------------------------------------------------------

set(STLSOFT_FOUND FALSE)

# try get environment var first...
set(_STLSOFT_ROOT $ENV{STLSOFT})

if(_STLSOFT_ROOT)
       # add include folder to ENVIRONMENT path...

       file(TO_CMAKE_PATH ${_STLSOFT_ROOT} _STLSOFT_ROOT)
       set(STLSOFT_INCLUDE_DIR ${_STLSOFT_ROOT}/include)

else(_STLSOFT_ROOT)
       # try to find in well known for CMake includes

       find_path(STLSOFT_INCLUDE_DIR "stlsoft/stlsoft.h")              

endif(_STLSOFT_ROOT)

if(STLSOFT_INCLUDE_DIR)
       file(READ "${STLSOFT_INCLUDE_DIR}/stlsoft/stlsoft.h" _stlsoft_h_CONTENT)
       string(REGEX REPLACE ".*#define[ ]+_STLSOFT_VER_MAJOR[ ]+([0-9]+).*" "\\1" STLSOFT_VER_MAJOR "${_stlsoft_h_CONTENT}")
       string(REGEX REPLACE ".*#define[ ]+_STLSOFT_VER_MINOR[ ]+([0-9]+).*" "\\1" STLSOFT_VER_MINOR "${_stlsoft_h_CONTENT}")
       string(REGEX REPLACE ".*#define[ ]+_STLSOFT_VER_REVISION[ ]+([0-9]+).*" "\\1" STLSOFT_VER_REVISION "${_stlsoft_h_CONTENT}")

       MESSAGE("Found STLSOFT Ver. ${STLSOFT_VER_MAJOR}.${STLSOFT_VER_MINOR}.${STLSOFT_VER_REVISION} here - ${STLSOFT_INCLUDE_DIR}")

       if(STLSOFT_VER_MAJOR AND STLSOFT_VER_MINOR AND STLSOFT_VER_REVISION)

           set(STLSOFT_FOUND TRUE)

       endif(STLSOFT_VER_MAJOR AND STLSOFT_VER_MINOR AND STLSOFT_VER_REVISION)

else(STLSOFT_INCLUDE_DIR)
     message("No Stlsoft found... Please point us with STLSOFT env var")
endif(STLSOFT_INCLUDE_DIR)

MARK_AS_ADVANCED(
	STLSOFT_INCLUDE_DIR 
	STLSOFT_VER_MAJOR 
	STLSOFT_VER_MINOR 
	STLSOFT_VER_REVISION)
