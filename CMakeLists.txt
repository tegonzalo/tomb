#
# ****************************
# TOMB: Tool of Model Building
# ****************************
#
# CMakeLists.txt
# Created by T. Gonzalo (t.e.gonzalo@fys.uio.no) on 01/11/2016
#

# Minimum CMAKE version required
cmake_minimum_required(VERSION 2.6)

# Project Tomb
project(Tomb CXX)
set(tomb_VERSION_MAJOR 1)
set(tomb_VERSION_MINOR 0)

# Compiler flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# Check for C++11 and C++14 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(TOMB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/tomb)
set(GROUP_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/group)
set(UTILS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/utils)
set(TOMB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/tomb)
set(GROUP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/group)
set(UTILS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/utils)

# Externals
set(TOMB_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(LIBJSON_SOURCE_DIR ${TOMB_EXTERNAL_DIR}/libjson/_internal/Source)
file(GLOB libjson_sources "${LIBJSON_SOURCE_DIR}/[a-zA-Z]*.cpp")
file(GLOB libjson_headers "${LIBJSON_SOURCE_DIR}/[a-zA-Z]*.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
add_library(libjson STATIC ${libjson_sources} ${libjson_headers})
set_target_properties(libjson PROPERTIES LINKER_LANGUAGE CXX)
include_directories("${TOMB_EXTERNAL_DIR}/libjson")

# Utils
file(GLOB utils_sources "${UTILS_SOURCE_DIR}/[a-zA-Z]*.cc")
file(GLOB utils_headers "${UTILS_INCLUDE_DIR}/[a-zA-Z]*.h")
add_library(utils OBJECT ${utils_sources} ${utils_headers})
add_dependencies(utils libjson)
include_directories(${UTILS_INCLUDE_DIR})

# Group
file(GLOB group_sources "${GROUP_SOURCE_DIR}/[a-zA-Z]*.cc")
file(GLOB group_headers "${GROUP_INCLUDE_DIR}/[a-zA-Z]*.h")
set(group_objects $<TARGET_OBJECTS:utils>)
add_executable(group ${group_sources} ${group_objects} ${group_headers})
add_dependencies(group utils)
target_link_libraries(group libjson)
include_directories(${GROUP_INCLUDE_DIR})
