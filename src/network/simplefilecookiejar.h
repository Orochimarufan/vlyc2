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

#ifndef SIMPLEFILECOOKIEJAR_H
#define SIMPLEFILECOOKIEJAR_H

#include <QNetworkCookieJar>

class SimpleFileCookieJar : public QNetworkCookieJar
{
    Q_OBJECT

    QString cookieFile;

public:
    explicit SimpleFileCookieJar(QString file=QString(), QObject *parent = 0);
    
    void clean();
    void save();
    void load();
};

#endif // SIMPLEFILECOOKIEJAR_H
