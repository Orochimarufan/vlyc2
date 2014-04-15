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
#include "mainwindow.h"
#include "vlycbrowser.h"
#include "pluginmanager.h"

#include <VlycPluginManager.h>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

#define LIBRARY_EXT ".so"

VlycApp::VlycApp(QObject *parent) :
    QObject(parent),
    mp_window(new MainWindow(this)),
    mp_plugins(new PluginManager(this)),
    mp_plugins2(new Vlyc::PluginManager()),
    mp_browser(new VlycBrowser(this))
{
    mp_plugins2->setPrivateInterface((void*)this);
    mp_plugins2->bootstrap(QRegularExpression("libvlyc2-.+\\" LIBRARY_EXT "$"));
    mp_plugins2->loadPluginsFrom(qApp->applicationDirPath() + "/plugins");

    mp_plugins->loadPlugins(qApp->applicationDirPath());
    mp_plugins->constructToolMenu(mp_window->getToolMenu());
}

VlycApp::~VlycApp()
{
    delete mp_window;
    delete mp_plugins;
    delete mp_browser;
}

VlycBrowser *VlycApp::browser() const
{
    return mp_browser;
}

MainWindow *VlycApp::window() const
{
    return mp_window;
}

PluginManager *VlycApp::plugins() const
{
    return mp_plugins;
}

Vlyc::PluginManager *VlycApp::plugins2() const
{
    return mp_plugins2;
}
