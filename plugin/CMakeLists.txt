include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_ALL_NO_LIB)

if(UNIX AND NOT WIN32)
	add_definitions(-DLINUX)
endif()

configure_file(plugin.rc.in plugin.rc @ONLY)
configure_file(pluginversion.h.in pluginversion.h @ONLY)

set(PLUGIN_SOURCES
	amxpath.cpp
	amxpath.h
	amxplugin.cpp
	configreader.cpp
	configreader.h
	fileutils.cpp
	fileutils.h
	hook.cpp
	hook.h
	plugin.h
	plugincommon.h
	plugin.cpp
	plugin.def
	plugin.rc
	pluginversion.h
)

if(WIN32)
	list(APPEND PLUGIN_SOURCES fileutils-win32.cpp)
else()
	list(APPEND PLUGIN_SOURCES fileutils-posix.cpp)
endif()

add_library(plugin MODULE ${PLUGIN_SOURCES})

if(UNIX OR MINGW)
	# Remove the "lib" prefix of the output filename added in GCC/MinGW builds.
	set_target_properties(plugin PROPERTIES PREFIX "")
endif()

if(UNIX OR MINGW)
	set_property(TARGET plugin APPEND_STRING PROPERTY 
		COMPILE_FLAGS " -m32 -Wno-attributes")
	set_property(TARGET plugin APPEND_STRING PROPERTY 
		LINK_FLAGS " -m32 -Wl,--no-undefined")
	if(WIN32)
		# Link with static runtime
		set_property(TARGET plugin APPEND_STRING PROPERTY 
			LINK_FLAGS " -static-libgcc -static-libstdc++")
	endif()
endif()

target_link_libraries(plugin amx_profiler ${Boost_LIBRARIES})

install(TARGETS plugin LIBRARY DESTINATION ".")