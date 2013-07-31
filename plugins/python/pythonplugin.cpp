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

#include <PythonQt/PythonQt.h>

#include "pythonplugin.h"
#include "pythonsites.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

static PyModuleDef VlycModuleDef = {
    PyModuleDef_HEAD_INIT,
    "vlyc",
    NULL,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

PythonPlugin::PythonPlugin(QObject *parent) :
    QObject(parent)
{
    PythonQt::init(PythonQt::RedirectStdOut);
    PythonQt *py = PythonQt::self();

    PyObject *module = PyModule_Create(&VlycModuleDef);
    PyObject *modules = PySys_GetObject("modules");
    PyDict_SetItemString(modules, VlycModuleDef.m_name, module);

    py->registerClass(&PythonVideoWrapper::staticMetaObject);
    py->registerClass(&PythonPluginRegistrar::staticMetaObject);

    connect(py, SIGNAL(pythonStdOut(QString)), SLOT(write_out(QString)));
    connect(py, SIGNAL(pythonStdErr(QString)), SLOT(write_err(QString)));

    PythonQt::self()->addObject(module, "registrar", &reg);
}

void PythonPlugin::write_out(QString s)
{
    puts(qPrintable(s));
}

void PythonPlugin::write_err(QString s)
{
    fputs(qPrintable(s), stderr);
}

PythonPlugin::~PythonPlugin()
{
}

bool PythonPlugin::canHandle(QString path)
{
    QFileInfo f(path);
    if (f.isFile() && f.isReadable())
        return path.endsWith(".py");
    else if (f.isDir())
        return QDir(path).exists("__init__.py");
    return false;
}

// PythonPluginRegistrar
PythonPluginRegistrar::PythonPluginRegistrar()
{
    reg = NULL;
}

struct RegScope
{
    PythonPluginRegistrar *reg;
    RegScope(PythonPluginRegistrar *reg, VlycForeignPluginRegistrar newReg)
    {
        this->reg = reg;
        reg->reg = newReg;
    }
    ~RegScope()
    {
        reg->reg = NULL;
    }
};

bool PythonPlugin::loadPlugin(QString path, VlycForeignPluginRegistrar registrar)
{
    static long pluginCnt = 0;
    static QString modfmt("VLYCPLUGIN%1_%2");
    QFileInfo f(path);
    RegScope r(&reg, registrar);
    if (f.isFile())
    {
        QStringList modParts = path.split("/").last().split(".");
        modParts.removeLast();
        QString mod = modfmt.arg(pluginCnt++).arg(modParts.join("_"));
        PythonQtObjectPtr module = PythonQt::self()->createModuleFromScript(mod);
        PythonQt::self()->evalFile(module.object(), path);
    }
    else
    {
        QString mod = modfmt.arg(pluginCnt++).arg(path.split("/").last().replace('/', '_').replace('.', '_'));
        QDir folder(path);
        PythonQtObjectPtr package = PythonQt::self()->createModuleFromScript(mod);
        QVariantList __path__;
        __path__ << path;
        package.addVariable("__path__", QVariant::fromValue(__path__));
        PythonQt::self()->evalFile(package, folder.absoluteFilePath("__init__.py"));
    }
    if(PythonQt::self()->hadError())
    {
        PyErr_Print();
        PyErr_Clear();
        return false;
    }
    return true;
}

void PythonPluginRegistrar::registerSite(QString name, QString author, int rev, PyObject *fn_forUrl, PyObject *fn_video)
{
    qDebug("Registering Python Site %s by %s", qPrintable(name), qPrintable(author));
    if(!reg)
    {
        PyErr_SetString(PyExc_RuntimeError, "the PythonPluginRegistrar instance can only be used while initializing the module!");
        return;
    }
    if(!PyCallable_Check(fn_forUrl) || !PyCallable_Check(fn_video))
    {
        PyErr_SetString(PyExc_TypeError, "Signature: registerSite(str name, str author, int rev, callable forUrl, callable loadVideo)");
        return;
    }
    reg(new PythonSitePlugin(name, author, rev, fn_forUrl, fn_video));
}