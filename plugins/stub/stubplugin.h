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

#ifndef STUBPLUGIN_H
#define STUBPLUGIN_H

#include "siteplugin.h"
#include <QObject>
#include <QString>

class StubPlugin : public QObject, public SitePlugin
{
    Q_OBJECT
    Q_INTERFACES(SitePlugin)
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.SitePlugin/1.0")
public:
    virtual QString name() { return "Stub"; }
    virtual QString author() { return "Orochimarufan"; }
    virtual int rev() { return 1; }

    virtual QString forUrl(QUrl url);
    virtual Video* video(QString video_id);
};

#endif // STUBPLUGIN_H
