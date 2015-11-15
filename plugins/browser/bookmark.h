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

#pragma once

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QAbstractTableModel>

#include <QtGui/QIcon>

/**
 * @brief The Bookmark struct
 * represents a bookmark
 */
struct Bookmark
{
    QIcon icon;
    QString name;
    QUrl url;

    inline bool operator == (const Bookmark &other) const
    {
        return /*name == other.name &&*/ url == other.url;
    }
};

using BookmarkList = QList<Bookmark>;


/**
 * @brief The BookmarkModel class
 * An editable model for a list of bookmarks
 */
class BookmarkModel : public QAbstractTableModel
{
    Q_OBJECT

    BookmarkList m_bookmarks;

public:
    BookmarkModel(const BookmarkList &bookmarks = BookmarkList());

    ~BookmarkModel();

    // Direct API
    inline const BookmarkList &bookmarks() const
    {
        return m_bookmarks;
    }

    bool replace(const BookmarkList &bookmarks);

    bool add(const Bookmark &bookmark);
    bool insert(size_t where, const Bookmark &bookmark);
    bool replace(size_t where, const Bookmark &bookmark);
    bool remove(size_t where);

    size_t indexOf(const Bookmark &bookmark) const
    {
        return m_bookmarks.indexOf(bookmark);
    }

    inline size_t size() const
    {
        return m_bookmarks.size();
    }

    inline const Bookmark &operator[](size_t item) const
    {
        return m_bookmarks[item];
    }

    // Model API
    inline int rowCount(const QModelIndex &parent=QModelIndex()) const override
    {
        return m_bookmarks.size();
    }

    inline int columnCount(const QModelIndex &parent=QModelIndex()) const override
    {
        return 2;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;

    bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;

    // Drag-Reorder
    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;

    QMimeData *mimeData(const QModelIndexList &indices) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

Q_SIGNALS:
    void bookmarksChanged();
};
