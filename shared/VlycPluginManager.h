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

#include <QObject>
#include <QList>
#include <QRegularExpression>

#include "VlycPlugin.h"

namespace Vlyc {

// Stuff _might_ change
template <typename T>
inline T *plugin_cast(Plugin *p)
{
    return dynamic_cast<T *>(p);
}

class PluginManagerPrivate;
class PluginInterface;

class PluginManager : public QObject
{
    PluginManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(PluginManager)

public:
    /// Create a new PluginManager
    PluginManager();
    ~PluginManager();

    // ---- Loading Plugins ----
    /// Enable Qt Plugin Loader
    void bootstrap(QRegularExpression cpp_plugin_match);

    /// Lazy-load Plugins from a directory
    void loadPluginsFrom(const QString &directory, bool try_twice=true);

    /// Load a single plugin
    bool loadPlugin(const QString &fileName);

    // ---- Getting Info about Plugins ----
    /// Get Plugin by ID
    Plugin *getPluginById(const QString &id);

    /// Get plugin metadata
    QJsonObject getPluginMetaData(const QString &id);
    QJsonObject getPluginMetaData(Plugin *);

    /// Get all plugins
    QList<Plugin *> getAllPlugins();

    /// Get all plugins of a type
    template <typename T>
    QList<T *> getPlugins()
    {
        QList<T *> result;

        for (Plugin *p : getAllPlugins())
        {
            T *plugin = plugin_cast<T>(p);
            if (plugin != NULL)
                result << plugin;
        }

        return result;
    }

    // ---- Preferences ----
    /// Get the preferences object for a plugin
    //PluginPreferences *prefsForPlugin(const QString &id);

    /// Reset the preferences for a plugin
    //void resetPluginPrefs(const QString &id);

    /// Purge preferences of missing plugins from config
    //void cleanConfig();

    // ---- HAX ----
    void setPrivateInterface(void *pi);
    void *getPrivateInterface();

    void setPublicInterface(Vlyc::PluginInterface *);
    Vlyc::PluginInterface* getPublicInterface();
};

}
