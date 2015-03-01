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

#include <QString>

#include "VlycPlugin.h"

namespace Vlyc {

class PluginLoader;

/**
 * @brief The LoaderPlugin class
 * A vlyc2 plugin that can load other plugins.
 */
class VLYC_EXPORT LoaderPlugin : public virtual Plugin
{
public:
    /// Get the PluginLoader instance for fileName. Return nullptr if unapplicable.
    virtual PluginLoader *loaderFor(const QString &fileName) = 0;

    /// A LoaderPlugin may bring builtin plugins.
    virtual QList<PluginLoader *> builtinPlugins() = 0;
};

/**
 * @brief The PluginLoader class
 * Used to load vlyc2 plugins.
 * For every loaded plugin, one of these exists.
 */
class VLYC_EXPORT PluginLoader
{
public:
    virtual ~PluginLoader();

    virtual LoaderPlugin *loaderPlugin() = 0;

    virtual QString fileName() = 0;
    virtual QJsonObject *metaData() = 0;
    virtual QString errorString() = 0;

    virtual Plugin *instance() = 0;

    virtual bool load() = 0;
    virtual bool isLoaded() = 0;
    virtual bool unload() = 0;
};

/**
 * @brief The BuiltinPluginLoader class
 * Static plugin loader.
 * Useful for providing static/builtin plugins.
 */
class VLYC_EXPORT BuiltinPluginLoader : public PluginLoader
{
    LoaderPlugin *mp_loader_plugin;
    QString m_file_name;
    QJsonObject m_meta_data;
    QString m_error_string;
    Plugin *mp_instance;

public:
    BuiltinPluginLoader(LoaderPlugin *loaderPlugin, const QString &fileName, const QJsonObject &metaData, Plugin *instance);
    BuiltinPluginLoader(LoaderPlugin *loaderPlugin, const QString &fileName, const QJsonObject &metaData, const QString &errorString);

    virtual LoaderPlugin *loaderPlugin();

    virtual QString fileName();
    virtual QJsonObject *metaData();
    virtual QString errorString();

    virtual Plugin *instance();

    virtual bool load();
    virtual bool isLoaded();
    virtual bool unload();
};

}
