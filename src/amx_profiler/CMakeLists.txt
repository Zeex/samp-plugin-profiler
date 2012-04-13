cmake_minimum_required(VERSION 2.8)

include(AmxConfig)

find_package(Boost 1.47 COMPONENTS chrono date_time system)
if(Boost_FOUND)
	add_definitions(-DHAVE_BOOST=${Boost_VERSION})
	# Disable Boost auto-link feature 
	add_definitions(-DBOOST_ALL_NO_LIB)
	# Work around swprintf() error related to lexical_cast<> on MinGW
	add_definitions(-DBOOST_LCAST_NO_WCHAR_T)
	if(Boost_CHRONO_FOUND)
		add_definitions(-DHAVE_BOOST_CHRONO)
	endif()
	if(Boost_DATE_TIME_FOUND)
		add_definitions(-DHAVE_BOOST_DATE_TIME)
	endif()
	include_directories(${Boost_INCLUDE_DIRS})
	if(CMAKE_COMPILER_IS_GNUCXX)
		# Chrono needs -lrt for clock_gettime()
		set(Boost_LIBRARIES ${Boost_LIBRARIES} -lrt)
	endif()
	list(APPEND AMX_PROFILER_DEPS ${Boost_LIBRARIES})
endif()

add_library(amx_profiler STATIC
	"c_interface.cpp"
	"c_interface.h"
	"call_graph.cpp"
	"call_graph.h"
	"call_graph_writer.h"
	"call_graph_writer_gv.cpp"
	"call_graph_writer_gv.h"
	"call_stack.cpp"
	"call_stack.h"
	"chrono.h"
	"debug_info.cpp"
	"debug_info.h"
	"function.cpp"
	"function.h"
	"function_call.cpp"
	"function_call.h"
	"function_info.cpp"
	"function_info.h"
	"profile_writer_html.cpp"
	"profile_writer_html.h"
	"native_function.cpp"
	"native_function.h"
	"normal_function.cpp"
	"normal_function.h"
	"profile_writer.h"
	"profiler.cpp"
	"profiler.h"
	"public_function.cpp"
	"public_function.h"
	"profile_writer_text.cpp"
	"profile_writer_text.h"
	"performance_counter.cpp"
	"performance_counter.h"
	"time_interval.h"
	"time_res.h"
	"profile_writer_xml.cpp"
	"profile_writer_xml.h"
)

if(DEFINED AMX_PROFILER_DEPS)
	target_link_libraries(amx_profiler ${AMX_PROFILER_DEPS})
endif()
