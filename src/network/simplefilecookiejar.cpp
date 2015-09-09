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

#include "simplefilecookiejar.h"

#include <QDateTime>
#include <QNetworkCookie>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTextStream>

SimpleFileCookieJar::SimpleFileCookieJar(QString file, QObject *parent) :
    QNetworkCookieJar(parent), cookieFile(file)
{
    if (file.isNull())
    {
        QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        cookieFile = dir.absoluteFilePath("cookies.txt");
    }
    qDebug("CookieJar: %s", qPrintable(cookieFile));
}

QString SimpleFileCookieJar::fileName()
{
    return cookieFile;
}

void SimpleFileCookieJar::clean()
{
    auto cookies = allCookies();

    QDateTime now = QDateTime::currentDateTime();
    foreach(QNetworkCookie cookie, cookies)
        if(cookie.expirationDate() < now)
            cookies.removeAll(cookie);

    setAllCookies(cookies);
}

void SimpleFileCookieJar::save()
{
    clean();

    QFile f(cookieFile);
    f.open(QIODevice::WriteOnly);

    foreach(QNetworkCookie cookie, allCookies())
    {
        f.write(cookie.toRawForm());
        f.write("\n");
    }

    f.close();
}

void SimpleFileCookieJar::load()
{
    QList<QNetworkCookie> cookies;

    QFile f(cookieFile);
    if(!f.exists()) return;
    f.open(QIODevice::ReadOnly);

    QTextStream s(&f);
    while(!s.atEnd())
        cookies << QNetworkCookie::parseCookies(s.readLine().toUtf8());

    f.close();

    setAllCookies(cookies);

    clean();
}
