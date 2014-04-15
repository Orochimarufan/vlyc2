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

#ifndef VLYCQTPLUGINLOADER_H
#define VLYCQTPLUGINLOADER_H

#include <QPluginLoader>
#include <QRegularExpression>

#include "VlycLoaderPlugin.h"

namespace Vlyc {

class QtPluginLoader : public QPluginLoader, public PluginLoader
{
    LoaderPlugin *mp_loader_plugin;
    QJsonObject meta_data;

public:
    QtPluginLoader(LoaderPlugin *loaderPlugin, const QString &fileName);

    LoaderPlugin *loaderPlugin();

    QString fileName();
    QJsonObject *metaData();
    QString errorString();
    Plugin *instance();
    bool load();
    bool isLoaded();
    bool unload();
};

class QtLoaderPlugin : public LoaderPlugin
{
    //BuiltinPluginLoader *selfLoader;
    QRegularExpression cpp_plugin_match;
    QList<PluginLoader *> static_plugins;

public:
    const static QString ID;

    QtLoaderPlugin(QRegularExpression cpp_plugin_match);

    QString id() const;

    QString name() const;

    int rev() const;

    void init(InitEvent &init);

    QList<PluginLoader *> builtinPlugins();

    PluginLoader *loaderFor(const QString &fileName);
};

}

#endif // VLYCQTPLUGINLOADER_H
