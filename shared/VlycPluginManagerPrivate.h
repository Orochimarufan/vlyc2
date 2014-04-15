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

#ifndef VLYCPLUGINMANAGERPRIVATE_H
#define VLYCPLUGINMANAGERPRIVATE_H

#include "VlycPluginManager.h"
#include "VlycLoaderPlugin.h"

namespace Vlyc {

class PluginManagerPrivate
{
    //PluginManager *q_ptr;
    //Q_DECLARE_PUBLIC(PluginManager)
    friend class PluginManager;

    QList<PluginLoader *> m_plugin_loaders;
    QList<Plugin *> m_plugins;
    QList<PluginLoader *> m_failed_plugins;

    void *private_interface;

    PluginManagerPrivate();

    bool initPlugin(PluginLoader *loader);
};

}

#endif // VLYCPLUGINMANAGERPRIVATE_H
