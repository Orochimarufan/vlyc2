#ifndef VLYC_PYTHON_VLYCPYTHON_H
#define VLYC_PYTHON_VLYCPYTHON_H

#include <QtCore/QObject>
#include <PythonQt/PythonQt.h>

#include <VlycPlugin.h>

namespace Vlyc {
namespace Python {

class PythonPlugin;

class VlycPython : public QObject
{
    Q_OBJECT

    PythonPlugin *mp_plugin;
    PyObject *mo_module;
    PyObject *mo_state;

public:
    VlycPython(PythonPlugin *p);
    ~VlycPython();

    PythonPlugin *plugin();
    PyObject *module();
    PyObject *state();

    void initState(InitEvent *e);

private:
    void initPythonQt();
    void initModules();

private Q_SLOTS:
    void py_stdout(const QString &s);
    void py_stderr(const QString &s);
};

} // namespace Python
} // namespace Vlyc

#endif // VLYC_PYTHON_VLYCPYTHON_H
