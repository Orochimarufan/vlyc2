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

#ifndef VLYCPLUGIN_H
#define VLYCPLUGIN_H

#include <QtPlugin>
#include <QJsonObject>

namespace Vlyc {

/**
 * @brief The InitEvent struct
 * Event sent to Plugin.init().
 *
 * Subject to change!
 * Any new members will be appended.
 * Removals will be replaced by placeholders.
 */
struct InitEvent
{
    QJsonObject *pluginMetaData;
    void *interface;
    void *private_interface;
};

/**
 * @brief The Plugin class
 * Base class for all vlyc2 plugins.
 */
class Plugin
{
public:
    Plugin();
    virtual ~Plugin();

    /// Plugin ID. must be unique.
    virtual QString id() const = 0;

    /// Initialize your plugin here.
    /// You must call this from any derived overrides!
    virtual void init(InitEvent &init);

    // By default taken from metadata
    virtual QString name() const;
    virtual QString author() const;
    virtual QString version() const;

protected:
    QJsonObject *metaData;
};

#define VLYC_PLUGIN_HEAD(ID) public:\
    virtual QString id() const { return QStringLiteral(#ID); }

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Vlyc::Plugin, "me.sodimm.oro.vlyc.Plugin/2.0")
QT_END_NAMESPACE

#endif // VLYCPLUGIN_H
