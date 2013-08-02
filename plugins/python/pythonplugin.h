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

#ifndef PYTHONPLUGIN_H
#define PYTHONPLUGIN_H

#include <PythonQt/PythonQt.h>

#include <QtCore/QObject>
#include <foreignplugin.h>

class PythonPluginRegistrar : public QObject
{
    Q_OBJECT
    friend class PythonPlugin;
    friend class RegScope;
    PythonPluginRegistrar();
    VlycForeignPluginRegistrar reg;
public Q_SLOTS:
    void registerSite(QString name, QString author, int rev, PyObject *fn_forUrl, PyObject *fn_video);
};

class PythonPlugin : public QObject, public VlycForeignPlugin
{
    Q_OBJECT
    Q_INTERFACES(VlycBasePlugin VlycForeignPlugin)
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.Plugin/1.0")
    PythonPluginRegistrar reg;
    VlycPluginInitializer initer;
public:
    PythonPlugin(QObject *parent=0);
    virtual ~PythonPlugin();

    virtual void initialize(VlycPluginInitializer init);

    virtual QString name() const { return "Python Interface"; }
    virtual QString author() const { return "Orochimarufan"; }
    virtual int rev() const { return 1; }

    virtual bool canHandle(QString path);
    virtual bool loadPlugin(QString path, VlycForeignPluginRegistrar registrar);

public Q_SLOTS:
    void write_out(QString s);
    void write_err(QString s);
};

#endif // PYTHONPLUGIN_H
