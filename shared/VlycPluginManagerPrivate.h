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

#include "VlycPluginManager.h"
#include "VlycLoaderPlugin.h"

namespace Vlyc {

class VLYC_HIDDEN PluginManagerPrivate
{
    //PluginManager *q_ptr;
    //Q_DECLARE_PUBLIC(PluginManager)
    friend class PluginManager;

    PluginManager *manager;

    QList<PluginLoader *> m_plugin_loaders;
    QList<Plugin *> m_plugins;
    QList<PluginLoader *> m_failed_plugins;

    void *private_interface;
    PluginInterface *interface;

    PluginManagerPrivate();

    bool initPlugin(PluginLoader *loader);
};

}
