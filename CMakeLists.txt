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
project(tomb)
set(tomb_VERSION_MAJOR 1)
set(tomb_VERSION_MINOR 0)

# Directories
set(TOMB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# Includes
include_directories("${PROJECT_SOURCE_DIR}/include")

# Externals
set(TOMB_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(LIBJSON_SOURCE_DIR ${TOMB_EXTERNAL_DIR}/libjson/_internal/SOURCE)
file(GLOB libjson_sources "${LIBJSON_SOURCE_DIR}/[a-zA-Z]*.cpp")
file(GLOB libjson_headers "${LIBJOSN_SOURCE_DIR}/[a-zA-Z]*.h")
add_library(libjson STATIC ${libjson_srouces} ${libjson_headers})
include_directories("${LIBJSON_SOURCE_DIR}")
set_target_properties(libjson PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(EXTERNAL_LIBS ${EXTERNAL_LIBS} libjson)

# Executables for Tomb
add_executable(group ${TOMB_SOURCE_DIR}/group.cc)
target_link_libraries(group ${EXTERNAL_LIBS})
#add_dependencies(group ${EXTERN_LIBS})
