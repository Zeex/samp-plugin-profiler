project(profiler)

option(PROFILER_USE_STATIC_RUNTIME "Use static C++ runtime" OFF)

cmake_minimum_required(VERSION 2.8.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(AmxConfig)
include(GetGitRevisionDescription)

# Enable project folders in Visual Studio (and possibly other IDEs).
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Suppress annoying GCC warning about 'packed' attribute being ignored.
if(CMAKE_COMPILER_IS_GNUCC)
	if(NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.0)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
	endif()
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.0)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
	endif()
endif()

# Link against the static version of libstdc++.
if(PROFILER_USE_STATIC_RUNTIME)
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
	endif()
endif()

git_describe(description --match "v[0-9]*.[0-9]**")
if(description)
	string(REGEX REPLACE "\\-g[0-9a-f]+$" "" description ${description})
	string(REGEX REPLACE "^v(.*)" "\\1" version ${description})
else()
	message(STATUS "Failed to get version from Git, will read VERSION.txt")
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.txt" version)
	string(STRIP ${version} version)
endif()

# Comma-separated version for RC.
string(REGEX REPLACE "[.-]" "," version_rc ${version})
# Append one or two ",0" to the end to make windres happy.
string(REGEX REPLACE "^([0-9]+,[0-9]+)$" "\\1,0,0" version_rc ${version_rc})
string(REGEX REPLACE "^([0-9]+,[0-9]+,[0-9]+)$" "\\1,0" version_rc ${version_rc})

set(PROJECT_VERSION    ${version})
set(PROJECT_VERSION_RC ${version_rc})

add_subdirectory(amx)
add_subdirectory(amx_profiler)
add_subdirectory(plugin)

set_target_properties(plugin PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(FILES "README.md" DESTINATION ".")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-win32")
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-linux")
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

message(STATUS "You're going to build ${PROJECT_NAME} v${PROJECT_VERSION}")