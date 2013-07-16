/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "stubplugin.h"

#include <QtVlc/VlcMedia.h>

#include <QtCore/QUrl>

struct StubVideo : StandardVideo
{
    StubVideo(StubPlugin* site, QString video_id)
    {
        mp_site = site;
        m_video_id = video_id;
    }

    virtual VlcMedia media(VideoQuality q)
    {
        return VlcMedia("file:///media/hinata/youtube/kogarashi/Project_Diva_Extend__Tsugai_Kogarashi_-_Meiko__Kaito.mkv");
    }

    virtual void load()
    {
        m_title = "Stub Video";
        m_author = "Stub Author";
        m_description = "A Stub Video";
        m_likes = 0;
        m_dislikes = 10000;
        m_favorites = 0;

        VideoQuality url;
        url.q = (VideoQualityLevel)1;
        url.description = "MKV";
        ml_available.append(url);

        emit this->loaded(this);
    }
};


QString StubPlugin::forUrl(QUrl url)
{
    if (url.toString() == "stub://")
        return "stub";
    return QString::null;
}

Video* StubPlugin::video(QString video_id)
{
    return new StubVideo(this, video_id);
}
