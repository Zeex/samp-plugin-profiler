include(AmxConfig)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
endif()

add_library(amx_profiler STATIC
	call_graph.cpp
	call_graph.h
	call_graph_writer.h
	call_graph_writer_gv.cpp
	call_graph_writer_gv.h
	call_stack.cpp
	call_stack.h
	debug_info.cpp
	debug_info.h
	duration.h
	function.cpp
	function.h
	function_call.cpp
	function_call.h
	function_statistics.cpp
	function_statistics.h
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
)

target_link_libraries(amx_profiler amx)