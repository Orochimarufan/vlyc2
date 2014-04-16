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

#include "Playlist.h"

namespace Vlyc {
namespace Result {

qint64 Playlist::startAt()
{
    return 0;
}

// StandardPlaylist
StandardPlaylist::StandardPlaylist(qint64 startIndex) :
    m_start_index(startIndex)
{
}

ResultPtr StandardPlaylist::get(const qint64 &index)
{
    return QList::operator [](index);
}

qint64 StandardPlaylist::length()
{
    return QList::length();
}

qint64 StandardPlaylist::startAt()
{
    return m_start_index;
}

} // namespace Result
} // namespace Vlyc
