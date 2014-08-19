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
#include <QtVlc/VlcMediaPlayerVideo.h>
#include <QtVlc/VlcMedia.h>

#include <VlycResult/Result.h>

#include "PlaylistModel.h"
#include "PromiseListener.h"

//FIXME: enum class and operator& ?
enum PlaybackFlags
{
    Normal = 0,
    RepeatAll = 1,
    RepeatOne = 2,
    Shuffle = 4,
};

class VlycPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VlycPlayer(VlycApp *app);

    PlaylistModel &model();
    VlcMediaPlayer player();

    PlaylistNode *current();

signals:
    void endReached();
    void qualityListChanged(QList<QString>, int current);
    void subsListChanged(QList<QString>, int current);

public slots:
    void queue(Vlyc::Result::ResultPtr result);
    void queueAndPlay(Vlyc::Result::ResultPtr result);

    void play(PlaylistNode *node);

    // UI slots
    void playNow(const QModelIndex &index);
    void setRepeatRoot(const QModelIndex &index);

    void remove(const QModelIndex &index);
    void clearPlaylist();

    void play();
    void setQuality(int index);
    void setSubtitles(int index);

    void setPlaybackFlags(const PlaybackFlags &flags);

    void next();
    void prev();

    // Complete Playlist Entries (Async)
    void complete(PlaylistNode *node, bool play=false, bool reverse=false);

private slots:
    void playItem(PlaylistNode *item);
    void playFirstItem(PlaylistNode *origin, bool reverse=false);

    void onNodeAboutToBeDeleted(PlaylistNode *node);
    void onNodeAdded(PlaylistNode *node);

    void promiseFulfilled(PlaylistNode *node);

    void onStateChanged(VlcState::Type state);

private:
    VlycApp *mp_app;
    PlaylistModel m_model;

    PlaybackFlags m_playback_flags;
    PlaylistNode *mp_repeat_root;

    VlcMediaPlayer m_player;
    VlcMediaPlayerVideo m_player_video;

    VlcMedia m_current_media;
    PlaylistNode *mp_current_node;
    quint32 m_current_item_type;

    PromiseListener m_promise;
    PlaylistNode *mp_promised_node;
    bool m_promised_reverse;

    // Old & broken stuff
    int m_last_quality_select;
    QList<QString> ml_current_quality_list;
    QList<int> ml_current_quality_id_list;
    int m_current_quality_index;
    QList<QString> ml_current_subs_list;
    int m_current_subs_index;

    void playNextItem(PlaylistNode *origin, PlaybackFlags flags=PlaybackFlags::Normal);
    void playPrevItem(PlaylistNode *origin);

    void createMedia();
    void setItem(PlaylistNode *item);
    void playMedia();

    void setSubtitleFile(QString &path);
    QString m_current_spu_file;
    void cleanupSpu();
};
