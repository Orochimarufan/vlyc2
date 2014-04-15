#include "pythonplugin.h"
#include "LegacyPluginLoader.h"
#include "console/PythonQtScriptingConsole.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QAction>

namespace Vlyc {
namespace Python {

PythonPlugin::PythonPlugin() :
    state(this)
{
}

void PythonPlugin::init(InitEvent &init)
{
    Plugin::init(init);
    state.initState(&init);
}

// -----------------------------------------------------------------------
// Loader Plugin
PluginLoader *PythonPlugin::loaderFor(const QString &fileName)
{
    QFileInfo f(fileName);

    if (f.isFile() && f.isReadable() && f.suffix() == "py")
        return new LegacyPluginLoader(&state, f);

    return nullptr;
}

QList<PluginLoader *> PythonPlugin::builtinPlugins()
{
    return QList<PluginLoader *>();
}

// -----------------------------------------------------------------------
// Tool Plugin
QAction *PythonPlugin::toolMenuAction()
{
    QAction *tma = new QAction(tr("Python Console"), nullptr);
    connect(tma, &QAction::triggered, this, &PythonPlugin::openPythonConsole);
    return tma;
}

void PythonPlugin::openPythonConsole()
{
    static int num = 0;
    QByteArray name = QStringLiteral("VLYC_CONSOLE_%1").arg(num++).toUtf8();
    PyObject *module = PythonQt::self()->createModuleFromScript(name);
    PythonQtScriptingConsole *con = new PythonQtScriptingConsole(nullptr, module);
    con->setAttribute(Qt::WA_DeleteOnClose);
    con->show();
    Py_DECREF(module);
}

}
}
