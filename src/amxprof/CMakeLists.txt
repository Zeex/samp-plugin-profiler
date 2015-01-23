include(AMXConfig)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(AMXPROF_SOURCES
  amx_types.h
  amx_utils.cpp
  amx_utils.h
  call_graph.cpp
  call_graph.h
  call_graph_writer.cpp
  call_graph_writer.h
  call_graph_writer_dot.cpp
  call_graph_writer_dot.h
  call_stack.cpp
  call_stack.h
  clock.h
  debug_info.cpp
  debug_info.h
  duration.h
  exception.h
  function.cpp
  function.h
  function_call.cpp
  function_call.h
  function_statistics.cpp
  function_statistics.h
  macros.h
  performance_counter.cpp
  performance_counter.h
  profiler.cpp
  profiler.h
  statistics.cpp
  statistics.h
  statistics_writer.cpp
  statistics_writer.h
  statistics_writer_html.cpp
  statistics_writer_html.h
  statistics_writer_text.cpp
  statistics_writer_text.h
  statistics_writer_json.cpp
  statistics_writer_json.h
  stdint.h
  system_error.h
  time_utils.cpp
  time_utils.h
)

if(WIN32)
  list(APPEND AMXPROF_SOURCES
    clock_win32.cpp
    system_error_win32.cpp
  )
else()
  list(APPEND AMXPROF_SOURCES
    clock_posix.cpp
    system_error_posix.cpp
  )
endif()

add_library(amxprof STATIC ${AMXPROF_SOURCES})

target_link_libraries(amxprof amx)
if(UNIX)
  target_link_libraries(amxprof rt)
endif()
