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

#include "vlycbrowser.h"
#include "pluginmanager.h"
#include "vlyc.h"
#include "video.h"
#include "mainwindow.h"

#include <browser/networkaccessmanager.h>

#include <QtCore/QUrl>

#include <siteplugin.h>

VlycBrowser::VlycBrowser(Vlyc *self) :
    Browser((QObject *)self),
    mp_self(self),
    cookies()
{
    cookies.load();
    mp_network->setCookieJar(&cookies);
}

VlycBrowser::~VlycBrowser()
{
    cookies.save();
}

bool VlycBrowser::navigationRequest(QUrl url)
{
    Video *v = mp_self->plugins()->sites_video(url);
    if (v != nullptr)
    {
        qDebug("VlycBrowser: '%s' is a video url: %s", qPrintable(url.toString()), qPrintable(v->site()->name()));
        v->load();
        mp_self->window()->playVideo(v);
        return false;
    }
    return true;
}
