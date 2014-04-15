#include "VlycPython.h"
#include "pythonqtdecorator.h"

#include <PythonQt/PythonQt_QtAll.h>

// TODO: make it public D:
#include "../../src/vlyc.h"

namespace Vlyc {
namespace Python {

namespace PyDef{
    static PyModuleDef VlycModule = {
        PyModuleDef_HEAD_INIT,
        "vlyc",
        NULL,//PyDoc_STR("The vlyc2 interface"),
        -1,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };

    static PyModuleDef StateModule = {
        PyModuleDef_HEAD_INIT,
        "_state",
        NULL,//PyDoc_STR("Contains the internal interface"),
        -1,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
}

// Initialize PythonQt
void VlycPython::initPythonQt()
{
    // initialize
    PythonQt::init(PythonQt::RedirectStdOut);
    PythonQt_QtAll::init();

    PythonQt::self()->addDecorators(new PythonQtDecorator());
    PythonQt::self()->installDefaultImporter();

    // Standard Streams
    connect(PythonQt::self(), &PythonQt::pythonStdOut, this, &VlycPython::py_stdout);
    connect(PythonQt::self(), &PythonQt::pythonStdErr, this, &VlycPython::py_stderr);
}

// Initialize the vlyc module
void VlycPython::initModules()
{
    // Create VLYC Module
    mo_module = PyModule_Create(&PyDef::VlycModule);

    PyObject *packagePath = PyTuple_New(1);
    PyTuple_SET_ITEM(packagePath, 0, PyUnicode_FromString(":/lib/vlyc"));
    PyModule_AddObject(mo_module, "__path__", packagePath);

    mo_state = PyModule_Create(&PyDef::StateModule);
    Py_INCREF(mo_state);
    PyModule_AddObject(mo_module, PyDef::StateModule.m_name, mo_state);

    // Install Module
    PyDict_SetItemString(PyImport_GetModuleDict(), PyDef::VlycModule.m_name, mo_module);
}

VlycPython::VlycPython(PythonPlugin *p) :
    mp_plugin(p)
{
    initPythonQt();
    initModules();

    // get rid of apport on Ubuntu!
    PyObject *excepthook = PySys_GetObject("__excepthook__");
    PySys_SetObject("excepthook", excepthook);
    Py_DECREF(excepthook);
}

VlycPython::~VlycPython()
{
    Py_DECREF(mo_module);
    Py_DECREF(mo_state);
}

void VlycPython::initState(InitEvent *e)
{
    QNetworkAccessManager *nam = ((VlycApp*)e->private_interface)->network();
    PyModule_AddObject(mo_state, "network_access_manager", PythonQt::priv()->wrapQObject(nam));
}

PythonPlugin *VlycPython::plugin()
{
    return mp_plugin;
}

PyObject *VlycPython::module()
{
    return mo_module;
}

PyObject *VlycPython::state()
{
    return mo_state;
}

// -----------------------------------------------------------------------
// Standard Streams
void VlycPython::py_stdout(const QString &s)
{
    std::cout << qPrintable(s);
    std::cout.flush();
}

void VlycPython::py_stderr(const QString &s)
{
    std::cerr << qPrintable(s);
    std::cerr.flush();
}

} // namespace Python
} // namespace Vlyc
