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
VlycMem/Pointer.h
VlycMem/Object.h

VlycAsync/PromiseEvent.h

VlycPlugin.h
VlycToolPlugin.h
VlycUrlHandlerPlugin.h
VlycLoaderPlugin.h
VlycQtPluginLoader.h
VlycPluginManager.h
VlycPluginInterface.h

legacy/plugin.h
legacy/siteplugin.h
legacy/video.h
legacy/videoimpl.h
legacy/VlycLegacySitePlugin.h

VlycResult/Result.h
VlycResult/Url.h

VlycResult/Object.h
VlycResult/ObjectList.h
VlycResult/Media.h
VlycResult/Menu.h

VlycResult/Promise.h

VlycUtil/VariantArgs.h
)

set(PRIVATE_HEADERS
VlycPluginManagerPrivate.h
)

set(SOURCES
VlycMem/Object.cpp

VlycAsync/PromiseEvent.cpp

VlycPlugin.cpp
VlycLoaderPlugin.cpp
VlycQtPluginLoader.cpp
VlycPluginManager.cpp
VlycPluginManagerPrivate.cpp
VlycPluginInterface.cpp

legacy/video.cpp
legacy/VlycLegacySitePlugin.cpp

VlycResult/Object.cpp

VlycResult/Promise.cpp
)

add_definitions(-DLIBVLYC2_LIBRARY)

add_library(libvlyc2 SHARED ${HEADERS} ${SOURCES})
qt5_use_modules(libvlyc2 Core Network)
set_target_properties(libvlyc2 PROPERTIES OUTPUT_NAME vlyc2)
