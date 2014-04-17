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

#include "ResultModel.h"

#include <VlycResult/Url.h>
#include "../vlyc.h"

#include <QtGui/QIcon>
// ----------------------------------------------------------------------------
// Construct
ResultModel::ResultModel(VlycApp *app) :
    mp_app(app), ml_root()
{
}

ResultModel::~ResultModel()
{
    clear();
}

// ----------------------------------------------------------------------------
// Modify
void ResultModel::queue(ResultPtr result)
{
    while (result.is<Vlyc::Result::Url>())
        result = mp_app->handleUrl(*result.cast<Vlyc::Result::Url>());
    beginInsertRows(QModelIndex(), ml_root.length(), ml_root.length());
    ResultModelNode *node = new ResultModelNode(mp_app, result);
    ml_root.append(node);
    endInsertRows();
}

void ResultModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, ml_root.length()-1);
    for (ResultModelNode *node : ml_root)
        delete node;
    endRemoveRows();
}

// ----------------------------------------------------------------------------
// Qt Model
bool ResultModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.internalPointer())
        return nodeFromIndex(parent)->type() == ResultType::Playlist;
    else
        return !ml_root.isEmpty();
}

QModelIndex ResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.internalPointer())
    {
        auto pnode = nodeFromIndex(parent);
        if (pnode->type() == ResultType::Playlist)
            return createIndex(row, column, pnode->at(row));
        else
            qWarning("Fork node not a Playlist");
    }
    else if (0 <= row && row < ml_root.length())
        return createIndex(row, column, ml_root[row]);
    return QModelIndex();
}

QModelIndex ResultModel::parent(const QModelIndex &child) const
{
    if (child.internalPointer())
        return indexFromNode(nodeFromIndex(child)->parent());
    else
        return QModelIndex();
}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    if (parent.internalPointer())
        return  nodeFromIndex(parent)->length();
    else
        return ml_root.length();
}

int ResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (index.internalPointer())
    {
        ResultModelNode *node = nodeFromIndex(index);
        switch(role) {
        case Qt::DisplayRole:
            return node->displayName();
        case Qt::DecorationRole:
            if (node->type() == ResultType::Playlist)
                return QIcon(":/type/playlist");
            else
                return QIcon(":/type/file");
        }
    }

    return QVariant();
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section) {
        case 0:
            return "Title";
        }
    return QVariant();
}

// ----------------------------------------------------------------------------
// Internal
ResultModelNode *ResultModel::nodeFromIndex(const QModelIndex &index) const
{
    return static_cast<ResultModelNode *>(index.internalPointer());
}

QModelIndex ResultModel::indexFromNode(ResultModelNode *node, int column) const
{
    if (!node)
        return QModelIndex();
    ResultModelNode *parent = node->parent();
    return createIndex(parent ? parent->indexOf(node) : 0, column, node);
}

QList<ResultModelNode *> const &ResultModel::root() const
{
    return ml_root;
}
