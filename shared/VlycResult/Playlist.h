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

#include <QtCore/QList>
#include <QtCore/QString>

#include "Result.h"

namespace Vlyc {
namespace Result {

class Playlist : public Result
{
public:
    virtual ResultPtr get(const qint64 &index) = 0;

    virtual qint64 length() = 0;

    /// The index that should play first
    virtual qint64 startAt();

    /// The playlist name
    virtual QString name();
};

class StandardPlaylist : public Playlist, public QList<ResultPtr>
{
    qint64 m_start_index;
    QString m_name;

public:
    StandardPlaylist(QString name="Unnamed Playlist", qint64 startIndex=0);

    virtual ResultPtr get(const qint64 &);
    virtual qint64 length();
    virtual qint64 startAt();
    virtual QString name();
};


typedef ResultPointer<Playlist> PlaylistPtr;

} // namespace Result
} // namespace Vlyc
