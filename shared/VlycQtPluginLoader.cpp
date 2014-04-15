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

#include "VlycQtPluginLoader.h"

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

namespace Vlyc {

QtPluginLoader::QtPluginLoader(LoaderPlugin *loader, const QString &fileName) :
    QPluginLoader(fileName), mp_loader_plugin(loader)
{
    meta_data = QPluginLoader::metaData().value("MetaData").toObject();
}

LoaderPlugin *QtPluginLoader::loaderPlugin()
{
    return mp_loader_plugin;
}

QString QtPluginLoader::fileName()
{
    return QPluginLoader::fileName();
}

QJsonObject *QtPluginLoader::metaData()
{
    return &meta_data;
}

QString QtPluginLoader::errorString()
{
    return QPluginLoader::errorString();
}

Plugin *QtPluginLoader::instance()
{
    if (!isLoaded())
        load();
    return qobject_cast<Plugin *>(QPluginLoader::instance());
}

static const char *QJsonValue_typeName(const QJsonValue &v)
{
    switch (v.type())
    {
    case QJsonValue::Array:
        return "Array";
    case QJsonValue::Bool:
        return "Bool";
    case QJsonValue::Double:
        return "Double";
    case QJsonValue::Null:
        return "null";
    case QJsonValue::Object:
        return "Object";
    case QJsonValue::String:
        return "String";
    case QJsonValue::Undefined:
        return "undefined";
    default:
        return "Unknown";
    }
}

bool QtPluginLoader::load()
{
    // Plugin Metadata | Linker Flags
    QJsonValue linkerFlags((*metaData())["LinkerFlags"]);
    if (!linkerFlags.isUndefined() && !linkerFlags.isNull())
    {
        QStringList flags;
        if (linkerFlags.isString())
            flags = linkerFlags.toString().split("|");
        else if (linkerFlags.isArray())
            for (QJsonValue flag : linkerFlags.toArray())
                if (flag.isString())
                    flags.append(flag.toString());
                else
                    qWarning() << "Plugin Metadata | Ignoring LinkerFlag: Not String but " << QJsonValue_typeName(flag);
        else
            qWarning() << "Plugin Metadata | Ignoring LinkerFlags: Not String nor Array but " << QJsonValue_typeName(linkerFlags);

        //qDebug() << "Qt C++ Plugin Linker flags:" << flags;

        QLibrary::LoadHints hints = loadHints();
        for (QString flag : flags)
            if (flag == "LD_LAZY")
                hints = hints & ~QLibrary::ResolveAllSymbolsHint;
            else if (flag == "LD_NOW")
                hints = hints | QLibrary::ResolveAllSymbolsHint;
            else if (flag == "LD_LOCAL")
                hints = hints & ~QLibrary::ExportExternalSymbolsHint;
            else if (flag == "LD_GLOBAL")
                hints = hints | QLibrary::ExportExternalSymbolsHint;
            else
                qWarning() << "Plugin Metadata | Ignoring LinkerFlag: Unknown flag " << flag;
        setLoadHints(hints);
    }

    return QPluginLoader::load();
}

bool QtPluginLoader::isLoaded()
{
    return QPluginLoader::isLoaded();
}

bool QtPluginLoader::unload()
{
    return QPluginLoader::unload();
}

// --- LoaderPlugin ---
const QString QtLoaderPlugin::ID = "__CppLoaderPlugin";

QtLoaderPlugin::QtLoaderPlugin(QRegularExpression cpp_plugin_match) :
    cpp_plugin_match(cpp_plugin_match)
{
    /*static QJsonObject metaData;
    if (metaData.isEmpty())
    {
        QVariantMap map;
        map.insert("ID", CPP_ID);
        map.insert("Name", "Vlyc2 Qt C++ Plugin Loader");
        map.insert("Author", "Orochimarufan");
        map.insert("Version", "1.0");
        map.insert("Description", "The default Plugin Loader");
        metaData = QJsonObject::fromVariantMap(map);
    }
    selfLoader = new BuiltinPluginLoader(this, NULL, metaData, this);*/

    // register static plugins
    for (QObject *o : QPluginLoader::staticInstances())
    {
        Plugin *plugin = qobject_cast<Plugin *>(o);
        if (plugin)
        {
            QVariantMap map;
            map.insert("ID", plugin->id());
            static_plugins << new BuiltinPluginLoader(this, NULL, QJsonObject::fromVariantMap(map), plugin);
        }
    }
}

QString QtLoaderPlugin::id() const
{
    return ID;
}

QList<PluginLoader *> QtLoaderPlugin::builtinPlugins()
{
    return static_plugins;
}

PluginLoader *QtLoaderPlugin::loaderFor(const QString &fileName)
{
    if (cpp_plugin_match.match(fileName).hasMatch())
        return new QtPluginLoader(this, fileName);
    return nullptr;
}

}
