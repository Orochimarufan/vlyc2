/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "sitemanager.h"

#include <QDir>
#include <QPluginLoader>
#include <QUrl>

#ifdef Q_OS_WIN
#   define LIBRARY_EXT ".dll"
#else
#   define LIBRARY_EXT ".so"
#endif

SiteManager::SiteManager(QString pluginDir, QObject *parent) :
    QObject(parent)
{
    if (!pluginDir.isNull())
        this->loadPlugins(pluginDir);
}

void SiteManager::loadPlugins(QString pluginDir)
{
    QDir d(pluginDir);
    foreach (QString fileName, d.entryList(QDir::Files))
    {
        if (!fileName.endsWith(LIBRARY_EXT))
            continue;
        qDebug("Loading %s", qPrintable(fileName));
        QPluginLoader loader(d.absoluteFilePath(fileName));
        QObject* plugin = loader.instance();
        if (plugin)
            this->loadPlugin(plugin);
    }
}

void SiteManager::loadPlugin(QObject* plugin)
{
    SitePlugin* site = qobject_cast<SitePlugin *>(plugin);
    if (site) {
        qDebug("Loaded %s r%i by %s", qPrintable(site->name()), site->rev(), qPrintable(site->author()));
        this->plugins.append(site);
    }
}

SitePlugin* SiteManager::findSite(QUrl url)
{
    foreach (SitePlugin* site, this->plugins)
    {
        if (!site->forUrl(url).isNull())
            return site;
    }
    return nullptr;
}

Video* SiteManager::video(QUrl url)
{
    QString id;
    foreach (SitePlugin* site, this->plugins)
    {
        id = site->forUrl(url);
        if (!id.isNull())
            return site->video(id);
    }
    return nullptr;
}
