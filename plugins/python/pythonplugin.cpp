/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2014 Orochimarufan <orochimarufan.x3@gmail.com>
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
#include <PythonQt/PythonQt_QtAll.h>

#include "pythonplugin.h"
#include "pythonsites.h"
#include "pythonqtdecorator.h"
#include "console/PythonQtScriptingConsole.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

// -----------------------------------------------------------------------
// Module Definitions
PyDoc_STRVAR(VlycModuleDoc, "Has all Vlyc options");

static PyModuleDef VlycModuleDef = {
    PyModuleDef_HEAD_INIT,
    "vlyc",
    VlycModuleDoc,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyObject *vlyc__state_register_site(PyObject *self, PyObject *site);
static PyMethodDef VlycStateMethods[] = {
  {"register_site", &vlyc__state_register_site, METH_O, NULL},
  {NULL, NULL, 0, NULL},
};

static PyModuleDef VlycStateModuleDef = {
    PyModuleDef_HEAD_INIT,
    "_state",
    NULL,
    -1,
    VlycStateMethods,
    NULL,
    NULL,
    NULL,
    NULL,
};

// -----------------------------------------------------------------------
// Initialization
PythonPlugin::PythonPlugin(QObject *parent) :
    QObject(parent)
{
    // initialize
    PythonQt::init(PythonQt::RedirectStdOut);
    PythonQt_QtAll::init();
    PythonQt *py = PythonQt::self();
    py->addDecorators(new PythonQtDecorator());

    // Standard Streams
    connect(py, &PythonQt::pythonStdOut, this, &PythonPlugin::write_out);
    connect(py, &PythonQt::pythonStdErr, this, &PythonPlugin::write_err);

    // Create VLYC Module
    module.setNewRef(PyModule_Create(&VlycModuleDef));

    QStringList __path__;
    __path__ << ":/lib/vlyc";
    module.addVariable("__path__", __path__);
    py->installDefaultImporter();

    state = PyModule_Create(&VlycStateModuleDef);
    PyModule_AddObject(module, VlycStateModuleDef.m_name, state);

    // Install Module
    PyObject *modules = PySys_GetObject("modules");
    PyDict_SetItemString(modules, VlycModuleDef.m_name, module);
    Py_DECREF(modules);

    // get rid of apport on Ubuntu!
    PyObject *excepthook = PySys_GetObject("__excepthook__");
    PySys_SetObject("excepthook", excepthook);
    Py_DECREF(excepthook);
}

void PythonPlugin::initialize(VlycPluginInitializer init)
{
    initer = init;
    state.addObject("network_access_manager", (QObject*)initer.network);
}

PythonPlugin::~PythonPlugin()
{
}

// -----------------------------------------------------------------------
// Standard Streams
void PythonPlugin::write_out(QString s)
{
    puts(qPrintable(s));
    fflush(stdout);
}

void PythonPlugin::write_err(QString s)
{
    fputs(qPrintable(s), stderr);
    fflush(stderr);
}

// -----------------------------------------------------------------------
// Plugin Registering
VlycForeignPluginRegistrar vlyc_register_plugin = nullptr;

PyObject *vlyc__state_register_site(PyObject *self, PyObject *site)
{
    Q_UNUSED(self);

    if (vlyc_register_plugin == nullptr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Cannot register plugin after initialization is done.");
        return NULL;
    }

    PythonSitePlugin *plugin = PythonSitePlugin::create(site);
    if (!plugin)
    {
        PyErr_SetString(PyExc_RuntimeError, "Could not register site plugin.");
        return NULL;
    }

    vlyc_register_plugin(plugin);
    Py_RETURN_NONE;
}

struct RegScope
{
    RegScope(VlycForeignPluginRegistrar reg)
    {
        vlyc_register_plugin = reg;
    }
    ~RegScope()
    {
        vlyc_register_plugin = nullptr;
    }
};

// -----------------------------------------------------------------------
// Plugin Loading
bool PythonPlugin::canHandle(QString path)
{
    QFileInfo f(path);
    //qDebug("Python test path: '%s'' | file(%i) readable(%i) dir(%i) hasinit(%i)", qPrintable(path), f.isFile(), f.isReadable(), f.isDir(), QDir(path).exists("__init__.py"));
    if (f.isFile() && f.isReadable())
        return path.endsWith(".py");
    else if (f.isDir())
        return QDir(path).exists("__init__.py");
    return false;
}

bool PythonPlugin::loadPlugin(QString path, VlycForeignPluginRegistrar registrar)
{
    QFileInfo f(path);

    // Generate module name
    static long pluginCnt = 0;
    static QString modfmt = QStringLiteral("VLYCPLUGIN%1_%2");

    QString name = path.split("/").last();
    if (f.isFile())
        name = name.section(".", -1);

    QString mod = modfmt.arg(pluginCnt++).arg(name);

    // Create Module
    PythonQtObjectPtr plugin_mod = PythonQt::self()->createModuleFromScript(mod);

    // Load Plugin
    RegScope r(registrar);
    if (f.isFile())
        plugin_mod.evalFile(path);
    else
    {
        QStringList __path__;
        __path__ << path;
        plugin_mod.addVariable("__path__", __path__);
        plugin_mod.evalFile(QDir(path).absoluteFilePath("__init__.py"));
    }

    if(PythonQt::self()->hadError())
    {
        PyErr_Print();
        PyErr_Clear();
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------
// Tool Menu Action
QAction *PythonPlugin::toolMenuAction()
{
    QAction *tma = new QAction(tr("Python Console"), nullptr);
    connect(tma, &QAction::triggered, this, &PythonPlugin::openConsole);
    return tma;
}

void PythonPlugin::openConsole()
{
    PythonQtObjectPtr module = PythonQt::self()->createUniqueModule();
    PythonQtScriptingConsole *con = new PythonQtScriptingConsole(nullptr, module);
    con->setAttribute(Qt::WA_DeleteOnClose);
    con->show();
}
