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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>

class VlycBasePlugin;
class VlycForeignPlugin;

class SitePlugin;
class Video;
class Vlyc;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(Vlyc *parent);
    virtual ~PluginManager();

    int loadPlugins(QString pluginDir);
    QList<VlycBasePlugin *> plugins();

    // SiteManager functionality
    Video *sites_video(QUrl url);
    bool sites_getSiteId(QUrl url, SitePlugin *&site_out, QString &id_out);

signals:
    void pluginLoaded(VlycBasePlugin *plugin);

private:
    Vlyc *mp_self;
    QList<QObject *> ml_pluginObjects;
    QList<VlycBasePlugin *> ml_plugins;
    QList<SitePlugin *> ml_sites;
    QList<VlycForeignPlugin *> ml_foreign;

    bool _initPlugin(QObject *o, QString f=QString::null);
};

#endif // PLUGINMANAGER_H
