# Copyright (c) 2022 Connor Mellon
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.18)
project(ingenue)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(cmake/forceoutline.cmake)
include(cmake/tools.cmake)
include(cmake/warnings.cmake)

add_library(project_settings INTERFACE)
enable_sanitisers(project_settings)
set_project_warnings(project_settings)
target_compile_features(project_settings INTERFACE c_std_11)
target_compile_definitions(project_settings INTERFACE IN_DEBUG=$<CONFIG:DEBUG> _CRT_SECURE_NO_WARNINGS)
if(WIN32)
    target_compile_definitions(project_settings INTERFACE IN_DESKTOP=1 IN_WIN32=1 IN_LINUX=0)
elseif(LINUX)
    target_compile_definitions(project_settings INTERFACE IN_DESKTOP=1 IN_LINUX=1 IN_WIN32=0)
endif()

add_subdirectory(ingenue)
if(NOT IN_NOTEST)
    add_subdirectory(test)
endif()
