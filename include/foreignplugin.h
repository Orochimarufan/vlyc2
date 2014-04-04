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

#ifndef FOREIGNPLUGIN_H
#define FOREIGNPLUGIN_H

#include <plugin.h>
#include <functional>

typedef std::function<bool(QObject*)> VlycForeignPluginRegistrar;

class VlycForeignPlugin : public virtual VlycBasePlugin
{
public:
    virtual ~VlycForeignPlugin() {}

    // you may not "try and see if it works" here.
    virtual bool canHandle(QString path) = 0;

    // may fail, but that will stop further tries on that file.
    virtual bool loadPlugin(QString path, VlycForeignPluginRegistrar registrar) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(VlycForeignPlugin, "me.sodimm.oro.vlyc.ForeignPlugin/1.0")
QT_END_NAMESPACE

#endif // FOREIGNPLUGIN_H
