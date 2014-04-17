#/*****************************************************************************
# * vlyc2 - A Desktop YouTube client
# * Copyright (C) 2013-2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

set(HEADERS
../include/plugin.h
../include/siteplugin.h
../include/video.h
../include/videoimpl.h
../include/vlyc2pointer.h

VlycPlugin.h
VlycToolPlugin.h
VlycLegacySitePlugin.h
VlycLoaderPlugin.h
VlycQtPluginLoader.h
VlycPluginManager.h
VlycUrlHandlerPlugin.h

VlycResult/Result.h
VlycResult/Url.h
VlycResult/Playlist.h
)

set(PRIVATE_HEADERS
VlycPluginManagerPrivate.h
)

set(SOURCES
video.cpp
vlyc2pointer.cpp

VlycPlugin.cpp
VlycLegacySitePlugin.cpp
VlycLoaderPlugin.cpp
VlycQtPluginLoader.cpp
VlycPluginManager.cpp
VlycPluginManagerPrivate.cpp

VlycResult/Result.cpp
VlycResult/Playlist.cpp
)

add_definitions(-DLIBVLYC2_LIBRARY)

add_library(libvlyc2 SHARED ${HEADERS} ${SOURCES})
qt5_use_modules(libvlyc2 Core Network)
set_target_properties(libvlyc2 PROPERTIES OUTPUT_NAME vlyc2)
