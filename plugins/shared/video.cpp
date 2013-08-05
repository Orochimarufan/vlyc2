/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
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

#include <video.h>


// VideoQuality
bool VideoQuality::operator <(const VideoQuality &o) const
{
    return q < o.q;
}

bool VideoQuality::operator >(const VideoQuality &o) const
{
    return q > o.q;
}

bool VideoQuality::operator ==(const VideoQuality &o) const
{
    return q == o.q;
}

bool VideoQuality::operator <=(const VideoQuality &o) const
{
    return q <= o.q;
}

bool VideoQuality::operator >=(const VideoQuality &o) const
{
    return q >= o.q;
}

// StandardVideo
bool StandardVideo::useFileMetadata() const
{
    return false;
}

QString StandardVideo::videoId() const
{
    return ms_video_id;
}

SitePlugin *StandardVideo::site() const
{
    return mp_site;
}

QString StandardVideo::title() const
{
    return ms_title;
}

QString StandardVideo::author() const
{
    return ms_author;
}

QString StandardVideo::description() const
{
    return ms_description;
}

int StandardVideo::views() const
{
    return mi_views;
}

int StandardVideo::likes() const
{
    return mi_likes;
}

int StandardVideo::dislikes() const
{
    return mi_dislikes;
}

int StandardVideo::favorites() const
{
    return mi_favorites;
}

QList<VideoQuality> StandardVideo::availableQualities() const
{
    return ml_availableQualities;
}

QStringList StandardVideo::availableSubtitleLanguages() const
{
    return ml_availableSubtitleLanguages;
}

void StandardVideo::getSubtitles(const QString &language)
{
    Q_UNUSED(language)
    emit subtitles(VideoSubtitles());
}

//done stuff

QString StandardVideo::getError() const
{
    return ms_error;
}

void StandardVideo::_error(const QString &m)
{
    ms_error = m;
}

//constructor
StandardVideo::StandardVideo(SitePlugin *site, const QString &video_id) :
    mp_site(site), ms_video_id(video_id),
    ms_title("Unknown Title"), ms_author("Unknown Author"),
    mi_views(0), mi_likes(0), mi_dislikes(0), mi_favorites(0)
{
    connect(this, SIGNAL(error(QString)), SLOT(_error(QString)));
}
