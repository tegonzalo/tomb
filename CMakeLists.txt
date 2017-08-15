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

# Set certain policies to NEW
foreach(p
  CMP0003 # CMake 2.6.0
  CMP0011 # Cmake 2.6.2
  CMP0012 # CMake 2.8.0
  CMP0022 # CMake 2.8.12
  CMP0025 # CMake 3.0
  CMP0042 # CMake 3.0
  CMP0051 # CMake 3.1
  CMP0054 # CMake 3.1
  )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Set up colours
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# Project Tomb
project(Tomb CXX)
set(TOMB_VERSION_MAJOR 1)
set(TOMB_VERSION_MINOR 0)

# Compiler flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# Include the main directory in search path for projects
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

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
  message("${BoldYellow}-- OpenMP enabled${ColourReset}")
else()
  message("${BoldRed}-- OpenMP disabled${ColourReset}")
endif()

# Mathematica
find_package(Mathematica 10.0)
if(Mathematica_FOUND)
  find_library(LIBUUID NAMES uuid)
  if(LIBUUID)
    message("${BoldYellow}-- Found library libuuid${ColourReset}")
    message("${BoldYellow}-- Found Mathematica${ColourReset}")
    if(Mathematica_MathLink_FOUND)
      message("${BoldYellow}-- Found MathLink. Mathematica frontend enabled${ColourReset}")
      set(HAVE_MATHEMATICA 1)
      set(MATHEMATICA_MATHLINK_H "${Mathematica_MathLink_INCLUDE_DIR}/mathlink.h")
#      set(MATHEMATICA_KERNEL "${Mathematica_KERNEL_EXECUTABLE}")
    else()
      message("${BoldRed}-- MathLink not found. Please make sure it is installed before attempting to use the Mathematica frontend${ColourReset}")
      set(HAVE_MATHEMATICA 0)
    endif()
  else()
    message("${BoldRed}-- Missing library libuuid required for MathLink. Mathematica will be disabled${ColourReset}")
    set(HAVE_MATHEMATICA 0)
  endif()
else()
  message("${BoldRed}-- Mathematica not found. Mathematica frontend disabled${ColourReset}")
  set(HAVE_MATHEMATICA 0)
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

# Set variables for compiler
configure_file("${PROJECT_SOURCE_DIR}/include/cmake_variables.h.in" "${UTILS_INCLUDE_DIR}/cmake_variables.h" )

# Externals, libjson
set(TOMB_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(LIBJSON_SOURCE_DIR ${TOMB_EXTERNAL_DIR}/libjson/_internal/Source)
set(LIBJSON_INCLUDE_DIR ${TOMB_EXTERNAL_DIR}/libjson)
file(GLOB libjson_sources "${LIBJSON_SOURCE_DIR}/[a-zA-Z]*.cpp")
file(GLOB libjson_external_headers "${LIBJSON_INCLUDE_DIR}/[a-zA-Z]*.h")
file(GLOB_RECURSE libjson_internal_headers "${LIBJSON_SOURCE_DIR}/[a-zA-Z]*.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
add_library(libjson OBJECT ${libjson_sources} ${libjson_external_headers} ${libjson_internal_headers})
include_directories(${LIBJSON_INCLUDE_DIR} ${LIBJSON_SOURCE_DIR})

# Utils
file(GLOB utils_sources "${UTILS_SOURCE_DIR}/[a-zA-Z]*.cc")
file(GLOB utils_headers "${UTILS_INCLUDE_DIR}/[a-zA-Z]*.h")
add_library(utils OBJECT ${utils_sources} ${utils_headers})
add_dependencies(utils libjson)
include_directories(${UTILS_INCLUDE_DIR})

# Group
file(GLOB group_sources "${GROUP_SOURCE_DIR}/[a-zA-Z]*.cc")
file(GLOB group_headers "${GROUP_INCLUDE_DIR}/[a-zA-Z]*.h")
set(group_main "${PROJECT_SOURCE_DIR}/src/group.cc")
add_library(libgroup OBJECT ${group_sources} ${group_headers})
add_dependencies(libgroup utils libjson)
set(group_objects $<TARGET_OBJECTS:libgroup> $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:libjson>)
add_executable(group ${group_main} ${group_objects})
#add_dependencies(group utils libjson)
include_directories(${GROUP_INCLUDE_DIR})

# Mathgroup
if (${HAVE_MATHEMATICA})
  set(group_mathmain "${PROJECT_SOURCE_DIR}/src/mathgroup.cc")
  set(mathlink_template "${PROJECT_SOURCE_DIR}/src/mathgroup.tm")
  set(mathlink_source "${PROJECT_SOURCE_DIR}/src/mathgrouptm.cc")
  add_custom_command(OUTPUT ${mathlink_source}
                     COMMAND ${Mathematica_MathLink_INCLUDE_DIR}/mprep  ${mathlink_template} -o ${mathlink_source})
  add_library(mathlink OBJECT ${mathlink_source})
  include_directories(${Mathematica_MathLink_INCLUDE_DIR})
  set(mathgroup_objects $<TARGET_OBJECTS:libgroup> $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:libjson> $<TARGET_OBJECTS:mathlink>)
  add_executable(mathgroup ${group_mathmain} ${mathgroup_objects})
#  add_dependencies(mathgroup utils libjson mathlink)
  include_directories(${GROUP_INCLUDE_DIR})
  target_link_libraries(mathgroup ${Mathematica_MathLink_LIBRARY} uuid rt dl)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    Mathematica_ABSOLUTIZE_LIBRARY_DEPENDENCIES(mathgroup)
  endif()
endif()

# Tomb
file(GLOB tomb_sources "${TOMB_SOURCE_DIR}/[a-zA-Z]*.cc")
file(GLOB tomb_headers "${TOMB_INCLUDE_DIR}/[a-zA-Z]*.h")
set(tomb_main "${PROJECT_SOURCE_DIR}/src/tomb.cc")
set(tomb_objects $<TARGET_OBJECTS:libgroup> $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:libjson>)
add_executable(tomb ${tomb_main} ${tomb_sources} ${tomb_objects} ${tomb_headers} )
#add_dependencies(tomb utils libjson)
include_directories(${TOMB_INCLUDE_DIR} ${GROUP_INCLUDE_DIR})
