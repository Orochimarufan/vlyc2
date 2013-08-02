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

# plugin name
SET(PLUGIN_NAME python)

######## Prepare ########
# Plugin stuff
STRING(SUBSTRING "${PLUGIN_NAME}" 0 1 _FIRST)
STRING(TOUPPER "${_FIRST}" _FIRST)
STRING(REGEX REPLACE "^.(.*)" "${_FIRST}\\1Plugin" PLUGIN_PROJECT "${PLUGIN_NAME}")
project(${PLUGIN_PROJECT})
SET(PLUGIN vlyc2-${PLUGIN_NAME})
SET(PLUGIN_QT Core)
SET(PLUGIN_LIBS)
SET(PLUGIN_SOURCES)
SET(PLUGIN_HEADERS)

# Set compiler flags
IF(APPLE)
	# assume clang 4.1.0+, add C++0x/C++11 stuff
	message(STATUS "Using APPLE CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
ELSEIF(UNIX)
	# assume GCC, add C++0x/C++11 stuff
	MESSAGE(STATUS "Using UNIX CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ELSEIF(MINGW)
	MESSAGE(STATUS "Using MINGW CMAKE_CXX_FLAGS")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
ENDIF()

######## Plugin ########
## this is where custom stuff goes!
# Find PythonQt
find_package(PythonQt5_3_QtAll REQUIRED)

include_directories(${PythonQt_QtAll_INCLUDE_DIRS})
LIST(APPEND PLUGIN_LIBS ${PythonQt_QtAll_LIBRARIES})

LIST(APPEND PLUGIN_QT ${PythonQt_QtAll_QT})

# Find Python
find_package(Python ${PythonQt_Python} REQUIRED EXACT)
include_directories(${PYTHON_INCLUDE_DIRS})
LIST(APPEND PLUGIN_LIBS ${PYTHON_LIBRARIES})

# Sources
LIST(APPEND PLUGIN_HEADERS
pythonplugin.h
pythonsites.h
pythonqtdecorator.h
)

LIST(APPEND PLUGIN_SOURCES
pythonplugin.cpp
pythonsites.cpp
pythonqtdecorator.cpp
)

SET(PLUGINS
	youtube.py
)

######## Build ########
# Qt
FOREACH(module ${PLUGIN_QT})
	find_package(Qt5${module} REQUIRED)
ENDFOREACH(module)

# Qt stuff
qt5_add_resources(RES pythonplugin.qrc)
LIST(APPEND PLUGIN_SOURCES ${RES})

# Build
add_library(${PLUGIN} SHARED ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})
qt5_use_modules(${PLUGIN} ${PLUGIN_QT})
target_link_libraries(${PLUGIN} ${PLUGIN_LIBS})

foreach(plugin ${PLUGINS})
	set(INPUT ${PROJECT_SOURCE_DIR}/plugins/${plugin} )
	set(OUTPUT ${CMAKE_BINARY_DIR}/${plugin})
	add_custom_command(OUTPUT ${OUTPUT}
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy ${INPUT} ${OUTPUT}
		MAIN_DEPENDENCY ${INPUT}
		COMMENT "Copying Python plugin ${plugin}"
	)
	add_dependencies(${PLUGIN} ${OUTPUT})
endforeach()
