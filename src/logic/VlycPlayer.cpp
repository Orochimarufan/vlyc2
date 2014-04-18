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

#include <tuple>

#include "VlycPlayer.h"
#include "PlaylistNode.h"

using namespace Vlyc::Result;

VlycPlayer::VlycPlayer(VlycApp *app) :
    m_model(app), mp_current_node(nullptr)
{
    connect(&m_player, &VlcMediaPlayer::endReached, this, &VlycPlayer::next);
}

// Get data
PlaylistModel &VlycPlayer::model()
{
    return m_model;
}

VlcMediaPlayer VlycPlayer::player()
{
    return m_player;
}

// Modify playlist
void VlycPlayer::queueItem(ResultPtr result)
{
    m_model.queue(result);
}

void VlycPlayer::clearPlaylist()
{
    mp_current_node = nullptr;
    m_model.clear();
}

// Playback control
void VlycPlayer::play()
{
    if (!m_current_media.isValid())
        playFirstItem(m_model.root());
    else if (m_player.state() == VlcState::Paused)
        m_player.resume();
    else
        m_player.play();
    // Else do nothing FIXME?
}

void VlycPlayer::next()
{
    playItem(findNextItem(mp_current_node ? mp_current_node : m_model.root()));
}

void VlycPlayer::prev()
{
    playItem(findPrevItem(mp_current_node ? mp_current_node : m_model.root()));
}

// ----------------------------------------------------------------------------
// Manage Current item

// Legacy video crap :(
#include "__lv_hacks.h"

static std::tuple<QList<QString>, QList<int>> legacy_video_qualities(VideoPtr v)
{
    auto qa = v->availableQualities();

    qSort(qa.begin(), qa.end(), qGreater<VideoQuality>());

    QList<QString> qas;
    QList<int> qis;
    for (VideoQuality q : qa)
    {
        qas << q.description;
        qis << (int)q.q;
    }

    return std::make_tuple(qas, qis);
}

VlcMedia __lv_get_media::operator ()(VideoPtr v, int q)
{
    connect(v.get(), &Video::error, this, &TempEventLoop::stop);
    connect(v.get(), &Video::media, this, &__lv_get_media::media);

    v->getMedia((VideoQualityLevel)q);

    start();

    if (!url.isValid())
        return VlcMedia();

    VlcMedia media(url);

    if (v->useFileMetadata() && !media.isParsed())
        media.parse(false);
    else
    {
        media.setMeta(VlcMeta::Title, v->title());
        media.setMeta(VlcMeta::Artist, v->author());
        media.setMeta(VlcMeta::Description, v->description());
    }

    return media;
}

void __lv_get_media::media(const VideoMedia &m)
{
    url = m.url;
    stop();
}

// Set a new item
void VlycPlayer::setItem(PlaylistNode *item)
{
    // ONLY CALL ON LegacyVideoResult items!
    mp_current_node = item;

    auto qualities = legacy_video_qualities(item->__lvideo());
    ml_current_quality_list = std::get<0>(qualities);
    ml_current_quality_id_list = std::get<1>(qualities);
    m_current_quality_index = 0;

    emit qualityListChanged(ml_current_quality_list, m_current_quality_index);
    m_model.setCurrentlyPlaying(item);
}

void VlycPlayer::createMedia()
{
    m_current_media = __lv_get_media()(mp_current_node->__lvideo(), ml_current_quality_id_list[m_current_quality_index]);
}

void VlycPlayer::playMedia()
{
    m_player.setMedia(m_current_media);
    m_player.play();
}

// Set the quality level from gui
void VlycPlayer::setQuality(int index)
{
    m_current_quality_index = index;
    createMedia();

    float position = m_player.position();
    playMedia();
    m_player.setPosition(position);
}

// ----------------------------------------------------------------------------
// Traversal
PlaylistNode *VlycPlayer::findNextItem(PlaylistNode *origin)
{
    PlaylistNode::iterator it(origin);
    while (*(++it) != nullptr)
    {
        if (!it->isComplete())
            it->complete();
        if (it->isPlayable())
            break;
    };
    return *it;
}

PlaylistNode *VlycPlayer::findPrevItem(PlaylistNode *origin)
{
    PlaylistNode::iterator it(origin);
    while (*(--it) != nullptr)
    {
        if (!it->isComplete())
            it->complete();
        if (it->isPlayable())
            break;
    };
    return *it;
}

void VlycPlayer::playItem(PlaylistNode *item)
{
    if(item)
    {
        setItem(item);
        createMedia();
        playMedia();
    }
    else
    {
        m_player.stop();
        emit endReached();
        m_model.setCurrentlyPlaying(nullptr);
    }
}

void VlycPlayer::playFirstItem(PlaylistNode *origin)
{
    if (!origin->isComplete())
        origin->complete();
    if (origin->isPlayable())
        playItem(origin);
    else
        playItem(findNextItem(origin));
}

// ----------------------------------------------------------------------------
// UI Slots
void VlycPlayer::playNow(const QModelIndex &index)
{
    playFirstItem(m_model.getNodeFromIndex(index));
}
