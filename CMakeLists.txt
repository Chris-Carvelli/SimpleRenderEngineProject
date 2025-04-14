# author: Chris (original from mortennobel)
# NEEDED:
# -include mixer
# - include bullet
# TODO:
# - incude assimp
# - make cross-platform
# - make more robust
# - refactor lib/include variables to make them more friendly towards target_include_directories and target_link_libraries

cmake_minimum_required(VERSION 3.8)
project(SimpleGameEngine CXX)

set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# build sre library
add_subdirectory(submodules/SimpleRenderEngine)
get_directory_property(SRE_INCLUDE DIRECTORY submodules/SimpleRenderEngine DEFINITION SRE_INCLUDE)
get_directory_property(SRE_LIBRARIES DIRECTORY submodules/SimpleRenderEngine DEFINITION SRE_LIBRARIES)
get_directory_property(SRE_LIBRARIES_SHARED DIRECTORY submodules/SimpleRenderEngine DEFINITION SRE_LIBRARIES_SHARED)

# custom, centralized path to dependencies
# set(SGE_LIBS_PATH "" CACHE PATH "Path to the SimpleGameEngine libraries")
set(SGE_LIBS_PATH ${SRE_LIBS_PATH})

# where to look for cmake "find" modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


############################################################################
# Collect SGE libraries
############################################################################

# mixer
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2MIXER_INCLUDE_DIR})

# box2d
find_package(Box2D)
include_directories(${BOX_2D_INCLUDE_DIR})


# lua
find_package(lua)
include_directories(${LUA_INCLUDE_DIR})

# 3rd party, head only libraries
include_directories(3rdparty)


include_directories(${SRE_INCLUDE})

# prepare static library list
set(SGE_LIBRARIES SRE ${SRE_LIBRARIES} ${SDL2MIXER_LIBRARY} ${BOX2D_LIBRARY} ${LUA_LIBRARY})

# prepare dynamic library list
set(SGE_LIBRARIES_SHARED ${SRE_LIBRARIES_SHARED} ${SDL2_MIXER_SHARED_LIBS} ${LUA_LIBRARY_SHARED})

message("424241: ${SRE_INCLUDE}")
message("424242: ${SGE_LIBRARIES}")
message("424243: ${SGE_LIBRARIES_SHARED}")

# build targets using sre
SET(USE_SRE_PROJECT_EXAMPLES ON CACHE BOOL "Enable project examples for SGE")
if (USE_SRE_PROJECT_EXAMPLES)
add_subdirectory(project)
endif(USE_SRE_PROJECT_EXAMPLES)

# WIP mark advanced cmake variables (TODO move this in the appropriate files/sections)

### Cmake
mark_as_advanced(
	CMAKE_INSTALL_PREFIX
	CMAKE_MAKE_PROGRAM
)

### SRE
mark_as_advanced(
	SRE_INCLUDE
	EXTRA_SOURCE_FILES
	USE_OPENVR
	USE_SRE_TEST_AND_UTILS
)

# SDL2
mark_as_advanced(
	SDL2_LIBRARY
	SDL2_LIBRARY_SHARED
	SDL2_INCLUDE_DIR
	EXTRA_INCLUDE
)

# SDL2_image
mark_as_advanced(
	SDL2_IMAGE_LIBRARY_SHARED
	SDL2_IMAGE_LIBRARIES
	SDL2_IMAGE_INCLUDE_DIRS
)

# glew
mark_as_advanced(
	GLEW_SOURCE
	GLEW_LIBRARY
	GLEW_INCLUDE_DIR
)


### SGE

# SDL2_mixer
mark_as_advanced(
	SDL2MIXER_LIBRARY
	SDL2MIXER_INCLUDE_DIR
	SDL2_MIXER_LIBRARY_SHARED
)

# lua
mark_as_advanced(
	LUA_LIBRARY_SHARED
)

### misc
mark_as_advanced(
	SDL2MAIN_LIBRARY
)

add_subdirectory(PlatformerProject)