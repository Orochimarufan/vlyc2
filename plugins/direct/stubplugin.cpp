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
QStringList mimeWhitelist = QStringList() <<
    "application/ogg";
#else
QStringList videoFileExts = QStringList() <<
    "mkv" << "mp4" << "avi" << "webm" << "ogv" << "wmv" << "mpg" << "ts" << "3gp" << "ps" << "ogm";
#endif

#include <VlycResult/Media.h>

// TODO: make it public D:
#include "../../src/vlyc.h"


DirectAccessPlugin::DirectAccessPlugin()
{
#ifdef WITH_LIBMAGIC
    m_cookie = magic_open(MAGIC_MIME_TYPE);
    magic_load(m_cookie, NULL);
#endif
}

void DirectAccessPlugin::init(Vlyc::InitEvent &e)
{
    mp_network = ((VlycApp*)e.private_interface)->network();
}

DirectAccessPlugin::~DirectAccessPlugin()
{
#ifdef WITH_LIBMAGIC
    magic_close(m_cookie);
#endif
}


Vlyc::Result::ResultPtr DirectAccessPlugin::handleUrl(const QUrl &url)
{
    bool valid = false;

    if (url.scheme() == "file")
    {
#ifdef WITH_LIBMAGIC
        QByteArray mime = magic_file(m_cookie, url.path().toUtf8().constData());
        qDebug("%s mime: %s", qPrintable(url.fileName()), qPrintable(mime));
        if (mime.startsWith("video/") || mime.startsWith("audio/") || mimeWhitelist.contains(mime))
        {
#else
        if (videoFileExts.contains(url.path().split(".").last().toLower()))
        {
#endif
            valid = true;
        }
    }
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
            valid = true;
    }

    if (valid)
        return Vlyc::Result::File(url);
    else
        return nullptr;
}
