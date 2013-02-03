include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(plugin.rc.in plugin.rc @ONLY)
configure_file(pluginversion.h.in pluginversion.h @ONLY)

set(SOURCES
	amxpath.cpp
	amxpath.h
	amxplugin.cpp
	configreader.cpp
	configreader.h
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
	list(APPEND SOURCES hook-win32.cpp)
elseif(UNIX)
	list(APPEND SOURCES hook-unix.cpp)
else()
	message(FATAL_ERROR Unsupported operating system)
endif()

if(UNIX AND NOT WIN32)
	add_definitions(-DLINUX)
endif()

add_library(plugin MODULE ${SOURCES})
target_link_libraries(plugin amx_profiler)

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

install(TARGETS plugin LIBRARY DESTINATION ".")