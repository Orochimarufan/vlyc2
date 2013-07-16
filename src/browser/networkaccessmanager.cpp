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
/* NOTE: the browser is modeled after the "Tab Browser" example found in the Qt
 * documentation, available under GPLv3 */

#include "networkaccessmanager.h"

#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>

#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent),
    mi_finished(0), mi_fromCache(0), mi_pipelined(0), mi_secure(0), mi_downloadBuffer(0)
{
    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));

    loadSettings();

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    diskCache->setCacheDirectory(location);
    setCache(diskCache);
}

void NetworkAccessManager::requestFinished(QNetworkReply *reply)
{
    mi_finished++;

    if (reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool())
        mi_fromCache++;

    if (reply->attribute(QNetworkRequest::HttpPipeliningWasUsedAttribute).toBool())
        mi_pipelined++;

    if (reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute).toBool())
        mi_secure++;

    if (reply->attribute(QNetworkRequest::DownloadBufferAttribute).toBool())
        mi_downloadBuffer++;
}

void NetworkAccessManager::loadSettings()
{
    QNetworkProxy proxy;
    QSettings settings;

    settings.beginGroup(QStringLiteral("proxy"));
    int type = settings.value(QStringLiteral("type"), 0).toInt();

    switch (type) {
    case 0:
    case 2:
        return;
    case 1:
        proxy = QNetworkProxy::Socks5Proxy;
        break;
    case 3:
        proxy = QNetworkProxy::HttpProxy;
        break;
    case 4:
    case 5:
        return;
    }

    proxy.setHostName(settings.value(QStringLiteral("host")).toString());
    proxy.setPort(settings.value(QStringLiteral("port"), 1080).toInt());
    proxy.setUser(settings.value(QStringLiteral("user")).toString());
    proxy.setPassword(QString(QByteArray::fromBase64(settings.value(QStringLiteral("password")).toByteArray())));

    setProxy(proxy);
}
