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

#ifndef VLYC_H
#define VLYC_H

#include <QObject>
#include <QUrl>

class MainWindow;
class NetworkAccessManager;
class QNetworkAccessManager;

namespace Vlyc {
class PluginManager;
}


class VlycApp : public QObject
{
    Q_OBJECT
public:
    explicit VlycApp(QObject *parent = 0);

    MainWindow *window() const;
    Vlyc::PluginManager *plugins2() const;

    virtual ~VlycApp();

    QNetworkAccessManager *network();
    bool tryPlayUrl(QUrl url);

private:
    MainWindow *mp_window;
    Vlyc::PluginManager *mp_plugins;
    NetworkAccessManager *mp_network;
};

#endif // VLYC_H
