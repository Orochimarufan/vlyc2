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

#include "ResultModelNode.h"

#include <VlycResult/Url.h>
#include <VlycResult/Playlist.h>
#include "../vlyc.h"

ResultModelNode::ResultModelNode(VlycApp *app, ResultModelNode *parent, ResultPtr result) :
    mp_app(app), mp_parent(parent), mp_result(result)
{
    PlaylistPtr playlist = result.cast<Playlist>();
    if (playlist.isValid())
    {
        m_type = ResultType::Playlist;
        map_children = new ResultModelNode*[playlist->length()];
        for (int i = 0; i < playlist->length(); ++i)
            map_children[i] = nullptr;
    }
    else if (result.is<LegacyVideoResult>())
        m_type = ResultType::Video;

    qDebug("Node: %p = %s", this, qPrintable(displayName()));
}

ResultModelNode::ResultModelNode(VlycApp *app, ResultPtr result) :
    ResultModelNode(app, nullptr, result)
{
}

ResultModelNode::~ResultModelNode()
{
    if (m_type == ResultType::Playlist)
    {
        for (int i = 0; i < mp_result.cast<Playlist>()->length(); ++i)
            delete map_children[i];
        delete map_children;
    }
}

// Children
ResultModelNode *ResultModelNode::parent()
{
    return mp_parent;
}

qint64 ResultModelNode::length()
{
    if (m_type == ResultType::Playlist)
        return mp_result.cast<Playlist>()->length();
    else
        return 0;
}

ResultPtr ResultModelNode::resultAt(const qint64 &index)
{
    if (m_type == ResultType::Playlist)
        return mp_result.cast<Playlist>()->get(index);
    else
        return nullptr;
}

ResultModelNode *ResultModelNode::at(const qint64 &index)
{
    if (m_type == ResultType::Playlist)
    {
        if (map_children[index] != nullptr)
            return map_children[index];
        ResultPtr result = mp_result.cast<Playlist>()->get(index);
        UrlPtr url = result.cast<Url>();
        while(url.isValid())
        {
            result = mp_app->handleUrl(*url);
            url = result.cast<Url>();
        }
        ResultModelNode *node = new ResultModelNode(mp_app, this, result);
        map_children[index] = node;
        return node;
    }
    return nullptr;
}

qint64 ResultModelNode::indexOf(ResultModelNode *child)
{
    // We actually only need to search through the already-created children,
    // because there can't be a ResultModelNode of the uninitialized ones.
    if (m_type == ResultType::Playlist)
        for (int i = 0; i < length(); ++i)
            if (map_children[i] == child)
                return i;
    return -1;
}

// Data
ResultType ResultModelNode::type()
{
    return m_type;
}

ResultPtr ResultModelNode::result()
{
    return mp_result;
}

QString ResultModelNode::displayName()
{
    switch(m_type) {
    case ResultType::Playlist:
        return mp_result.cast<Playlist>()->name();
    case ResultType::Video:
        return video()->title();
    default:
        return "";
    }
}

VideoPtr ResultModelNode::video()
{
    auto lvr = mp_result.cast<LegacyVideoResult>();
    if (lvr.isValid())
        return lvr->video();
    return nullptr;
}
