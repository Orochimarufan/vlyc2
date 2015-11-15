/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2015 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include "bookmark.h"


BookmarkModel::BookmarkModel(const BookmarkList &bookmarks) :
    m_bookmarks(bookmarks)
{
    connect(this, &QAbstractItemModel::modelReset, this, &BookmarkModel::bookmarksChanged);
    connect(this, &QAbstractItemModel::dataChanged, this, &BookmarkModel::bookmarksChanged);
    connect(this, &QAbstractItemModel::rowsInserted, this, &BookmarkModel::bookmarksChanged);
    connect(this, &QAbstractItemModel::rowsMoved, this, &BookmarkModel::bookmarksChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &BookmarkModel::bookmarksChanged);
}

BookmarkModel::~BookmarkModel()
{
}

// Native API
bool BookmarkModel::replace(const BookmarkList &bookmarks)
{
    beginResetModel();
    m_bookmarks = bookmarks;
    endResetModel();
    return true;
}

bool BookmarkModel::add(const Bookmark &bookmark)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_bookmarks.append(bookmark);
    endInsertRows();
    return true;
}

bool BookmarkModel::insert(size_t where, const Bookmark &bookmark)
{
    beginInsertRows(QModelIndex(), where, where);
    m_bookmarks.insert(where, bookmark);
    endInsertRows();
    return true;
}

bool BookmarkModel::replace(size_t where, const Bookmark &bookmark)
{
    m_bookmarks.replace(where, bookmark);
    emit dataChanged(createIndex(where, 0), createIndex(where, 1));
    return true;
}

bool BookmarkModel::remove(size_t where)
{
    beginRemoveRows(QModelIndex(), where, where);
    m_bookmarks.removeAt(where);
    endRemoveRows();
    return true;
}

// Qt API
QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
        if (role == Qt::DisplayRole)
            switch (section)
            {
            case 0:
                return "Name";
            case 1:
                return "URL";
            }
    return QVariant();
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    const Bookmark &bm = m_bookmarks[index.row()];

    if (index.column() == 0)
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return bm.name;
        case Qt::DecorationRole:
            return bm.icon;
        case Qt::ToolTipRole:
            return bm.url.toString();
        case Qt::UserRole:
            return bm.url;
        }
    else
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
        case Qt::EditRole:
            return bm.url.toString();
        }

    return QVariant();
}

Qt::ItemFlags BookmarkModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    else
        return Qt::ItemIsDropEnabled;
}

bool BookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Bookmark &bm = m_bookmarks[index.row()];

    if (index.column() == 0)
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            bm.name = value.toString();
            emit dataChanged(index, index, {Qt::DisplayRole});
            return true;
        case Qt::DecorationRole:
            bm.icon = qvariant_cast<QIcon>(value);
            emit dataChanged(index, index, {Qt::DecorationRole});
            return true;
        case Qt::UserRole:
            bm.url = value.toUrl();
            emit dataChanged(index, createIndex(index.row(), 1), {Qt::DisplayRole, Qt::ToolTipRole});
            return true;
        }
    else
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            bm.url = value.toString();
            emit dataChanged(createIndex(index.row(), 0), index, {Qt::DisplayRole, Qt::ToolTipRole});
            return true;
        }

    return false;
}

bool BookmarkModel::insertRows(int row, int count, const QModelIndex &parent)
{
    emit beginInsertRows(parent, row, row + count - 1);
    for (int i=count; i; --i)
        m_bookmarks.insert(row, Bookmark());
    emit endInsertRows();
    return true;
}

bool BookmarkModel::removeRows(int row, int count, const QModelIndex &parent)
{
    emit beginRemoveRows(parent, row, row + count - 1);
    for (int i=count; i; --i)
        m_bookmarks.removeAt(row);
    emit endRemoveRows();
    return true;
}
