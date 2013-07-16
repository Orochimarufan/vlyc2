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

#include "vlycbrowser.h"
#include "sitemanager.h"
#include "vlyc.h"
#include "video.h"
#include "mainwindow.h"

#include <QtCore/QUrl>

VlycBrowser::VlycBrowser(Vlyc *self) :
    Browser((QObject *)self),
    mp_self(self)
{
}

bool VlycBrowser::navigationRequest(QUrl url)
{
    Video *v = mp_self->sites()->video(url);
    if (v != nullptr)
    {
        v->load();
        mp_self->window()->playVideo(v);
        return false;
    }
    return true;
}
