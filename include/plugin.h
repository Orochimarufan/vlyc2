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

#ifndef VLYC_BASE_PLUGIN_H
#define VLYC_BASE_PLUGIN_H

#include <QtPlugin>
#include <QObject>

class QNetworkAccessManager;

struct VlycPluginInitializer
{
    QNetworkAccessManager *network;
};

class VlycBasePlugin
{
public:
    virtual ~VlycBasePlugin() {}

    virtual void initialize(VlycPluginInitializer) {}

    virtual QString name() const = 0;
    virtual QString author() const = 0;
    virtual int rev() const = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(VlycBasePlugin, "me.sodimm.oro.vlyc.Plugin/1.0")
QT_END_NAMESPACE

#endif // VLYC_BASE_PLUGIN_H
