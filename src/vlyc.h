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

#include <VlycResult/Result.h>

#include <functional>

class MainWindow;
class NetworkAccessManager;
class QNetworkAccessManager;
class VlycPlayer;

namespace Vlyc {
class PluginManager;
}


class VlycApp : public QObject
{
    Q_OBJECT
public:
    explicit VlycApp(QObject *parent = 0);

    MainWindow *window() const;
    Vlyc::PluginManager *plugins() const;
    QNetworkAccessManager *network() const;
    VlycPlayer *player() const;

    virtual ~VlycApp();

    Vlyc::Result::ResultPtr handleUrl(const QUrl &url);
    void queueResult(Vlyc::Result::ResultPtr);
    void playResult(Vlyc::Result::ResultPtr);

public slots:
    void play(const QUrl &url);

private:
    Vlyc::PluginManager *mp_plugins;
    NetworkAccessManager *mp_network;
    VlycPlayer *mp_player;
    MainWindow *mp_window;
};

#endif // VLYC_H
