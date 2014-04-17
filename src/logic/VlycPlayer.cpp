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

#include "VlycPlayer.h"

#include "TempEventLoop.h"

#include <tuple>

VlycPlayer::VlycPlayer(VlycApp *app) :
    mp_current_node(nullptr), m_model(app)
{
    connect(&m_player, &VlcMediaPlayer::endReached, this, &VlycPlayer::playNextItem);
}

// Get data
ResultModel &VlycPlayer::model()
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
        playNextItem();
    else if (m_player.state() == VlcState::Paused)
        m_player.resume();
    else
        m_player.play();
    // Else do nothing FIXME?
}

void VlycPlayer::next()
{
    playNextItem();
}

void VlycPlayer::prev()
{
    return; // FIXME: implement
    setItem(findPrevItem());
    playItem();
}

// ----------------------------------------------------------------------------
// Manage Current item

// Legacy video crap :(
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

static VlcMedia legacy_video_get_media(VideoPtr v, VideoQualityLevel q)
{
    TempEventLoop loop;
    VlcMedia media;

    v->connect(&v, &Video::media, [&](const VideoMedia &vmedia) {
        loop.stop();
        media = VlcMedia(vmedia.url);
    });
    v->connect(&v, &Video::error, &loop, &TempEventLoop::stop);

    v->getMedia(q);

    loop.start();

    if (!media.isValid())
        return media;

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

// Set a new item
void VlycPlayer::setItem(ResultModelNode *item)
{
    // ONLY CALL ON LegacyVideoResult items!
    mp_current_node = item;

    auto qualities = legacy_video_qualities(item->video());
    ml_current_quality_list = std::get<0>(qualities);
    ml_current_quality_id_list = std::get<1>(qualities);
    m_current_quality_index = 0;

    emit qualityListChanged(ml_current_quality_list, m_current_quality_index);
}

void VlycPlayer::createMedia()
{
    m_current_media = legacy_video_get_media(mp_current_node->video(), (VideoQualityLevel)ml_current_quality_id_list[m_current_quality_index]);
}

void VlycPlayer::playItem()
{
    createMedia();
    playMedia();
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
ResultModelNode *VlycPlayer::findNextItem()
{
    // FIXME: nuke this piece of crap
    ResultModelNode *node = mp_current_node;
    ResultModelNode *parent;
    int index, length;
    bool up;

    if (!node)
    {
        if (m_model.root().length() < 1)
            return nullptr;

        node = m_model.root().at(0);
        if (node->type() == ResultType::Video)
            return node;
        index = 0;
        length = m_model.root().length();
    }

    while (node != nullptr)
    {
        if (node->length() > 0 && !up)
        {
            // Deeper
            node = node->at(0);
            index = 0;
            length = node->length();
        }
        else
        {
            up = false;
            parent = node->parent();

            if (!parent)
            {
                index = m_model.root().indexOf(node);
                length = m_model.root().length();
            }
            else
            {
                index = parent->indexOf(node);
                length = parent->length();
            }

            if (length > ++index)
                // Sibling
            {
                if (parent)
                    node = parent->at(index);
                else
                    node = m_model.root().at(index);
            }
            else if (parent)
            {
                // Upwards
                node = parent;
                up = true;
                continue;
            }
            else
                return nullptr;
        }

        if (node->type() == ResultType::Video)
            return node;
    }
    return nullptr;
}

ResultModelNode *VlycPlayer::findPrevItem()
{
    return nullptr;
}

void VlycPlayer::playNextItem()
{
    ResultModelNode *next = findNextItem();
    if(!next)
        return;
    setItem(next);
    playItem();
}
