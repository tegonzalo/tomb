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

# Executables for Tomb
add_executable(tomb tomb.cpp)
