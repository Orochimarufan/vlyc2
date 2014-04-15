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
/* the browser is modeled after the "Tab Browser" example found in the Qt
 * documentation, available under GPLv3 */

#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include "simplefilecookiejar.h"

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkAccessManager(QObject *parent = 0);
    ~NetworkAccessManager();

private:
    QList<QString> ml_trustedHosts;
    qint64 mi_finished;
    qint64 mi_fromCache;
    qint64 mi_pipelined;
    qint64 mi_secure;
    qint64 mi_downloadBuffer;
    SimpleFileCookieJar cookies;

public slots:
    void loadSettings();
    void requestFinished(QNetworkReply *reply);
};

#endif // NETWORKACCESSMANAGER_H
