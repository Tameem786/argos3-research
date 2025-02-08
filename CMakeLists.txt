cmake_minimum_required(VERSION 2.8.12)
project(foraging_nn)

# Deactivate RPATH for MacOSX.
set(CMAKE_MACOSX_RPATH 0)

# Set the path where CMake will find additional scripts.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Find the ARGoS package.
find_package(ARGoS REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Check for required libraries to compile code depending on Qt and OpenGL.
include(FindARGoSQTOpenGL)

# Find the GSL package.
find_package(GSL REQUIRED)

# Find the LUA package.
find_package(Lua REQUIRED)

# Set ARGoS include directory.
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})

# Set ARGoS link directory.
link_directories(/usr/lib/argos3)
link_libraries(${ARGOS_LDFLAGS})

# Descend into the source code directory.
add_subdirectory(source)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")