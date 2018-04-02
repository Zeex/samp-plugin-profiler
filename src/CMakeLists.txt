include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX)
  add_definitions(-DAMXPROF_RELOCATE_OPCODES)
endif()

configure_file(plugin.rc.in plugin.rc @ONLY)
configure_file(pluginversion.h.in pluginversion.h @ONLY)

set(PROFILER_SOURCES
  amxhandler.h
  amxpathfinder.cpp
  amxpathfinder.h
  amxplugin.cpp
  fileutils.cpp
  fileutils.h
  logprintf.cpp
  logprintf.h
  natives.cpp
  natives.h
  plugin.h
  plugincommon.h
  plugin.cpp
  plugin.def
  ${CMAKE_CURRENT_BINARY_DIR}/plugin.rc
  ${CMAKE_CURRENT_BINARY_DIR}/pluginversion.h
  profilerhandler.cpp
  profilerhandler.h
  stringutils.cpp
  stringutils.h
)

if(WIN32)
  list(APPEND PROFILER_SOURCES fileutils_win32.cpp)
else()
  list(APPEND PROFILER_SOURCES fileutils_posix.cpp)
endif()

add_samp_plugin(profiler ${PROFILER_SOURCES})

add_subdirectory(amx)
add_subdirectory(amxprof)
target_link_libraries(profiler amxprof configreader subhook)

install(TARGETS profiler LIBRARY DESTINATION ".")
