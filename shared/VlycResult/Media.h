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

#include "Object.h"

/**
 * @file Media.h
 * Helper functions to create objects.
 */

namespace Vlyc {
namespace Result {

/*
 * Valid Object types:
 * [Media]
 * - file       = A file to play
 *
 * [Playlists]
 * - urllist    = A list of urls to play
 * - objectlist = A list of objects to play [used with the special ObjectList class!]
 */

inline auto File(const QUrl &mrl)
{
    return new Object{{"type", "file"}, {"mrl", mrl}};
}

inline auto File(const QUrl &mrl, const QStringList &options)
{
    return new Object{{"type", "file"}, {"mrl", mrl}, {"options", options}};
}

inline auto File(const QString &mrl)
{
    return new Object{{"type", "file"}, {"mrl", mrl}};
}

inline auto File(const QString &mrl, const QStringList &options)
{
    return new Object{{"type", "file"}, {"mrl", mrl}, {"options", options}};
}


inline auto UrlList(const QStringList &urls, const QString &name="Playlist")
{
    return new Object{{"type", "urllist"}, {"urls", urls}, {"name", name}};
}

} // namespace Result
} // namespace Vlyc
