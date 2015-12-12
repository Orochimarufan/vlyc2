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

#include <VlycResult/Url.h>
#include <VlycResult/Object.h>
#include <VlycResult/ObjectList.h>

#include "PlaylistNode.h"
#include "PlaylistModel.h"

#include "__lv_hacks.h"

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
    ObjectPtr o = mp_result.cast<Vlyc::Result::Object>();
    if (o.isValid())
    {
        qDebug("Object: %s", qPrintable(o->objectName()));

        auto pl = o.cast<ObjectList>();
        if (pl.isValid())
        {
            if (!from_constructor) mp_model->beginInsertNode(this, 0, pl->length() - 1);
            m_children.reserve(pl->length());
            for (ResultPtr res : *pl)
                m_children.push_back(new PlaylistNode(this, res));
            if (!from_constructor) mp_model->endInsertNode();
        }

        else if (o->type() == "objectlist")
        {
            QVariantList list = o->property<QVariantList>("children");
            if (!from_constructor) mp_model->beginInsertNode(this, 0, list.length() - 1);
            m_children.reserve(list.length());
            for (QVariant child : list)
                m_children.push_back(new PlaylistNode(this, child.value<ResultPtr>()));
            if (!from_constructor) mp_model->endInsertNode();
        }

        else if (o->type() == "urllist")
        {
            QVariantList urls = o->property<QVariantList>("urls");
            if (!from_constructor) mp_model->beginInsertNode(this, 0, urls.length() - 1);
            m_children.reserve(urls.length());
            for (QVariant url : urls)
                m_children.push_back(new PlaylistNode(this, new Vlyc::Result::Url(url.toUrl())));
            if (!from_constructor) mp_model->endInsertNode();
        }

        o->setProperty("__vlyc_playlist_node", QVariant::fromValue((void*)this));
    }
    else
    {
        auto lv = mp_result.cast<LegacyVideoResult>();
        if (lv.isValid())
        {
            QStringList urls = lv->video()->getChildrenUrls();
            if (!urls.empty())
            {
                if (!from_constructor) mp_model->beginInsertNode(this, 0, urls.length() - 1);
                m_children.reserve(urls.length());
                for (const QString &url : urls)
                    m_children.push_back(new PlaylistNode (this, new Vlyc::Result::Url(QUrl(url))));
                if (!from_constructor) mp_model->endInsertNode();
            }
        }
    }

    mp_model->nodeWasCreated(this);
}

PlaylistNode::~PlaylistNode()
{
    for (PlaylistNode *child : m_children)
        delete child;
}

PlaylistNode *PlaylistNode::append(ResultPtr result)
{
    mp_model->beginInsertNode(this, m_children.size());
    PlaylistNode *node = new PlaylistNode(this, result);
    m_children.push_back(node);
    mp_model->endInsertNode();
    return node;
}

PlaylistNode *PlaylistNode::insert(size_t index, ResultPtr result)
{
    mp_model->beginInsertNode(this, index);
    PlaylistNode *node = new PlaylistNode(this, result);
    m_children.insert(m_children.begin() + index, node);
    mp_model->endInsertNode();
    return node;
}

void PlaylistNode::remove()
{
    mp_model->beginRemoveNode(this);
    auto it = mp_parent->m_children.begin();
    while (*it != this) ++it;
    mp_parent->m_children.erase(it);
    mp_model->endRemoveNode();
    delete this;
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

bool PlaylistNode::contains(PlaylistNode *node) const
{
    for (PlaylistNode *n : *const_cast<PlaylistNode*>(this))
        if (n == node)
            return true;
    return false;
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

PlaylistNode::iterator &PlaylistNode::iterator::operator+=(unsigned int n)
{
    for (; n && **this; --n)
        ++*this;
    return *this;
}

PlaylistNode::iterator &PlaylistNode::iterator::operator-=(unsigned int n)
{
    for (; n && **this; --n)
        --*this;
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

QVariant PlaylistNode::property(const QString &name)
{
    ObjectPtr media = mp_result.cast<Object>();
    if (media.isValid())
        return media->getProperty(name);
    return QVariant();
}

bool PlaylistNode::hasProperty(const QString &name)
{
    ObjectPtr o = mp_result.cast<Object>();
    return o.isValid() && o->hasProperty(name);
}

bool PlaylistNode::hasMethod(const QString &name)
{
    ObjectPtr o = mp_result.cast<Object>();
    return o.isValid() && o->hasMethod(name);
}

QVariant PlaylistNode::call(const QString &method, const QVariantList &args)
{
    ObjectPtr o = mp_result.cast<Object>();
    if (o.isValid())
        return o->call(method, args);
    return QVariant();
}

bool PlaylistNode::isPlayable() const
{
    auto lv = mp_result.cast<LegacyVideoResult>();
    if (lv.isValid())
        return lv->video()->availableQualities().size() >= 1;

    auto o = mp_result.cast<Object>();
    return o.isValid() && o->type() == "file";
}

QString PlaylistNode::displayName() const
{
    auto brokenUrl = mp_result.cast<BrokenUrl>();
    if (brokenUrl.isValid())
        return "Unknown: " + brokenUrl->toString();

    auto url = mp_result.cast<Url>();
    if (url.isValid())
        return url->toString();

    auto video = mp_result.cast<LegacyVideoResult>();
    if (video.isValid())
        return video->video()->title();

    auto o = mp_result.cast<Object>();
    if (o.isValid())
    {
        if (o->hasProperty("name"))
            return o->property<QString>("name");
        else if (o->type() == "file")
            return o->property<QUrl>("mrl").fileName();
        else
            return o->objectName();
    }

    return QString();
}

VideoPtr PlaylistNode::__lvideo() const
{
    auto video = mp_result.cast<LegacyVideoResult>();
    if (video.isValid())
        return video->video();
    return nullptr;
}

// Completeness
bool PlaylistNode::isComplete() const
{
    auto o = mp_result.cast<Object>();
    if (o.isValid() && m_error.isNull())
        return (o->type() == "file" || o->type() == "urllist" || o->type() == "objectlist");

    return !m_error.isNull() ||
         (!mp_result.is<Promise>() &&
         (!mp_result.is<Url>() || mp_result.is<BrokenUrl>()));
}

void PlaylistNode::replaceWith(ResultPtr new_content)
{
    if (new_content.isValid())
    {
        mp_result = new_content;
        initFromResult();
    }
}

void PlaylistNode::markFailed(const QString &reason)
{
    if (reason.isNull())
        m_error = "[FAILED]" + displayName();
    else
        m_error = reason;
}

bool PlaylistNode::hasFailed() const
{
    return !m_error.isNull();
}

QString PlaylistNode::failReason() const
{
    return m_error;
}

bool PlaylistNode::isList() const
{
    auto lv = mp_result.cast<LegacyVideoResult>();
    if (lv.isValid())
        return !lv->video()->getChildrenUrls().empty();

    auto o = mp_result.cast<Object>();
    return o.isValid() && (o->type() == "objectlist" || o->type() == "urllist");
}
