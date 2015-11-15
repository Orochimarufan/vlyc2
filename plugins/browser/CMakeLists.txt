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
include(../prepare.cmake)

######## Plugin ########
## this is where custom stuff goes!
find_package(QtVlc) #HACK until public interfaces
include_directories(${QtVlc_INCLUDE_DIRS})
list(APPEND PLUGIN_LIBS vlyc2)

# Sources
LIST(APPEND PLUGIN_HEADERS
	BrowserPlugin.h
	LinkContextMenu.h
	browser.h

        bookmark.h
        bookmarkdialog.h

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

        bookmark.cpp
        bookmarkdialog.cpp

	browserwindow.cpp
	navigationbar.cpp

	tabbar.cpp
	tabwidget.cpp

	webpage.cpp
	webview.cpp
)

LIST(APPEND PLUGIN_FORMS
        bookmarkdialog.ui
)

LIST(APPEND PLUGIN_QT WebKitWidgets)

######## Build ########
include(../build.cmake)
