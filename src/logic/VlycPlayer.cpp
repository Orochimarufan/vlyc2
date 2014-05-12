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

#include "../gui/mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>

#include "../vlyc.h"

using namespace Vlyc::Result;

VlycPlayer::VlycPlayer(VlycApp *app) :
    mp_app(app), m_model(app), m_player(), m_player_video(m_player), mp_current_node(nullptr)
{
    connect(&m_player, &VlcMediaPlayer::endReached, this, &VlycPlayer::next);
    connect(&m_model, &PlaylistModel::nodeAboutToBeDeleted, this, &VlycPlayer::onNodeAboutToBeDeleted);
    connect(&m_model, &PlaylistModel::nodeAdded, this, &VlycPlayer::onNodeAdded);
    connect(&m_promise, &PromiseListener::finished, this, &VlycPlayer::promiseFulfilled);
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
void VlycPlayer::queue(ResultPtr result)
{
    m_model.queue(result);
}

void VlycPlayer::queueAndPlay(ResultPtr result)
{
    playFirstItem(m_model.queue(result));
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

void VlycPlayer::play(PlaylistNode *node)
{
    playFirstItem(node);
}

void VlycPlayer::next()
{
    playNextItem(mp_current_node ? mp_current_node : m_model.root());
}

void VlycPlayer::prev()
{
    playPrevItem(mp_current_node ? mp_current_node : m_model.root());
}

// ----------------------------------------------------------------------------
// Manage Current item

// Legacy video crap :(
#include "__lv_hacks.h"

// Set a new item
void VlycPlayer::setItem(PlaylistNode *item)
{
    // ONLY CALL ON LegacyVideoResult items!
    mp_current_node = item;

    auto qualities = __lv_qualities(item->__lvideo());
    ml_current_quality_list = std::get<0>(qualities);
    ml_current_quality_id_list = std::get<1>(qualities);
    m_current_quality_index = 0;

    ml_current_subs_list = item->__lvideo()->availableSubtitleLanguages();
    ml_current_subs_list.prepend("No Subtitles");
    m_current_subs_index = 0;

    emit qualityListChanged(ml_current_quality_list, m_current_quality_index);
    emit subsListChanged(ml_current_subs_list, m_current_subs_index);
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

void VlycPlayer::setSubtitles(int index)
{
    m_current_subs_index = index;

    if (index == 0)
        m_player_video.setSpu(0);
    else
    {
        __lv_get_subs subs;
        subs(mp_current_node->__lvideo(), ml_current_subs_list[index]);

        if (subs.type.isNull())
            m_player_video.setSubtitleFile(subs.data.toString());
        else
        {
            static QString templat ("vlyc2-XXXXXX.%1");
            QTemporaryFile file(QDir::temp().absoluteFilePath(templat.arg(subs.type)));
            file.setAutoRemove(false);
            file.open();
            file.write(subs.data.toByteArray());
            file.close();

            m_player_video.setSubtitleFile(file.fileName());
        }
    }
}

// ----------------------------------------------------------------------------
// Traversal
void VlycPlayer::playNextItem(PlaylistNode *origin)
{
    PlaylistNode::iterator it(origin);
    while (*(++it) != nullptr)
    {
        if (!it->isComplete())
        {
            complete(*it, true);
            return;
        }
        if (it->isPlayable())
            break;
    }
    playItem(*it);
}

void VlycPlayer::playPrevItem(PlaylistNode *origin)
{
    PlaylistNode::iterator it(origin);
    while (*(--it) != nullptr)
    {
        if (!it->isComplete())
        {
            complete(*it, true, true);
            return;
        }
        if (it->isPlayable())
            break;
    };
    playItem(*it);
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

void VlycPlayer::playFirstItem(PlaylistNode *origin, bool reverse)
{
    if (!origin->isComplete())
    {
        complete(origin, true);
        return;
    }
    if (origin->isPlayable())
        playItem(origin);
    else if (reverse)
        playPrevItem(origin);
    else
        playNextItem(origin);
}

// ----------------------------------------------------------------------------
// UI Slots
void VlycPlayer::playNow(const QModelIndex &index)
{
    playFirstItem(m_model.getNodeFromIndex(index));
}

void VlycPlayer::remove(const QModelIndex &index)
{
    m_model.getNodeFromIndex(index)->remove();
}

void VlycPlayer::onNodeAboutToBeDeleted(PlaylistNode *node)
{
    if (node->contains(mp_current_node))
        // Skip over the whole tree!
        playNextItem(node->last());

}

// ----------------------------------------------------------------------------
// Complete
void VlycPlayer::complete(PlaylistNode *node, bool play, bool reverse)
{
    UrlPtr url = node->result().cast<Url>();
    if (url.isValid())
    {
        ResultPtr it = url.cast<Result>();
        while (it.is<Url>())
            it = mp_app->handleUrl(*it.cast<Url>());
        node->replaceWith(it);
        if (!node->isComplete())
            complete(node, play);
        else if (play)
            playFirstItem(node, reverse);
        return;
    }

    if (node->result().is<Promise>())
    {
        m_promise.schedule(node);
        if (play)
        {
            mp_promised_node = node;
            m_promised_reverse = reverse;
        }
        return;
    }

    qWarning("Cannot complete Result!");
}

void VlycPlayer::promiseFulfilled(PlaylistNode *node)
{
    if (!node->isComplete())
    {
        complete(node, node == mp_promised_node, m_promised_reverse);
        return;
    }

    if (node == mp_promised_node)
    {
        if (node->hasFailed())
            QMessageBox::critical(mp_app->window(), "Error", node->failReason());
        else
            playFirstItem(node, m_promised_reverse);
    }
}

void VlycPlayer::onNodeAdded(PlaylistNode *node)
{
    Q_UNUSED(node);
    // TODO: Move stuff into a different thread and we can pre-load stuff :)
    //if (!node->isComplete())
    //    complete(node);
}
