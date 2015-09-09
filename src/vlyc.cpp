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

#include "vlyc.h"
#include "gui/mainwindow.h"
#include "network/networkaccessmanager.h"

#include <VlycPluginManager.h>
#include <VlycPluginInterface.h>
#include <VlycLegacySitePlugin.h>
#include <VlycUrlHandlerPlugin.h>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

#include <video.h>
#include "logic/TempEventLoop.h"
#include "logic/__lv_hacks.h"
#include "logic/VlycPlayer.h"

#if   defined(Q_OS_WIN)
#   define LIBRARY_EXT ".dll"
#elif defined(Q_OS_MAC)
#   define LIBRARY_EXT ".dylib"
#else
#   define LIBRARY_EXT ".so"
#endif

VlycApp::VlycApp(const QVariantHash &args, QObject *parent) :
    QObject(parent),
    m_args(args),
    mp_plugins(new Vlyc::PluginManager()),
    mp_network(new NetworkAccessManager(this)),
    mp_player(new VlycPlayer(this)),
    mp_window(new MainWindow(this))
{
    mp_plugins->setPrivateInterface((void*)this);
    mp_plugins->setPublicInterface(new Vlyc::PluginInterface(this));
    mp_plugins->bootstrap(QRegularExpression("libvlyc2-.+\\" LIBRARY_EXT "$"));
    mp_plugins->loadPluginsFrom(qApp->applicationDirPath() + "/plugins");

    mp_window->addPluginActions();

    qRegisterMetaType<ResultPtr>();
}

VlycApp::~VlycApp()
{
    delete mp_window;
    delete mp_plugins;
    delete mp_network;
    delete mp_player;
}

const QVariantHash &VlycApp::args() const
{
    return m_args;
}

MainWindow *VlycApp::window() const
{
    return mp_window;
}

Vlyc::PluginManager *VlycApp::plugins() const
{
    return mp_plugins;
}

QNetworkAccessManager *VlycApp::network() const
{
    return mp_network;
}

VlycPlayer *VlycApp::player() const
{
    return mp_player;
}

Vlyc::Result::ResultPtr VlycApp::handleUrl(const QUrl &url)
{
    qDebug("handleUrl(%s)", qPrintable(url.toString()));
    for (Vlyc::UrlHandlerPlugin *plugin : mp_plugins->getPlugins<Vlyc::UrlHandlerPlugin>())
    {
        auto result = plugin->handleUrl(url);
        if (result.isValid())
            return result;
    }
    for (Vlyc::LegacySitePlugin *site : mp_plugins->getPlugins<Vlyc::LegacySitePlugin>())
    {
        QString id = site->forUrl(url);
        if (id.isEmpty())
            continue;
        VideoPtr v = site->video(id);
        //if (!legacy_video_load(v))
        //    continue;
        return new LegacyVideoPromise(v);
    }
    return nullptr;
}

void VlycApp::queueResult(ResultPtr r)
{
    mp_player->queue(r);
}

void VlycApp::playResult(ResultPtr r)
{
    mp_player->queueAndPlay(r);
}

void VlycApp::play(const QUrl &url)
{
    mp_player->queueAndPlay(handleUrl(url));
}
