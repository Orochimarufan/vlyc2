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
#include <PythonQt/PythonQtConversion.h>

// PythonVideo

PythonVideo::PythonVideo(/*Python*/SitePlugin *site, const QString &video_id) :
    StandardVideo(site, video_id)
{
}

QString PythonVideo::videoId() const
{
    return ms_video_id;
}

QString PythonVideo::title() const
{
    return ms_title;
}

QString PythonVideo::author() const
{
    return ms_author;
}

QString PythonVideo::description() const
{
    return ms_description;
}

int PythonVideo::views() const
{
    return mi_views;
}

int PythonVideo::likes() const
{
    return mi_likes;
}

int PythonVideo::dislikes() const
{
    return mi_dislikes;
}

int PythonVideo::favorites() const
{
    return mi_favorites;
}

void PythonVideo::setTitle(const QString &title)
{
    ms_title = title;
}

void PythonVideo::setAuthor(const QString &author)
{
    ms_author = author;
}

void PythonVideo::setDescription(const QString &description)
{
    ms_description = description;
}

void PythonVideo::setViews(const int &views)
{
    mi_views = views;
}

void PythonVideo::setLikes(const int &likes)
{
    mi_likes = likes;
}

void PythonVideo::setDislikes(const int &dislikes)
{
    mi_dislikes = dislikes;
}

void PythonVideo::setFavorites(const int &favorites)
{
    mi_favorites = favorites;
}

void PythonVideo::addQuality(const int &level, const QString &descr, const QString &url)
{
    qDebug("AddQ: %i %s", level, qPrintable(descr));
    VideoQuality q;
    q.q = (VideoQualityLevel)level;
    q.description = descr;
    Media m(this, q, QUrl(url));
    ml_available.append(q);
    m_urls.insert((VideoQualityLevel)level, m);
}

void PythonVideo::setError(QString message)
{
    emit error(message);
}

void PythonVideo::setDone()
{
    emit done();
}

Media PythonVideo::media(VideoQualityLevel q)
{
    return m_urls[q];
}

void PythonVideo::load()
{
    if (isDone()) return;

    QVariantList l;
    l.append(QVariant::fromValue<QObject *>(this));

    static_cast<PythonSitePlugin *>(mp_site)->mof_video.call(l);

    if (PyErr_Occurred())
    {
        PyObject *a, *b, *c;
        PyErr_Fetch(&a, &b, &c);
        emit error(PythonQtConv::PyObjGetString(b));
        Py_DECREF(a);
        Py_DECREF(b);
        Py_DECREF(c);
    }
}

// PythonSitePlugin
PythonSitePlugin::PythonSitePlugin(QString name, QString author, int rev, PyObject *fn_forUrl, PyObject *fn_video) :
    QObject(), ms_name(name), ms_author(author), mi_rev(rev)
{
    mof_forUrl = fn_forUrl;
    mof_video = fn_video;
}

QString PythonSitePlugin::forUrl(QUrl url)
{
    QVariantList args;
    args << url.toString();
    return mof_forUrl.call(args).toString();
}

Video *PythonSitePlugin::video(QString video_id)
{
    return new PythonVideo(this, video_id);
}
