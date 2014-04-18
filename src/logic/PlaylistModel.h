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

#include <QAbstractItemModel>

#include <VlycResult/Result.h>
#include <VlycResult/Url.h>

class PlaylistNode;
class VlycApp;

struct BrokenUrl : Vlyc::Result::Url
{
    BrokenUrl(const QUrl &url) : Url(url) {}
};

class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PlaylistModel(VlycApp *app);
    ~PlaylistModel();

    // QAbstractItemModel implementation
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Node juggling [violates const-ness!]
    PlaylistNode *getNodeFromIndex(const QModelIndex &index) const;
    QModelIndex createIndexForNode(PlaylistNode *node, int column = 0) const;
    PlaylistNode *root() const;

    // Modify it
    void queue(Vlyc::Result::ResultPtr result);
    void clear();

private:
    friend class PlaylistNode;

    // Node callbacks
    void beginInsertNode(PlaylistNode *parent, size_t index);
    void beginInsertNode(PlaylistNode *parent, size_t first_index, size_t last_index);
    void endInsertNode();

    void beginMoveNode(PlaylistNode *node, PlaylistNode *new_parent, size_t new_index);
    void endMoveNode();

    void beginRemoveNode(PlaylistNode *node);
    void endRemoveNode();

    Vlyc::Result::ResultPtr completeUrl(Vlyc::Result::UrlPtr url) const;

    // members
    VlycApp *mp_app;
    PlaylistNode *mp_root;
};
