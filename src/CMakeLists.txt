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

######## 3rd Party Libs ########
# Find the required Qt parts
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(QtVlc REQUIRED)
find_package(QtVlcWidgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtVlc_INCLUDE_DIRS})

######## libxml++ #########
#include(FindPkgConfig)
#pkg_check_modules(libxml++ REQUIRED libxml++-2.6)
#include_directories(${libxml++_INCLUDE_DIRS})

################################ FILES ################################

######## Headers ########
SET(VLYC_HEADERS
	vlyc.h

	util/cmdparser.h
	util/cmdparser_p.h

	gui/mainwindow.h
	gui/fullscreencontroller.h
	gui/videowidget.h
	gui/about.h

	network/simplefilecookiejar.h
	network/networkaccessmanager.h
)


######## Sources ########
SET(VLYC_SOURCES
	main.cpp
	vlyc.cpp

	util/cmdparser.cpp

	gui/mainwindow.cpp
	gui/fullscreencontroller.cpp
	gui/videowidget.cpp
	gui/about.cpp

	network/simplefilecookiejar.cpp
	network/networkaccessmanager.cpp
)


######## UIs ########
SET(VLYC_UIS
	gui/mainwindow.ui
	gui/fullscreencontroller.ui
	gui/about.ui
)

SET(VLYC_RES
	../vlyc.qrc
)

################################ COMPILE ################################
# Qt 5 stuff
QT5_WRAP_UI(VLYC_UI ${VLYC_UIS})
QT5_ADD_RESOURCES(VLYC_QRC ${VLYC_RES})

# Qt 5.0.x WM_CLASS hackery :( (sa: vlyc_xcb.*)
IF(UNIX AND NOT APPLE)
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
SET(VLYC_HEADERS ${VLYC_HEADERS} vlyc_xcb.h)
SET(VLYC_SOURCES ${VLYC_SOURCES} vlyc_xcb.cpp)
SET(VLYC_EXTRA_LIBS ${VLYC_EXTRA_LIBS} xcb)
ENDIF()

# Add executable
ADD_EXECUTABLE(vlyc2 ${VLYC_SOURCES} ${VLYC_HEADERS} ${VLYC_UI} ${VLYC_QRC} ${VLYC_BROWSER})

# Link
QT5_USE_MODULES(vlyc2 Core Gui Widgets Network WebKit WebKitWidgets)
TARGET_LINK_LIBRARIES(vlyc2 libvlyc2 ${QtVlc_LIBRARIES} ${QtVlcWidgets_LIBRARIES} ${VLYC_EXTRA_LIBS})

