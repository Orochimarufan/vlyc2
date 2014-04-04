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

#include <QtWidgets/QApplication>

Vlyc::Vlyc(QObject *parent) :
    QObject(parent),
    mp_window(new MainWindow(this)),
    mp_plugins(new PluginManager(this)),
    mp_browser(new VlycBrowser(this))
{
    mp_plugins->loadPlugins(qApp->applicationDirPath().append("/plugins"));
    mp_plugins->constructToolMenu(mp_window->getToolMenu());
}

Vlyc::~Vlyc()
{
    delete mp_window;
    delete mp_plugins;
    delete mp_browser;
}

VlycBrowser *Vlyc::browser() const
{
    return mp_browser;
}

MainWindow *Vlyc::window() const
{
    return mp_window;
}

PluginManager *Vlyc::plugins() const
{
    return mp_plugins;
}
