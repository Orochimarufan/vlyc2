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

#include <vector>

#include <QString>

#include <VlycResult/Result.h>
#include <vlyc2pointer.h>

class PlaylistModel;
class Video;
typedef Vlyc2Pointer<Video> VideoPtr;

class PlaylistNode
{
public:
    /**
     * @brief PlaylistNode constructor
     * Create the root playlist node.
     */
    PlaylistNode(PlaylistModel *);

    /**
     * @brief create a child node
     * @param result the VlycResult the new node contains
     * @return a new PlaylistNode
     */
    PlaylistNode *append(Vlyc::Result::ResultPtr result);
    PlaylistNode *insert(size_t index, Vlyc::Result::ResultPtr result);

    /**
     * @brief remove this node
     * this will DELETE this node and its children!
     */
    void remove();

    /**
     * @brief PlaylistNode destructor
     * Deletes all children!
     */
    ~PlaylistNode();

    // Structure
    /**
     * @brief model
     * @return the PlaylistModel this node belongs to
     */
    PlaylistModel *model() const;

    /**
     * @brief parent
     * @return this node's parent, or nullptr for the root node
     */
    PlaylistNode *parent() const;

    /**
     * @brief index
     * @return the index of this node within it's parent's children, or 0 for the root node
     */
    size_t index() const;

    /**
     * @brief size
     * @return the number of children this node has
     */
    size_t size() const;

    /**
     * @brief front
     * @return the first child
     */
    PlaylistNode *front() const;

    /**
     * @brief back
     * @return the last child
     */
    PlaylistNode *back() const;

    /**
     * @brief at
     * @param index
     * @return the child at \c index
     */
    PlaylistNode *at(size_t index) const;

    /**
     * @brief last
     * @return the last node in the subtree
     */
    PlaylistNode *last() const;

    /**
     * @brief contains
     * @param node
     * @return whether \c node is in this node's subtree
     */
    bool contains(PlaylistNode *node) const;

    // STL iterator interface
    /**
     * @brief The iterator class
     * A depth-first pre-order iterator.
     */
    class iterator
    {
    public:
        iterator(const iterator &other);
        iterator(PlaylistNode *item, bool end=false);

        bool operator ==(const iterator &other) const;
        bool operator ==(PlaylistNode *node) const;
        bool operator !=(const iterator &other) const;
        bool operator !=(PlaylistNode *node) const;

        const iterator &operator ++();
        const iterator &operator --();

        PlaylistNode *operator *() const;
        PlaylistNode *operator ->() const;

    private:
        PlaylistNode *mp_node;
        PlaylistNode *mp_last;
        bool m_last;
    };

    /**
     * @brief begin
     * @return an iterator to this node.
     */
    iterator begin();
    iterator rbegin();

    /**
     * @brief end
     * @return an iterator after the last child.
     */
    iterator end();
    iterator rend();

    // Data
    /**
     * @brief result
     * @return the raw VlycResult this node manages, or an invalid pointer
     * Note that this might change over the Node's livetime if the result is a Url reference
     */
    Vlyc::Result::ResultPtr result();

    /**
     * @brief isPlayable
     * @return true if this item can be played
     */
    bool isPlayable() const;

    /**
     * @brief displayName
     * @return the text to display in the GUI
     */
    QString displayName() const;

    /**
     * @brief __lvideo
     * @return the Video to play.
     * This is sure to be nuked sooner or later!!
     */
    VideoPtr __lvideo() const;

    // Completeness
    /**
     * @brief isComplete
     * @return wether this node is complete
     */
    bool isComplete() const;

    /**
     * @brief used to replace an incomplete with a complete result.
     * @param new_content the new result
     * Should only be called from PlaylistModel/PromiseListener
     */
    void replaceWith(Vlyc::Result::ResultPtr new_content);

private:
    PlaylistModel *mp_model;
    PlaylistNode *mp_parent;
    std::vector<PlaylistNode *> m_children;
    Vlyc::Result::ResultPtr mp_result;

    PlaylistNode(PlaylistNode *parent, Vlyc::Result::ResultPtr result);
    void initFromResult(bool from_constructor = false);
};
