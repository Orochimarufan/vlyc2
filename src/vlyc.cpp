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
#include <VlycLegacySitePlugin.h>
#include <VlycUrlHandlerPlugin.h>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

#if   defined(Q_OS_WIN)
#   define LIBRARY_EXT ".dll"
#elif defined(Q_OS_MAC)
#   define LIBRARY_EXT ".dylib"
#else
#   define LIBRARY_EXT ".so"
#endif

VlycApp::VlycApp(QObject *parent) :
    QObject(parent),
    mp_window(new MainWindow(this)),
    mp_plugins(new Vlyc::PluginManager()),
    mp_network(new NetworkAccessManager(this))
{
    mp_plugins->setPrivateInterface((void*)this);
    mp_plugins->bootstrap(QRegularExpression("libvlyc2-.+\\" LIBRARY_EXT "$"));
    mp_plugins->loadPluginsFrom(qApp->applicationDirPath() + "/plugins");

    mp_window->addPluginActions();
}

VlycApp::~VlycApp()
{
    delete mp_window;
    delete mp_plugins;
    delete mp_network;
}

MainWindow *VlycApp::window() const
{
    return mp_window;
}

Vlyc::PluginManager *VlycApp::plugins2() const
{
    return mp_plugins;
}

QNetworkAccessManager *VlycApp::network()
{
    return mp_network;
}

bool VlycApp::tryPlayUrl(QUrl url)
{
    for (Vlyc::LegacySitePlugin *site : mp_plugins->getPlugins<Vlyc::LegacySitePlugin>())
    {
        QString id = site->forUrl(url);
        if (id.isEmpty())
            continue;
        VideoPtr v = site->video(id);
        qDebug("'%s' is a video url: %s", qPrintable(url.toString()), qPrintable(v->site()->name()));
        v->load();
        mp_window->playVideo(v);
        return true;
    }
    return false;
}

Vlyc::Result::ResultPtr VlycApp::handleUrl(const QUrl &url)
{
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
        return new LegacyVideoResult(v);
    }
    return nullptr;
}

#include <VlycResult/Playlist.h>
#include <VlycResult/Url.h>

void VlycApp::handleResult(Vlyc::Result::ResultPtr result)
{
    auto video = result.cast<LegacyVideoResult>();
    if (video.isValid())
    {
        qDebug("Playing LegacyVideoResult");
        VideoPtr v(video->video());
        v->load();
        mp_window->playVideo(v);
        return;
    }

    auto playlist = result.cast<Vlyc::Result::Playlist>();
    if (playlist.isValid() && playlist->length() > 0)
    {
        handleResult(playlist->get(0));
        return;
    }

    auto url = result.cast<Vlyc::Result::Url>();
    if (url.isValid())
    {
        handleResult(handleUrl(*url));
        return;
    }
}

// LegacyVideoResult
LegacyVideoResult::LegacyVideoResult(VideoPtr video) :
    mp_video(video)
{}

VideoPtr LegacyVideoResult::video()
{
    return mp_video;
}
