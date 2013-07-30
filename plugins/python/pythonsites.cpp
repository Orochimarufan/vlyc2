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

#include "pythonplugin.h"
#include "pythonsites.h"

#include <QtCore/QUrl>

// PythonVideoWrapper
PythonVideoWrapper::PythonVideoWrapper(PythonVideo *v) : mp_vid(v)
{}

PythonVideoWrapper::PythonVideoWrapper() : mp_vid(NULL)
{}

PythonVideoWrapper::PythonVideoWrapper(const PythonVideoWrapper &w) : mp_vid(w.mp_vid)
{}

PythonVideoWrapper::~PythonVideoWrapper()
{}

QString PythonVideoWrapper::videoId() const
{
    return mp_vid->ms_video_id;
}

void PythonVideoWrapper::setTitle(const QString &title)
{
    mp_vid->ms_title = title;
}

void PythonVideoWrapper::setAuthor(const QString &author)
{
    mp_vid->ms_author = author;
}

void PythonVideoWrapper::setDescription(const QString &description)
{
    mp_vid->ms_description = description;
}

void PythonVideoWrapper::setLikes(const int &likes)
{
    mp_vid->mi_likes = likes;
}

void PythonVideoWrapper::setDislikes(const int &dislikes)
{
    mp_vid->mi_dislikes = dislikes;
}

void PythonVideoWrapper::setFavorites(const int &favorites)
{
    mp_vid->mi_favorites = favorites;
}

void PythonVideoWrapper::addQuality(const int &level, const QString &descr, const QString &url)
{
    qDebug("AddQ: %i %s", level, qPrintable(descr));
    VideoQuality q;
    q.q = (VideoQualityLevel)level;
    q.description = descr;
    Media m(mp_vid, q, QUrl(url));
    mp_vid->ml_available.append(q);
    mp_vid->m_urls.insert((VideoQualityLevel)level, m);
}

void PythonVideoWrapper::error(QString message)
{
    emit mp_vid->error(message);
}

QString PythonVideoWrapper::getError()
{
    return mp_vid->getError();
}

bool PythonVideoWrapper::isDone()
{
    return mp_vid->isDone();
}

// PythonVideo
PythonVideo::PythonVideo(SitePlugin *site, QString videoId)
{
    ms_video_id = videoId;
    mp_site = site;
}

Media PythonVideo::media(VideoQualityLevel q)
{
    return m_urls[q];
}

void PythonVideo::load()
{
    PythonVideoWrapper w(this);
    QVariantList l;
    l.append(QVariant::fromValue<QObject*>(&w));
    PythonQt::self()->call(static_cast<PythonSitePlugin *>(mp_site)->mof_video.object(), l);
    if (!isDone())
        emit done();
}

// PythonSitePlugin
PythonSitePlugin::PythonSitePlugin(QString name, QString author, int rev, PyObject *fn_forUrl, PyObject *fn_video) :
    QObject(), ms_name(name), ms_author(author), mi_rev(rev)
{
    mof_forUrl = fn_forUrl;
    mof_video = fn_video;
    qDebug("New site PLugin");
}

QString PythonSitePlugin::forUrl(QUrl url)
{
    QVariantList args;
    args << url.toString();
    return PythonQt::self()->call(mof_forUrl.object(), args).toString();
}

Video *PythonSitePlugin::video(QString video_id)
{
    return new PythonVideo(this, video_id);
}
