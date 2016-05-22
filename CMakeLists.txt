#CMAKE VERSION
cmake_minimum_required(VERSION 2.8)

#CMAKE POLICY
#cmake_policy(SET CMP0003 NEW)

project(ohio)

#INCLUDE PATHS
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ext)
#include_directories(${PROJECT_SOURCE_DIR}/../versor/include)
include_directories(${PROJECT_SOURCE_DIR}/ext/hana/include)

#LINK PATHS
link_directories(${PROJECT_BINARY_DIR})

#LIBRARY
add_library(json ext/json/jsoncpp.cpp)
set(libraries json)

#CXX FLAGS (C++14 required)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -o0")

#EXECUTABLE
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
file(GLOB tests tests/*.cpp)
foreach(filename ${tests})
  get_filename_component(target ${filename} NAME_WE)
  add_executable(${target} ${filename} )
  target_link_libraries(${target} ${libraries} )
endforeach()


# #BELOW CODE IS USED FOR GRAPHICS TESTING!!!
# #OPTIONAL ADD GFX AS SUBDIRECTORY TO BUILD GRAPHICS DEMOS
# add_subdirectory(ext/gfx)
#
# #GET INCLUDE DIRECTORIES, LINK DIRECTORIES, AND LINKED LIBS
# get_property(GFX_LIBRARIES GLOBAL PROPERTY GFX_LIBRARIES)
# get_property(GFX_INCLUDE_DIR GLOBAL PROPERTY GFX_INCLUDE_DIR)
# get_property(GFX_LINK_DIR GLOBAL PROPERTY GFX_LINK_DIR)
#
# include_directories(${GFX_INCLUDE_DIR})
# link_directories(${GFX_LINK_DIR})
