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

#include "__lv_hacks.h"

#include <QtCore/QHash>

// LegacyVideoResult
LegacyVideoResult::LegacyVideoResult(VideoPtr video) :
    mp_video(video)
{}

VideoPtr LegacyVideoResult::video()
{
    return mp_video;
}

// LegacyVideoPromise
LegacyVideoPromise::LegacyVideoPromise(VideoPtr video) :
    mp_video(video)
{}

VideoPtr LegacyVideoPromise::video()
{
    return mp_video;
}

void LegacyVideoPromise::work()
{
    connect(mp_video.get(), &Video::done, this, &LegacyVideoPromise::done);
    connect(mp_video.get(), &Video::error, this, &LegacyVideoPromise::error);
    mp_video->load();
}

void LegacyVideoPromise::done()
{
    Promise::fulfill(new LegacyVideoResult(mp_video));
}

void LegacyVideoPromise::error(QString message)
{
    Promise::error(message);
}

// Get stuff
std::tuple<QList<QString>, QList<int>> __lv_qualities(VideoPtr v)
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

    // Do some magic
    QStringList opts;
    QString url_str = url.toString();
    if (url_str.contains(" :"))
    {
        opts = url_str.split(" :");
        url_str = opts.takeFirst();
    }

    VlcMedia media(url_str);

    for (QString opt : opts)
        media.addOption(opt.prepend(":"));

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

void __lv_get_subs::operator ()(VideoPtr v, const QString &lang)
{
    connect(v.get(), &Video::error, this, &TempEventLoop::stop);
    connect(v.get(), &Video::subtitles, this, &__lv_get_subs::subtitles);

    v->getSubtitles(lang);

    start();
}

void __lv_get_subs::subtitles(const VideoSubtitles &m)
{
    type = m.type;
    data = m.data;
    stop();
}
