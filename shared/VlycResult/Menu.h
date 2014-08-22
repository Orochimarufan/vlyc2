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

#include <QVariant>

// Menu helpers
inline QVariant MenuEntry(const QString &method, const QString &text, const QString &popup=QString::null, const QVariantList &args=QVariantList())
{
    return QVariantMap{{"action", "call"}, {"method", method}, {"args", args}, {"text", text}, {"popup", popup}};
}

inline QVariant Menu(std::initializer_list<QVariant> entries)
{
    return QVariantList(entries);
}

inline QVariant Menu(std::initializer_list<std::pair<QString, QString>> entries)
{
    QVariantList r;
    for (auto entry : entries)
        r << MenuEntry(entry.first, entry.second);
    return r;
}
