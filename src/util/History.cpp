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

#include "History.h"

#define stringValue(X) value(X).toString()

History::History(const QString &prefix, int size) :
    prefix(prefix), size(size)
{
    settings.beginGroup(prefix);
}

QString History::lastFileOpenDir()
{
    return settings.stringValue("lastFileOpenDir");
}

void History::setFileOpenDir(const QString &dir)
{
    settings.setValue("lastFileOpenDir", dir);
}

static const QString file_fmt = QStringLiteral("File%1");

QStringList History::recentFiles()
{
    QStringList recent;
    for (int i=0; i<size; ++i)
    {
        QString key = file_fmt.arg(i);
        if (settings.contains(key))
            recent.append(settings.stringValue(key));
    }
    return recent;
}

void History::addRecentFile(const QString &path)
{
    for (int i=size-1; i; --i)
    {
        QString keyA = file_fmt.arg(i-1);
        QString keyB = file_fmt.arg(i);
        if (settings.contains(keyA))
            settings.setValue(keyB, settings.stringValue(keyA));
    }
    settings.setValue(file_fmt.arg(0), path);
}

void History::clearRecentFiles()
{
    for (int i=0; i<size; ++i)
        settings.remove(file_fmt.arg(i));
}
