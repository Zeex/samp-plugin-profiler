include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(subhook)

add_definitions(-DSUBHOOK_STATIC)
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
	plugin.h
	plugincommon.h
	plugin.cpp
	plugin.def
	${CMAKE_CURRENT_BINARY_DIR}/plugin.rc
	${CMAKE_CURRENT_BINARY_DIR}/pluginversion.h
)

if(WIN32)
	list(APPEND PLUGIN_SOURCES fileutils_win32.cpp)
else()
	list(APPEND PLUGIN_SOURCES fileutils_posix.cpp)
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
endif()

set(SUBHOOK_STATIC TRUE)
add_subdirectory(subhook)

target_link_libraries(plugin amx_profiler subhook)

install(TARGETS plugin LIBRARY DESTINATION ".")