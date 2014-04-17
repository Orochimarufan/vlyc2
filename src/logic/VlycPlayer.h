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

#include <QtCore/QObject>
#include <QtVlc/VlcInstance.h>
#include <QtVlc/VlcMediaPlayer.h>
#include <QtVlc/VlcMedia.h>

#include <VlycResult/Result.h>

#include "ResultModel.h"

class VlycPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VlycPlayer(VlycApp *app);

    ResultModel &model();
    VlcMediaPlayer player();

signals:
    void endReached();
    void qualityListChanged(QList<QString>, int current);

public slots:
    void queueItem(ResultPtr result);
    void clearPlaylist();

    void play();
    void setQuality(int index);

    void next();
    void prev();

private slots:
    void setItem(ResultModelNode *item);
    void playItem();
    void playMedia();
    void playNextItem();

private:
    ResultModel m_model;
    VlcMediaPlayer m_player;

    VlcMedia m_current_media;
    ResultModelNode *mp_current_node;

    // Old & broken stuff
    QList<QString> ml_current_quality_list;
    QList<int> ml_current_quality_id_list;
    int m_current_quality_index;

    ResultModelNode *findNextItem();
    ResultModelNode *findPrevItem();

    void createMedia();
};
