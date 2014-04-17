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

#include <VlycResult/Result.h>

#include <video.h>

using namespace Vlyc::Result;

class VlycApp;

enum class ResultType : unsigned {
    Playlist,
    Video
};

class ResultModelNode
{
    VlycApp *mp_app;
    ResultModelNode *mp_parent;
    ResultPtr mp_result;
    ResultType m_type;

    ResultModelNode **map_children;

    ResultModelNode(VlycApp *app, ResultModelNode *parent, ResultPtr result);

public:
    ResultModelNode(VlycApp *app, ResultPtr result);
    ~ResultModelNode();

    // Children
    ResultModelNode *parent();
    qint64 length();
    ResultModelNode *at(const qint64 &index);
    ResultPtr resultAt(const qint64 &index);
    qint64 indexOf(ResultModelNode *child);

    // Data
    ResultType type();
    ResultPtr result();
    QString displayName();

    VideoPtr video();
};
