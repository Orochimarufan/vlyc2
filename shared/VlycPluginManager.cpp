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

#include "VlycPluginManager.h"
#include "VlycQtPluginLoader.h"
#include "VlycPluginManagerPrivate.h"

#include <QVariantMap>
#include <QDir>
#include <QDebug>

#define CPP_ID "__VlycCppLoaderPlugin"

namespace Vlyc {

PluginManager::PluginManager()
{
    d_ptr = new PluginManagerPrivate();
    d_ptr->manager = this;
}

PluginManager::~PluginManager()
{
    delete d_ptr;
}

void PluginManager::bootstrap(QRegularExpression cpp_plugin_match)
{
    //Q_D(PluginManager);
    if (getPluginById(CPP_ID) == nullptr)
    {
        QtLoaderPlugin *loader = new QtLoaderPlugin(cpp_plugin_match);
        d_ptr->m_plugins << loader;
    }
}

void PluginManager::loadPluginsFrom(const QString &directory, bool try_twice)
{
    Q_D(PluginManager);

    QDir folder(directory);

    if (!folder.exists())
        return;

    int loadedCnt;
    QStringList remaining;

    //qDebug() << folder.entryList();

    for (QString fileName : folder.entryList())
    {
        if (fileName.startsWith("."))
            continue;

        QString filePath(folder.absoluteFilePath(fileName));
        PluginLoader *loader = nullptr;

        //qDebug() << filePath;

        for (LoaderPlugin *loader_plugin : getPlugins<LoaderPlugin>())
        {
            //qDebug() << loader_plugin->name();

            loader = loader_plugin->loaderFor(filePath);

            if (loader == nullptr)
                continue;

            //qDebug() << loader->metaData();

            if (loader->load())
            {
                if (d->initPlugin(loader))
                    ++loadedCnt;
            }
            else
                d->m_failed_plugins << loader;

            break;
        }

        if (loader == nullptr)
            remaining << filePath;
    }

    if (try_twice)
        for (QString filePath : remaining)
            for (LoaderPlugin *loader_plugin : getPlugins<LoaderPlugin>())
            {
                PluginLoader *loader = loader_plugin->loaderFor(filePath);

                if (loader == nullptr)
                    continue;

                if (loader->load())
                {
                    if (d->initPlugin(loader))
                        ++loadedCnt;
                }
                else
                    d->m_failed_plugins << loader;

                break;
            }
}

QList<Plugin *> PluginManager::getAllPlugins()
{
    //Q_D(PluginManager);
    return d_ptr->m_plugins;
}

Plugin *PluginManager::getPluginById(const QString &id)
{
    //Q_D(PluginManager);
    for (Plugin *plugin : d_ptr->m_plugins)
        if (plugin->id() == id)
            return plugin;
    return nullptr;
}

// HAX
void PluginManager::setPrivateInterface(void *pi)
{
    d_ptr->private_interface = pi;
}

void *PluginManager::getPrivateInterface()
{
    return d_ptr->private_interface;
}

void PluginManager::setPublicInterface(PluginInterface *i)
{
    d_ptr->interface = i;
}

PluginInterface *PluginManager::getPublicInterface()
{
    return d_ptr->interface;
}

}
