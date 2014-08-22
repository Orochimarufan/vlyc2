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
    mp_app(app), m_model(app),
    m_playback_flags(PlaybackFlags::Normal), mp_repeat_root(m_model.root()),
    m_player(), m_player_video(m_player),
    mp_current_node(nullptr), m_last_quality_select((int)VideoQualityLevel::QA_INVALID)
{
    connect(&m_player, &VlcMediaPlayer::endReached, this, &VlycPlayer::next);
    connect(&m_player, &VlcMediaPlayer::stateChanged, this, &VlycPlayer::onStateChanged);
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

PlaylistNode *VlycPlayer::current()
{
    return mp_current_node;
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
    playNextItem(mp_current_node ? mp_current_node : m_model.root(), m_playback_flags);
}

void VlycPlayer::prev()
{
    playPrevItem(mp_current_node ? mp_current_node : m_model.root());
}

// ----------------------------------------------------------------------------
// Manage Current item

// Legacy video crap :(
#include "__lv_hacks.h"

// TODO: move into PlaylistNode
#include <VlycResult/Object.h>

static const quint32 T_BROKEN = 0;
static const quint32 T_LEGACY = 1;
static const quint32 T_DIRECT = 2;

// Set a new item
void VlycPlayer::setItem(PlaylistNode *item)
{
    m_current_item_type = T_BROKEN;

    if (item->result().is<LegacyVideoResult>())
    {
        m_current_item_type = T_LEGACY;

        auto qualities = __lv_qualities(item->__lvideo());
        ml_current_quality_list = std::get<0>(qualities);
        ml_current_quality_id_list = std::get<1>(qualities);

        // find best quality [DIRTY HACK!!!!]
        if (m_last_quality_select != (int)VideoQualityLevel::QA_INVALID)
            for (int i = ml_current_quality_id_list.length() - 1; i >= 0 && ml_current_quality_id_list[i] < (m_last_quality_select + 15); --i)
                m_current_quality_index = i;

        ml_current_subs_list = item->__lvideo()->availableSubtitleLanguages();
        ml_current_subs_list.prepend("No Subtitles");
    }

    auto o = item->result().cast<Object>();
    if (o.isValid())
    {
        QString type = o->type();
        if (type == "file")
        {
            m_current_item_type = T_DIRECT;

            ml_current_quality_list.clear();
            ml_current_quality_list << "File";

            ml_current_subs_list.clear();
        }
    }

    if (m_current_item_type == T_BROKEN)
    {
        QMessageBox::critical(0, "Error:  This shouldn't happen!", "Couldn't handle playable playlist node");
    }
    else
    {
        cleanupSpu();
        mp_current_node = item;

        m_current_quality_index = 0;
        m_current_subs_index = 0;

        emit qualityListChanged(ml_current_quality_list, m_current_quality_index);
        emit subsListChanged(ml_current_subs_list, m_current_subs_index);
        m_model.setCurrentlyPlaying(item);
    }
}

void VlycPlayer::createMedia()
{
    if (m_current_item_type == T_LEGACY)
        m_current_media = __lv_get_media()(mp_current_node->__lvideo(), ml_current_quality_id_list[m_current_quality_index]);
    else if (m_current_item_type == T_DIRECT)
    {
        m_current_media = VlcMedia(mp_current_node->property2<QUrl>("mrl"));
        if (mp_current_node->hasProperty("options"))
            for (auto opt : mp_current_node->property2<QStringList>("options"))
            {
                qDebug("option: %s", qPrintable(opt));
                m_current_media.addOption(opt);
            }
        if (!m_current_media.isParsed()) m_current_media.parse();
    }
}

void VlycPlayer::playMedia()
{
    m_player.setMedia(m_current_media);
    m_player.play();
}

void VlycPlayer::onStateChanged(VlcState::Type state)
{
    if (state != VlcState::Playing)
        return;
    if (m_current_item_type == T_DIRECT)
    {
        // Add file subtitles
        QHash<int, QString> spu = m_player_video.spuDescription();
        QList<int> spu_ids = spu.keys();
        qSort(spu_ids);
        ml_current_subs_list.clear();
        ml_current_quality_id_list.clear(); // HACK!
        for (int i : spu_ids)
        {
            qDebug("SPU %i: %s", i, qPrintable(spu[i]));
            ml_current_subs_list << spu[i];
            ml_current_quality_id_list << i;
        }
        m_current_subs_index = ml_current_quality_id_list.indexOf(m_player_video.spu());
        emit subsListChanged(ml_current_subs_list, m_current_subs_index);
    }
}

// Set the quality level from gui
void VlycPlayer::setQuality(int index)
{
    m_current_quality_index = index;
    createMedia();

    float position = m_player.position();
    playMedia();
    m_player.setPosition(position);

    if (!m_current_spu_file.isEmpty())
        m_player_video.setSubtitleFile(m_current_spu_file);

    m_last_quality_select = ml_current_quality_id_list[index];
}

void VlycPlayer::setSubtitles(int index)
{
    m_current_subs_index = index;

    if (index == 0)
        m_player_video.setSpu(-1);
    else if (m_current_item_type == T_DIRECT)
        m_player_video.setSpu(ml_current_quality_id_list[index]);
    else if (m_current_item_type == T_LEGACY)
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

void VlycPlayer::setSubtitleFile(QString &path)
{
    cleanupSpu();

    m_current_spu_file = path;
    m_player_video.setSubtitleFile(path);
}

void VlycPlayer::cleanupSpu()
{
    if (!m_current_spu_file.isEmpty())
    {
        QFile::remove(m_current_spu_file);
        m_current_spu_file.clear();
    }
}

// ----------------------------------------------------------------------------
// Traversal
void VlycPlayer::setPlaybackFlags(const PlaybackFlags &flags)
{
    m_playback_flags = flags;
}

void VlycPlayer::playNextItem(PlaylistNode *origin, PlaybackFlags flags)
{
    if (flags & PlaybackFlags::RepeatOne)
    {
        if (!origin->isComplete())
        {
            complete(origin, true);
            return;
        }
        if (origin->isPlayable())
        {
            playItem(origin);
            return;
        }
    }

    // Normal
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

    // Repeat All
    if (*it == nullptr && flags & PlaybackFlags::RepeatAll)
        playFirstItem(mp_repeat_root);
    else
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
    else if (origin->property("start_index").isValid())
    {
        PlaylistNode::iterator it(origin);
        it += origin->property2<int>("start_index");
        if (!*it)
            playNextItem(origin);
        else
        {
            playNextItem(*it);
            if (origin->property("start_time").isValid())
                m_player.setTime(origin->property("start_time").toLongLong());
        }
    }
    else
        playNextItem(origin);
}

// ----------------------------------------------------------------------------
// UI Slots
void VlycPlayer::playNow(const QModelIndex &index)
{
    playFirstItem(m_model.getNodeFromIndex(index));
}

void VlycPlayer::setRepeatRoot(const QModelIndex &index)
{
    mp_repeat_root = m_model.getNodeFromIndex(index);
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
        while (it.is<Url>() && !it.is<BrokenUrl>())
            it = mp_app->handleUrl(*it.cast<Url>());
        if (!it.isValid())
        {
            node->markFailed("BAM");
            if (play)
                playNextItem(node);
            return;
        }
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
