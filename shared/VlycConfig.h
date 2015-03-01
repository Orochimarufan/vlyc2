/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #include <QtCore/QtGlobal>
  #ifdef LIBVLYC2_LIBRARY
    #define VLYC_EXPORT Q_DECL_EXPORT
  #else
    #define VLYC_EXPORT Q_DECL_IMPORT
  #endif
  #define VLYC_HIDDEN
#elif __GNUC__ >= 4
  #define VLYC_EXPORT __attribute__ ((visibility ("default")))
  #define VLYC_HIDDEN  __attribute__ ((visibility ("hidden")))
#else
  #define VLYC_EXPORT
  #define VLYC_HIDDEN
#endif
