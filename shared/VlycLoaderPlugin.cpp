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

#include <VlycLoaderPlugin.h>

namespace Vlyc {

PluginLoader::~PluginLoader()
{
}

BuiltinPluginLoader::BuiltinPluginLoader(LoaderPlugin *loader, const QString &fileName, const QJsonObject &metaData, Plugin *instance) :
    mp_loader_plugin(loader), m_file_name(fileName), m_meta_data(metaData), mp_instance(instance)
{
}

BuiltinPluginLoader::BuiltinPluginLoader(LoaderPlugin *loader, const QString &fileName, const QJsonObject &metaData, const QString &errorString) :
    mp_loader_plugin(loader), m_file_name(fileName), m_meta_data(metaData), mp_instance(nullptr), m_error_string(errorString)
{
}

LoaderPlugin *BuiltinPluginLoader::loaderPlugin()
{
    return mp_loader_plugin;
}

QString BuiltinPluginLoader::fileName()
{
    return m_file_name;
}

QJsonObject *BuiltinPluginLoader::metaData()
{
    return &m_meta_data;
}

QString BuiltinPluginLoader::errorString()
{
    return m_error_string;
}

Plugin *BuiltinPluginLoader::instance()
{
    return mp_instance;
}

bool BuiltinPluginLoader::load()
{
    return false;
}

bool BuiltinPluginLoader::isLoaded()
{
    return m_error_string == QString::null;
}

bool BuiltinPluginLoader::unload()
{
    return false;
}

}
