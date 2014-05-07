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

#include <QObject>

#include <unordered_map>

#include <VlycResult/Promise.h>

class VlycApp;
class PlaylistNode;

class PromiseListener : public QObject
{
    Q_OBJECT
public:
    PromiseListener();

    void schedule(PlaylistNode *node);

    // This will NOT break the promise, it'll just allow for \c node to be deleted
    void del(PlaylistNode *node);

signals:
    void finished(PlaylistNode *node);

private:
    std::unordered_map<Vlyc::Result::PromisePtr, PlaylistNode *> mm_promises;

    bool event(QEvent *) override;
};
