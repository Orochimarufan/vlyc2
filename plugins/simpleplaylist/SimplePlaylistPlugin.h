/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#pragma once

#include <VlycUrlHandlerPlugin.h>

#include <VlycResult/Object.h>


class SimplePlaylistPlugin : public QObject, public Vlyc::UrlHandlerPlugin
{
    Q_INTERFACES(Vlyc::Plugin)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.Plugin/2.0" FILE "plugin.json")

    Vlyc::Result::ObjectPtr super;

public:
    SimplePlaylistPlugin();

    QString id() const { return "me.sodimm.oro.vlyc.plugin.SimplePlaylist"; }

    Vlyc::Result::ResultPtr handleUrl(const QUrl &url);
};
