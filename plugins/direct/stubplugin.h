/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include <QtCore/QObject>
#include <QtCore/QString>

#ifdef WITH_LIBMAGIC
#include <magic.h>
#endif

#include <VlycUrlHandlerPlugin.h>


class QNetworkAccessManager;

class DirectAccessPlugin : public QObject, public Vlyc::UrlHandlerPlugin
{
    Q_OBJECT
    Q_INTERFACES(Vlyc::Plugin)
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.Plugin/2.0" FILE "plugin.json")

#ifdef WITH_LIBMAGIC
    magic_t m_cookie;
#endif

    QNetworkAccessManager *mp_network;

public:
    DirectAccessPlugin();
    ~DirectAccessPlugin();

    QString id() const override { return "me.sodimm.oro.vlyc.plugin.DirectAccess"; }

    void init(Vlyc::InitEvent &init) override;

    Vlyc::Result::ResultPtr handleUrl(const QUrl &url) override;
};
