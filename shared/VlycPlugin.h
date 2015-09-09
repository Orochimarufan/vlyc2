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

#include <QtPlugin>
#include <QJsonObject>

#include "VlycConfig.h"

namespace Vlyc {

class PluginInterface;
class PluginManager;
typedef PluginManager PluginManagerInterface;

/**
 * @brief The InitEvent struct
 * Event sent to Plugin.init().
 *
 * Subject to change!
 * Any new members will be appended.
 * Removals will be replaced by placeholders.
 */
struct VLYC_EXPORT InitEvent
{
    PluginManagerInterface *plugin_manager;
    QJsonObject *metadata;
    PluginInterface *interface;
    void *private_interface;
};

/**
 * @brief The Plugin class
 * Base class for all vlyc2 plugins.
 */
class VLYC_EXPORT Plugin
{
public:
    Plugin();
    virtual ~Plugin();

    /// Plugin ID. must be unique.
    virtual QString id() const = 0;

    /// Initialize your plugin here.
    virtual void init(InitEvent &init);

    // Plugin metadata
    QJsonObject *metadata() const;
    QString name() const;
    QString author() const;
    QString version() const;

    PluginManagerInterface *plugins();
    PluginInterface *interface();

private:
    InitEvent m_initializer;
    void _init(InitEvent &init);
    friend class PluginManagerPrivate;
};

#define VLYC_PLUGIN_HEAD(ID) public:\
    QString id() const override { return QStringLiteral(#ID); }

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Vlyc::Plugin, "me.sodimm.oro.vlyc.Plugin/2.0")
QT_END_NAMESPACE
