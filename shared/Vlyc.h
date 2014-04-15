/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
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

#ifndef VLYC_H
#define VLYC_H

#include <QtCore/QString>

namespace Vlyc {

// Compile time constants.
// XXX: They might vanish!
#define VLYC_VERSION "2.0.0 Yoona"
#define VLYC_HEXVERSION 0x20000
//#define VLYC_REVISION "@VLYC_EXPORTED@"

/**
 * @brief Retrieve vlyc version
 * @return a string containing the vlyc version
 */
QString version(void);

/**
 * @brief Retrieve vlyc changeset
 * @return a string containing the changeset
 */
//QString changeset(void);

/**
 * @brief Retrieve vlyc compiler version
 * @return a string containing the used compiler
 */
//QString compiler(void);

/**
 * @brief Retrieve vlyc integral version
 * @return an integer containing the vlyc version number
 * major minor patch, 2 bytes for each
 */
quint32 hexversion(void);

}

#endif // VLYC_H
