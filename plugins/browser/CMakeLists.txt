#/*****************************************************************************
# * vlyc2 - A Desktop YouTube client
# * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
# *
# * This library is free software: you can redistribute it and/or modify
# * it under the terms of the GNU Lesser General Public License as published
# * by the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This library is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# * GNU Lesser General Public License for more details.
# *
# * You should have received a copy of the GNU Lesser General Public License
# * along with this library. If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/
#/* NOTE: the browser is modeled after the "Tab Browser" example found in the Qt
# * documentation, available under GPLv3 */

# plugin name
SET(PLUGIN_NAME browser)

######## Prepare ########
# Plugin stuff
STRING(SUBSTRING "${PLUGIN_NAME}" 0 1 _FIRST)
STRING(TOUPPER "${_FIRST}" _FIRST)
STRING(REGEX REPLACE "^.(.*)" "${_FIRST}\\1Plugin" PLUGIN_PROJECT "${PLUGIN_NAME}")
project(${PLUGIN_PROJECT})
SET(PLUGIN vlyc2-${PLUGIN_NAME})
SET(PLUGIN_QT Core Network WebKitWidgets)
SET(PLUGIN_LIBS libvlyc2)
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
# Sources
LIST(APPEND PLUGIN_HEADERS
	BrowserPlugin.h
	LinkContextMenu.h
	browser.h

	browserwindow.h
	navigationbar.h

	tabbar.h
	tabwidget.h

	webpage.h
	webview.h
)

LIST(APPEND PLUGIN_SOURCES
	BrowserPlugin.cpp
	LinkContextMenu.cpp
	browser.cpp

	browserwindow.cpp
	navigationbar.cpp

	tabbar.cpp
	tabwidget.cpp

	webpage.cpp
	webview.cpp
)

######## Build ########
# Qt
FOREACH(module ${PLUGIN_QT})
	find_package(Qt5${module} REQUIRED)
ENDFOREACH(module)

# Build
add_library(${PLUGIN} SHARED ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})
qt5_use_modules(${PLUGIN} ${PLUGIN_QT})
target_link_libraries(${PLUGIN} ${PLUGIN_LIBS})


