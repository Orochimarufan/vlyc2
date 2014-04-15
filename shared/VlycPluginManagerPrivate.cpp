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

#include "VlycPluginManagerPrivate.h"

#include <QtCore/QJsonDocument>

namespace Vlyc {

PluginManagerPrivate::PluginManagerPrivate()
{
}

bool PluginManagerPrivate::initPlugin(PluginLoader *loader)
{
    Plugin *plugin = loader->instance();

    QJsonObject *meta = loader->metaData();
    if (meta->contains("ID"))
    {
        if (meta->value("ID").toString() != plugin->id())
        {
            qWarning("Plugin ID mismatch: '%s' != '%s'", qPrintable(meta->value("ID").toString()), qPrintable(plugin->id()));
            meta->insert("ID", plugin->id());
        }
    }
    else
        meta->insert("ID", plugin->id());

    m_plugin_loaders << loader;
    m_plugins << plugin;

    InitEvent e;
    e.metadata = meta;
    e.plugin_manager = manager;
    e.private_interface = private_interface;
    plugin->_init(e);

    qDebug("Loaded '%s' %s by %s (%s)", qPrintable(plugin->name()), qPrintable(plugin->version()), qPrintable(plugin->author()), qPrintable(plugin->id()));

    return true;
}

}
