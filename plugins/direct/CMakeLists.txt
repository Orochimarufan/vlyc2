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

SET(PLUGIN_NAME direct)

######## Prepare ########
include(../prepare.cmake)

######## Plugin ########
## this is where custom stuff goes!
# Find libmagic
find_package(LibMagic)
if(LIBMAGIC_FOUND)
	include_directories(${LIBMAGIC_INCLUDE_DIR})
        list(APPEND PLUGIN_LIBS ${LIBMAGIC_LIBRARY})
	add_definitions(-DWITH_LIBMAGIC)
else()
	message(STATUS "Could not find libmagic development files. Falling back to fileext-based detection. It is recommended to use libmagic.")
endif()

list(APPEND PLUGIN_LIBS vlyc2)

list(APPEND PLUGIN_HEADERS
stubplugin.h
)

list(APPEND PLUGIN_SOURCES
stubplugin.cpp
)

######## Build ########
include(../build.cmake)
