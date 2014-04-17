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

#include <QtCore/QAbstractItemModel>

#include <VlycResult/Result.h>

#include "ResultModelNode.h"

using namespace Vlyc::Result;

class VlycApp;

class ResultModel : public QAbstractItemModel
{
    Q_OBJECT

    VlycApp *mp_app;
    QList<ResultModelNode*> ml_root;

public:
    explicit ResultModel(VlycApp *app);
    ~ResultModel();

    // Modify
    void queue(ResultPtr result);
    void clear();

    // QAbstractItemModel implementation
    bool hasChildren(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Internal Data handling
    ResultModelNode *nodeFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromNode(ResultModelNode *node, int column=0) const;
    QList<ResultModelNode *> const &root() const;
};
