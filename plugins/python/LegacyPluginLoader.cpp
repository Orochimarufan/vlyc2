#include "LegacyPluginLoader.h"

#include <QtCore/QDir>

#include <PythonQt/PythonQtConversion.h>
#include <PythonQt/PythonQtImporter.h>

#include "wrap/WrapLegacySitePlugin.h"
#include "pythonutil.h"

namespace Vlyc {
namespace Python {

LegacyPluginLoader::LegacyPluginLoader(VlycPython *state, QFileInfo f):
    mp_state(state), m_file(f), mp_instance(nullptr)
{
}

LoaderPlugin *LegacyPluginLoader::loaderPlugin()
{
    return (LoaderPlugin*)mp_state->plugin();
}

QString LegacyPluginLoader::fileName()
{
    return m_file.absoluteFilePath();
}

QJsonObject *LegacyPluginLoader::metaData()
{
    return &m_metadata;
}

QString LegacyPluginLoader::errorString()
{
    return m_errorString;
}

Plugin *LegacyPluginLoader::instance()
{
    if (!isLoaded())
        if (!load())
            return nullptr;
    return mp_instance;
}

PyObject *create_register_func(LegacyPluginLoader *l);

bool LegacyPluginLoader::load()
{
    // Generate module name
    static long pluginCnt = 0;
    PyObject *mod_name = PythonQtConv::QStringToPyObject(QStringLiteral("VLYC_PLUGIN_LE_%1_%2").arg(pluginCnt++).arg(m_file.baseName()));

    // Create Module
    PyObject *plugin = PyImport_AddModuleObject(mod_name); // borrowed ref

    // Initialize Module
    QString source = m_file.absoluteFilePath();

    if (m_file.isDir())
    {
        PyObject *package_path = PyTuple_New(1);
        PyTuple_SET_ITEM(package_path, 0, PythonQtConv::QStringToPyObject(source));
        PyModule_AddObject(plugin, "__path__", package_path); // steal package_path ref

        source = QDir(source).absoluteFilePath("__init__.py");
    }

    PyModule_AddObject(plugin, "__file__", PythonQtConv::QStringToPyObject(source));

    // Prepare Environment
    PyObject *func = create_register_func(this);
    PyObject *stdi = PyModule_GetDict(mp_state->state()); // borrowed ref
    PyDict_SetItemString(stdi, "register_site", func);
    Py_DECREF(func);

    PyObject *dict = PyModule_GetDict(plugin);
    PyObject *builtins = PyImport_ImportModule("builtins");
    PyDict_SetItemString(dict, "__builtins__", builtins);
    Py_DECREF(builtins);

    // Load File
    PyObject *code = PythonQtImport::getCodeFromPyc(source);

    if (code)
    {
        // Evaluate code
        PyObject *result = PyEval_EvalCode(code, dict, dict);

        // Clean Up
        Py_XDECREF(result);
        Py_DECREF(code);

        PyDict_DelItemString(stdi, "register_site");

        if (!PyErr_Occurred())
        {
            Py_DECREF(mod_name);
            return true;
        }
    }

    // Error Handling:
    if (PyErr_Occurred())
    {
        m_errorString = format_exc();
        if (m_errorString.isEmpty())
            m_errorString = "Unknown Exception occured.";
        else
        {
            std::cerr << "Exception occured while loading legacy python plugin "
                      << qPrintable(m_file.baseName()) << ":\n"
                      << qPrintable(m_errorString);
            std::cerr.flush();
        }
    }
    else
        m_errorString =  "Unknown Error occured";
    // Clean up
    PyDict_DelItem(PyImport_GetModuleDict(), mod_name);
    Py_DECREF(mod_name);
    PyErr_Clear();
    return false;
}

bool LegacyPluginLoader::isLoaded()
{
    return mp_instance != nullptr;
}

bool LegacyPluginLoader::unload()
{
    return false;
}

// ---------------------------------------------------------------------------
// Python callbacks
static char const *capsule_name = "vlyc2_LegacyPluginLoader_function_self";
static inline PyObject *capsule_create(LegacyPluginLoader *self)
{
    return PyCapsule_New(self, capsule_name, NULL);
}
static inline LegacyPluginLoader *capsule_get(PyObject *self)
{
    if (!PyCapsule_IsValid(self, capsule_name))
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid LegacyPluginLoader capsule");
        return NULL;
    }
    return (LegacyPluginLoader*)PyCapsule_GetPointer(self, capsule_name);
}

PyObject *f_register_func(PyObject *_self, PyObject *site)
{
    LegacyPluginLoader *loader = capsule_get(_self);
    if (!loader)
        return NULL;

    //qDebug("Registering python legacy site plugin");

    if (loader->mp_instance)
    {
        PyErr_SetString(PyExc_TypeError, "There can only be one plugin class per plugin.");
        return NULL;
    }

    Wrap::WrapLegacySitePlugin *plugin = Wrap::WrapLegacySitePlugin::create(site);
    if (!plugin)
    {
        PyErr_SetString(PyExc_RuntimeError, "Could not register legacy site");
        return NULL;
    }

    loader->mp_instance = plugin;
    Py_RETURN_NONE;
}

static PyMethodDef register_func {
    "register_site",    // name
    &f_register_func,   // PyCFunction
    METH_O,             // Flags
    NULL
};

PyObject *create_register_func(LegacyPluginLoader *l)
{
    PyObject *self = capsule_create(l);
    PyObject *func = PyCFunction_New(&register_func, self);
    Py_DECREF(self);
    return func;
}

} // namespace Python
} // namespace Vlyc
