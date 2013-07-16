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

#include "vlyc.h"
#include "mainwindow.h"
#include "vlycbrowser.h"
#include "sitemanager.h"

#include <QtWidgets/QApplication>

Vlyc::Vlyc(QObject *parent) :
    QObject(parent),
    mp_window(new MainWindow(this)),
    mp_sites(new SiteManager(qApp->applicationDirPath())),
    mp_browser(new VlycBrowser(this))
{
}

Vlyc::~Vlyc()
{
    delete mp_window;
    delete mp_sites;
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

SiteManager *Vlyc::sites() const
{
    return mp_sites;
}
