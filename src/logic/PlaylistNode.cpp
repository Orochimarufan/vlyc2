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

#include <VlycResult/Playlist.h>
#include <VlycResult/Url.h>

#include "PlaylistNode.h"
#include "PlaylistModel.h"

#include "../vlyc.h"

using namespace Vlyc::Result;

PlaylistNode::PlaylistNode(PlaylistModel *model) :
    mp_model(model), mp_parent(nullptr)
{
}

PlaylistNode::PlaylistNode(PlaylistNode *parent, ResultPtr result) :
    mp_model(parent->model()), mp_parent(parent), mp_result(result)
{
    qDebug("Node: %p -> %p = %s", parent, this, qPrintable(displayName()));
    if (result.isValid())
        initFromResult(true);
}

void PlaylistNode::initFromResult(bool from_constructor)
{
    PlaylistPtr playlist = mp_result.cast<Playlist>();
    if (playlist.isValid())
    {
        if (!from_constructor) mp_model->beginInsertNode(this, 0, playlist->length() - 1);
        m_children.reserve(playlist->length());
        for (int i = 0; i < playlist->length(); ++i)
            m_children.push_back(new PlaylistNode(this, playlist->get(i)));
        if (!from_constructor) mp_model->endInsertNode();
    }
}

PlaylistNode::~PlaylistNode()
{
    for (PlaylistNode *child : m_children)
        delete child;
}

PlaylistNode *PlaylistNode::append(ResultPtr result)
{
    PlaylistNode *node = new PlaylistNode(this, result);
    mp_model->beginInsertNode(this, m_children.size());
    m_children.push_back(node);
    mp_model->endInsertNode();
    return node;
}

PlaylistNode *PlaylistNode::insert(size_t index, ResultPtr result)
{
    PlaylistNode *node = new PlaylistNode(this, result);
    mp_model->beginInsertNode(this, index);
    m_children.insert(m_children.begin() + index, node);
    mp_model->endInsertNode();
    return node;
}

// ------------------------------------------------------------------------------
// Structure

PlaylistModel *PlaylistNode::model() const
{
    return mp_model;
}

PlaylistNode *PlaylistNode::parent() const
{
    return mp_parent;
}

size_t PlaylistNode::index() const
{
    if (!mp_parent)
        return 0;
    else
        for (size_t i = 0; i < mp_parent->m_children.size(); ++i)
            if (mp_parent->m_children[i] == this)
                return i;
    // Undefined behavior!
    return 0;
}

size_t PlaylistNode::size() const
{
    return m_children.size();
}

PlaylistNode *PlaylistNode::front() const
{
    return m_children.front();
}

PlaylistNode *PlaylistNode::back() const
{
    return m_children.back();
}

PlaylistNode *PlaylistNode::at(size_t index) const
{
    return m_children.at(index);
}

PlaylistNode *PlaylistNode::last() const
{
    PlaylistNode *node = const_cast<PlaylistNode*>(this);
    while (!node->m_children.empty())
        node = node->m_children.back();
    return node;
}

// STL iterator
PlaylistNode::iterator::iterator(const iterator &other) :
    mp_node(other.mp_node), mp_last(other.mp_last), m_last(other.m_last)
{
}

PlaylistNode::iterator::iterator(PlaylistNode *item, bool end) :
    mp_node(item), mp_last(nullptr), m_last(end)
{
}

bool PlaylistNode::iterator::operator ==(const PlaylistNode::iterator &other) const
{
    return !operator !=(other);
}

bool PlaylistNode::iterator::operator ==(PlaylistNode *node) const
{
    return mp_node == node;
}

bool PlaylistNode::iterator::operator !=(const PlaylistNode::iterator &other) const
{
    if (m_last)
        return mp_node != other.mp_last;
    else if (other.m_last)
        return mp_last != other.mp_node;
    else
        return mp_node != other.mp_node;
}

bool PlaylistNode::iterator::operator !=(PlaylistNode *node) const
{
    return mp_node != node;
}

const PlaylistNode::iterator &PlaylistNode::iterator::operator ++()
{
    mp_last = mp_node;

    if (!mp_node->m_children.empty())
        mp_node = mp_node->m_children.front();
    else if (mp_node->mp_parent)
    {
        size_t index = mp_node->index();
        while (mp_node->mp_parent->m_children.size() <= ++index)
        {
            mp_node = mp_node->mp_parent;
            if (!mp_node->mp_parent)
            {
                mp_node = nullptr;
                return *this;
            }
            index = mp_node->index();
        }
        mp_node = mp_node->mp_parent->m_children.at(index);
    }
    else
        mp_node = nullptr;
    return *this;
}

const PlaylistNode::iterator &PlaylistNode::iterator::operator --()
{
    mp_last = mp_node;

    // Travel UP
    while (mp_node && mp_node->index() == 0)
        mp_node = mp_node->parent();
    if (!mp_node)
        return *this;

    // Travel 1 backward
    mp_node = mp_node->parent()->at(mp_node->index() - 1);

    return *this;
}

PlaylistNode *PlaylistNode::iterator::operator *() const
{
    return mp_node;
}

PlaylistNode *PlaylistNode::iterator::operator ->() const
{
    return mp_node;
}

PlaylistNode::iterator PlaylistNode::begin()
{
    return iterator(this);
}

PlaylistNode::iterator PlaylistNode::rbegin()
{
    return iterator(last());
}

PlaylistNode::iterator PlaylistNode::end()
{
    return iterator(last(), true);
}

PlaylistNode::iterator PlaylistNode::rend()
{
    return iterator(this, true);
}

// ------------------------------------------------------------------------------
// Data
ResultPtr PlaylistNode::result()
{
    return mp_result;
}

bool PlaylistNode::isComplete() const
{
    return !mp_result.is<Url>() || mp_result.is<BrokenUrl>();
}

void PlaylistNode::complete()
{
    UrlPtr url = mp_result.cast<Url>();
    if (url.isValid())
    {
        mp_result = mp_model->completeUrl(url);
        if (mp_result.isValid())
            initFromResult();
        else
            mp_result = new BrokenUrl(*url);
    }
}

bool PlaylistNode::isPlayable() const
{
    return mp_result.is<LegacyVideoResult>();
}

QString PlaylistNode::displayName() const
{
    PlaylistPtr playlist = mp_result.cast<Playlist>();
    if (playlist.isValid())
        return playlist->name();
    auto brokenUrl = mp_result.cast<BrokenUrl>();
    if (brokenUrl.isValid())
        return "Unknown: " + brokenUrl->toString();
    UrlPtr url = mp_result.cast<Url>();
    if (url.isValid())
        return url->toString();
    auto video = mp_result.cast<LegacyVideoResult>();
    if (video.isValid())
        return video->video()->title();
    return QString();
}

VideoPtr PlaylistNode::__lvideo() const
{
    auto video = mp_result.cast<LegacyVideoResult>();
    if (video.isValid())
        return video->video();
    return nullptr;
}
