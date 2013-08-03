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
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#ifdef WITH_LIBMAGIC
#include <QtCore/QByteArray>
#else
QStringList videoFileExts = QStringList() <<
    "mkv" << "mp4" << "avi" << "webm" << "ogv" << "wmv" << "mpg" << "ts" << "3gp";
#endif

struct DirectAccessVideo : StandardVideo
{
    DirectAccessVideo(DirectAccessPlugin *site, const QString &video_id) :
        StandardVideo(site, video_id)
    {
    }

    virtual bool useVlcMeta() const
    {
        return true;
    }

    virtual Media media(VideoQualityLevel q)
    {
        QUrl url(videoId());
        QString ext = url.path().split(".").last();
        return Media{this, VideoQuality{q, ext.toUpper()}, url};
    }

    virtual void load()
    {
        ms_title = videoId().split("/").last();
        ms_author = "File";
        ms_description = "";

        VideoQuality url;
        url.q = VideoQualityLevel::QA_LOWEST;
        url.description = videoId().split(".").last().toUpper();
        ml_available.append(url);

        emit done();
    }
};

DirectAccessPlugin::DirectAccessPlugin()
{
#ifdef WITH_LIBMAGIC
    m_cookie = magic_open(MAGIC_MIME_TYPE);
    magic_load(m_cookie, NULL);
#endif
}

void DirectAccessPlugin::initialize(VlycPluginInitializer init)
{
    mp_network = init.network;
}

DirectAccessPlugin::~DirectAccessPlugin()
{
#ifdef WITH_LIBMAGIC
    magic_close(m_cookie);
#endif
}


QString DirectAccessPlugin::forUrl(QUrl url)
{
    if (url.scheme() == "file")
#ifdef WITH_LIBMAGIC
    {
        QByteArray mime = magic_file(m_cookie, url.path().toUtf8().constData());
        if (mime.startsWith("video/") || mime.startsWith("audio/"))
            return url.toString();
    }
#else
        if (videoFileExts.contains(url.path().split(".").last().toLower()))
            return url.toString();
#endif
        else if ((url.scheme() == "http" || url.scheme() == "https") &&
                 !(url.path().endsWith(".html") || url.path().endsWith(".htm") || url.path().endsWith("/") || url.path().endsWith(".shtml")))
        {
            // TODO: (expensive) HEAD request, make it a config option
            auto reply = mp_network->head(QNetworkRequest(url));
            QEventLoop loop;
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QString h = reply->header(QNetworkRequest::ContentTypeHeader).toString();
            reply->deleteLater();
            if (!h.isEmpty() && (h.startsWith("video/") || h.startsWith("audio/")))
                return url.toString();
        }
    return QString::null;
}

Video* DirectAccessPlugin::video(QString video_id)
{
    return new DirectAccessVideo(this, video_id);
}
