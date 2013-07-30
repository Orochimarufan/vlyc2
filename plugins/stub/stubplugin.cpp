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

#include "stubplugin.h"

#include <QtCore/QUrl>

struct StubVideo : StandardVideo
{
    StubVideo(StubPlugin* site, QString video_id)
    {
        mp_site = site;
        ms_video_id = video_id;
    }

    virtual Media media(VideoQualityLevel q)
    {
        return Media(this, VideoQuality(q, "lol"),
                     QUrl("file:///media/hinata/youtube/kogarashi/Project_Diva_Extend__Tsugai_Kogarashi_-_Meiko__Kaito.mkv"));
    }

    virtual void load()
    {
        ms_title = "Stub Video";
        ms_author = "Stub Author";
        ms_description = "A Stub Video";
        mi_likes = 0;
        mi_dislikes = 10000;
        mi_favorites = 0;

        VideoQuality url;
        url.q = (VideoQualityLevel)1;
        url.description = "MKV";
        ml_available.append(url);

        emit done();
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
