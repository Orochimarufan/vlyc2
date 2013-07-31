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

#include "pluginmanager.h"
#include <siteplugin.h>
#include <foreignplugin.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <functional>

#ifdef Q_OS_WIN
#   define LIBRARY_EXT ".dll"
#elif defined(Q_OS_MAC)
#   define LIBRARY_EXT ".dylib"
#else
#   define LIBRARY_EXT ".so"
#endif

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
}

PluginManager::~PluginManager()
{
    foreach (VlycBasePlugin *plugin, ml_plugins)
        delete plugin;
}

// Loading mechanisms
int PluginManager::loadPlugins(QString pluginDir)
{
    static QRegularExpression g_plugin_re("^libvlyc2-.+\\" LIBRARY_EXT "$");

    QDir folder(pluginDir);
    int loadedCnt;

    QStringList other;

    foreach (QString fileName, folder.entryList())
    {
        if (!g_plugin_re.match(fileName).hasMatch())
        {
            //qDebug("Ignoring wrongly named file: %s", qPrintable(fileName));
            other << fileName;
            continue;
        }
        //qDebug("Loading %s", qPrintable(fileName));

        QPluginLoader loader(folder.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin)
            if (_initPlugin(plugin, fileName))
                loadedCnt++;
    }

    auto base_fn = std::mem_fn(&PluginManager::_initPlugin);

    foreach (QString fileName, other)
        foreach (VlycForeignPlugin *plugin, ml_foreign)
            if(plugin->canHandle(fileName))
            {
                qDebug("Loading '%s' using '%s'", qPrintable(fileName), qPrintable(plugin->name()));
                // generate fn
                auto fn = std::bind(base_fn, this, std::placeholders::_1, fileName);
                loadedCnt += plugin->loadPlugin(fileName, fn);
                break;
            }

    return loadedCnt;
}

bool PluginManager::_initPlugin(QObject *o, QString fileName)
{
    // plugin
    VlycBasePlugin *plugin = qobject_cast<VlycBasePlugin *>(o);
    if (!plugin)
    {
        qDebug("Failed to load %s: Not a vlyc2 plugin.", qPrintable(fileName));
        return false;
    }
    ml_pluginObjects.append(o);
    ml_plugins.append(plugin);

    // site
    SitePlugin *site = qobject_cast<SitePlugin *>(o);
    if (site)
        ml_sites.append(site);

    // foreign
    VlycForeignPlugin *foreign = qobject_cast<VlycForeignPlugin *>(o);
    if (foreign)
        ml_foreign.append(foreign);

    qDebug("Loaded %s r%i by %s from %s", qPrintable(plugin->name()), plugin->rev(), qPrintable(plugin->author()), qPrintable(fileName));
    emit pluginLoaded(plugin);
    return true;
}

// SiteManager duties
Video *PluginManager::sites_video(QUrl url)
{
    QString id;
    foreach (SitePlugin *site, ml_sites)
        if (!(id = site->forUrl(url)).isEmpty())
            return site->video(id);
    return nullptr;
}

bool PluginManager::sites_getSiteId(QUrl url, SitePlugin *&site_out, QString &id_out)
{
    QString id;
    foreach (SitePlugin *site, ml_sites)
        if (!(id = site->forUrl(url)).isEmpty())
        {
            site_out = site;
            id_out = id;
            return true;
        }
    return false;
}
