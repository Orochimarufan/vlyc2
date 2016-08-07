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

# Include this file

######## Prepare ########
# Plugin stuff
STRING(SUBSTRING "${PLUGIN_NAME}" 0 1 _FIRST)
STRING(TOUPPER "${_FIRST}" _FIRST)
STRING(REGEX REPLACE "^.(.*)" "${_FIRST}\\1Plugin" PLUGIN_PROJECT "${PLUGIN_NAME}")
project(${PLUGIN_PROJECT})
SET(PLUGIN vlyc2-${PLUGIN_NAME})
SET(PLUGIN_QT Core Network)
SET(PLUGIN_LIBS libvlyc2)
SET(PLUGIN_SOURCES)
SET(PLUGIN_HEADERS)
SET(PLUGIN_FORMS)
SET(PLUGIN_RESOURCES)

