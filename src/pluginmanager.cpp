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
#include <toolplugin.h>
#include "vlyc.h"
#include "vlycbrowser.h"

#include <VlycToolPlugin.h>
#include <VlycPluginManager.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <functional>

#ifdef Q_OS_WIN
#   define LIBRARY_EXT ".dll"
#elif defined(Q_OS_MAC)
#   define LIBRARY_EXT ".dylib"
#else
#   define LIBRARY_EXT ".so"
#endif

PluginManager::PluginManager(VlycApp *parent) :
    QObject(parent), mp_self(parent)
{
}

PluginManager::~PluginManager()
{
    foreach (VlycBasePlugin *plugin, ml_plugins)
        delete plugin;
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

// Loading mechanisms
int PluginManager::loadPlugins(QString pluginDir)
{
    static QRegularExpression g_plugin_re("^libvlyc2-.+\\" LIBRARY_EXT "$");

    QDir folder(pluginDir);
    int loadedCnt;

    QStringList other;

    foreach (QString fileName, folder.entryList())
    {
        QString absolute = folder.absoluteFilePath(fileName);
        if (!g_plugin_re.match(fileName).hasMatch())
        {
            //qDebug("Ignoring wrongly named file: %s", qPrintable(fileName));
            other << absolute;
            continue;
        }
        //qDebug("Loading %s", qPrintable(fileName));

        QPluginLoader loader(absolute);

        // Plugin Metadata | Linker Flags
        QJsonValue linkerFlags(loader.metaData()["MetaData"].toObject()["LinkerFlags"]);
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

            qDebug() << flags;

            QLibrary::LoadHints hints = loader.loadHints();
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
            loader.setLoadHints(hints);
        }

        // Load plugin
        QObject *plugin = loader.instance();

        // Initialize Plugin
        if (plugin)
        {
            if (_initPlugin(plugin, fileName))
                loadedCnt++;
        }
        else
            qWarning() << "Could not load plugin from " << fileName << ": " << loader.errorString();
    }

    auto base_fn = std::mem_fn(&PluginManager::_initPlugin);

    foreach (QString path, other)
        foreach (VlycForeignPlugin *plugin, ml_foreign)
            if(plugin->canHandle(path))
            {
                QString fileName = path.split("/").last();
                qDebug() << "Loading " << fileName << " using " << plugin->name();
                // generate fn
                auto fn = std::bind(base_fn, this, std::placeholders::_1, fileName);
                loadedCnt += plugin->loadPlugin(path, fn);
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
        qWarning() << "Failed to load " << fileName << ": Not a vlyc2 plugin.";
        return false;
    }

    VlycPluginInitializer init;
    init.network = (QNetworkAccessManager *)mp_self->browser()->network();
    plugin->initialize(init);

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

    // tool
    VlycToolPlugin *tool = qobject_cast<VlycToolPlugin *>(o);
    if (tool)
        ml_tool.append(tool);

    qDebug() << "Loaded " << plugin->name() << " r" << plugin->rev() << " by " << plugin->author() << ".";
    emit pluginLoaded(plugin);
    return true;
}

// SiteManager duties
VideoPtr PluginManager::sites_video(QUrl url)
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

// Tool stuff
void PluginManager::constructToolMenu(QMenu &toolMenu)
{
    for (Vlyc::ToolPlugin *p : mp_self->plugins2()->getPlugins<Vlyc::ToolPlugin>())
    {
        QAction *action = p->toolMenuAction();
        qDebug("Adding action '%s' from %s", qPrintable(action->text()), qPrintable(p->name()));
        toolMenu.addAction(action);
    }
    foreach (VlycToolPlugin *p, ml_tool)
    {
        QAction *action = p->toolMenuAction();
        qDebug("Adding action '%s' from %s", qPrintable(action->text()), qPrintable(p->name()));
        toolMenu.addAction(action);
    }
}
