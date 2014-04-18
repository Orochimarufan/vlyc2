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

#include <QIcon>

#include <VlycResult/Playlist.h>

#include "PlaylistModel.h"
#include "PlaylistNode.h"

#include "../vlyc.h"

using namespace Vlyc::Result;

PlaylistModel::PlaylistModel(VlycApp *app) :
    mp_app(app), mp_root(new PlaylistNode(this))
{
}

PlaylistModel::~PlaylistModel()
{
    delete mp_root;
}

// ------------------------------------------------------------------------------
// Node callbacks
void PlaylistModel::beginInsertNode(PlaylistNode *parent, size_t index)
{
    qDebug("insert on %p: %i", parent, index);
    beginInsertRows(createIndexForNode(parent), (int)index, (int)index);
}

void PlaylistModel::beginInsertNode(PlaylistNode *parent, size_t first_index, size_t last_index)
{
    beginInsertRows(createIndexForNode(parent), (int)first_index, (int)last_index);
}

void PlaylistModel::endInsertNode()
{
    endInsertRows();
}

void PlaylistModel::beginMoveNode(PlaylistNode *node, PlaylistNode *new_parent, size_t new_index)
{
    int old_index = (int)node->index();
    beginMoveRows(createIndexForNode(node->parent()), old_index, old_index,
                  createIndexForNode(new_parent), (int)new_index);
}

void PlaylistModel::endMoveNode()
{
    endMoveRows();
}

void PlaylistModel::beginRemoveNode(PlaylistNode *node)
{
    int old_index = (int)node->index();
    beginRemoveRows(createIndexForNode(node->parent()), old_index, old_index);
}

void PlaylistModel::endRemoveNode()
{
    endRemoveRows();
}

ResultPtr PlaylistModel::completeUrl(UrlPtr url) const
{
    ResultPtr it = url.cast<Result>();
    while (it.is<Url>())
        it = mp_app->handleUrl(*it.cast<Url>());
    return it;
}

// ------------------------------------------------------------------------------
// Node juggling
PlaylistNode *PlaylistModel::getNodeFromIndex(const QModelIndex &index) const
{
    if (index.internalPointer())
        return static_cast<PlaylistNode *>(index.internalPointer());
    else
        return mp_root;
}

QModelIndex PlaylistModel::createIndexForNode(PlaylistNode *node, int column) const
{
    if (node && node != mp_root)
        return createIndex(node->index(), column, node);
    else
        return QModelIndex();
}

PlaylistNode *PlaylistModel::root() const
{
    return mp_root;
}

// ------------------------------------------------------------------------------
// QAbstractItemModel implementation
int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return getNodeFromIndex(parent)->size();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndexForNode(getNodeFromIndex(parent)->at(row), column);
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    return createIndexForNode(getNodeFromIndex(child)->parent());
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    PlaylistNode *node = getNodeFromIndex(index);
    switch(role)
    {
    case Qt::DecorationRole:
        if (!node->result().isValid() || node->result().is<Playlist>())
            return QIcon(":/type/playlist");
        else
            return QIcon(":/type/file");
    case Qt::DisplayRole:
        return node->displayName();
    case Qt::BackgroundColorRole:
        if (node == mp_current)
            return QColor(Qt::darkGreen);
        else
            return QVariant();
    }
    return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section) {
        case 0: return "Title";
        }
    return QVariant();
}

// ------------------------------------------------------------------------------
// Modify
void PlaylistModel::queue(ResultPtr result)
{
    mp_root->append(result);
}

void PlaylistModel::clear()
{
    beginResetModel();
    delete mp_root;
    mp_root = new PlaylistNode(this);
    endResetModel();
}

void PlaylistModel::setCurrentlyPlaying(PlaylistNode *node)
{
    static QVector<int> bgrole{Qt::BackgroundColorRole};

    PlaylistNode *old = mp_current;
    mp_current = node;

    if (node)
    {
        QModelIndex index = createIndexForNode(node);
        emit dataChanged(index, index, bgrole);
    }

    if (old)
    {
        QModelIndex index = createIndexForNode(old);
        emit dataChanged(index, index, bgrole);
    }
}
