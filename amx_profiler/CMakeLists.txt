include(AmxConfig)

include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_ALL_NO_LIB)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
endif()

set(AMX_PROFILER_SOURCES
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
	function.cpp
	function.h
	function_call.cpp
	function_call.h
	function_statistics.cpp
	function_statistics.h
	macros.h
	native_function.cpp
	native_function.h
	normal_function.cpp
	normal_function.h
	performance_counter.cpp
	performance_counter.h
	profiler.cpp
	profiler.h
	public_function.cpp
	public_function.h
	statistics.cpp
	statistics.h
	statistics_writer.cpp
	statistics_writer.h
	statistics_writer_html.cpp
	statistics_writer_html.h
	statistics_writer_text.cpp
	statistics_writer_text.h
	statistics_writer_xml.cpp
	statistics_writer_xml.h
	stdint.h
	time_utils.cpp
	time_utils.h
)

if(WIN32)
	list(APPEND AMX_PROFILER_SOURCES clock-win32.cpp)
else()
	list(APPEND AMX_PROFILER_SOURCES clock-posix.cpp)
endif()

add_library(amx_profiler STATIC ${AMX_PROFILER_SOURCES})

target_link_libraries(amx_profiler amx ${Boost_LIBRARIES})
if(UNIX)
	target_link_libraries(amx_profiler rt)
endif()