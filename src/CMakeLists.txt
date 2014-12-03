include(AmxConfig)
include(Plugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DSUBHOOK_STATIC)
if(UNIX AND NOT WIN32)
  add_definitions(-DLINUX)
endif()

configure_file(plugin.rc.in plugin.rc @ONLY)
configure_file(pluginversion.h.in pluginversion.h @ONLY)

set(PROFILER_SOURCES
  amxpath.cpp
  amxpath.h
  amxplugin.cpp
  amxservice.h
  configreader.cpp
  configreader.h
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
  profiler.cpp
  profiler.h
)

if(WIN32)
  list(APPEND PROFILER_SOURCES fileutils_win32.cpp)
else()
  list(APPEND PROFILER_SOURCES fileutils_posix.cpp)
endif()

add_plugin(profiler ${PROFILER_SOURCES})

add_subdirectory(amx)
add_subdirectory(amxprof)
target_link_libraries(profiler amxprof subhook)

install(TARGETS profiler LIBRARY DESTINATION ".")
