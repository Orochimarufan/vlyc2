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

#ifndef VLYCVIDEOPLUGIN_H
#define VLYCVIDEOPLUGIN_H

#include "VlycPlugin.h"

#include <QString>
#include <QVector>

namespace Vlyc {

struct VideoFormat
{
    unsigned level;
    QString name;
    QString resource;

    // Compare by level.
    bool operator <(const VideoFormat &other) const;
};

class Video
{
public:
    virtual QVector<VideoFormat> formats();
};

class VideoPlugin : public virtual Plugin
{
public:
    VideoPlugin();
};

}

#endif // VLYCVIDEOPLUGIN_H
