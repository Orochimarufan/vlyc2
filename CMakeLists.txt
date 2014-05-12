#/*****************************************************************************
# * vlyc2 - A Desktop YouTube client
# * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/

cmake_minimum_required(VERSION 2.8.10)

IF(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

######## Qt ########
find_package(Qt5Core REQUIRED)

######## VLYC ########
project(VLYC)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Output all executables and shared libs in the main build folder, not in subfolders.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

######## Set CMake options ########
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

######## Set compiler flags ########
IF(APPLE)
	# assume clang 4.1.0+, add C++0x/C++11 stuff
	message(STATUS "Using APPLE CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
ELSEIF(UNIX)
	# assume GCC, add C++0x/C++11 stuff
	MESSAGE(STATUS "Using UNIX CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	# Need extra -Xlinker -export-dynamic for python C extensions.
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export-dynamic")
ELSEIF(MINGW)
	MESSAGE(STATUS "Using MINGW CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
ENDIF()

### include plugin defs ###
include_directories(${PROJECT_SOURCE_DIR}/shared ${PROJECT_SOURCE_DIR}/shared/legacy)

######## Shared library ########
add_subdirectory(shared)

######## Main App ########
add_subdirectory(src)

######## plugins ########
add_subdirectory(plugins)
