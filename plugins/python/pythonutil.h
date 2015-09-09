#pragma once

#include <Python.h>
#include <PythonQt/PythonQtConversion.h>


namespace Vlyc {
namespace Python {

/// Format a python exception like the runtime would print it
inline QString format_exception(PyObject *etype, PyObject *exception, PyObject *etrace)
{
    // Normalize
    PyErr_NormalizeException(&etype, &exception, &etrace);

    // Use traceback.format_exception
    PyObject *traceback = NULL, *format_exception = NULL, *cause = NULL;

    traceback = PyImport_ImportModule("traceback");
    if (traceback)
    {
        format_exception = PyObject_GetAttrString(traceback, "format_exception");
        Py_DECREF(traceback);
    }

    if (format_exception)
    {
        PyObject *args = PyTuple_Pack(3, etype, exception ? exception : Py_None, etrace ? etrace : Py_None);
        if (args)
        {
            cause = PyObject_CallObject(format_exception, args);
            Py_DECREF(args);
        }
        Py_DECREF(format_exception);
    }

    if (cause)
    {
        bool ok;
        QStringList lines = PythonQtConv::PyObjToStringList(cause, true, ok);
        Py_DECREF(cause);
        if (ok)
            return lines.join("");
    }

    return QString::null;
}

/// Format and clear the currently raised exception
/// @note also clears the exception, contrary to traceback.format_exc!
inline QString format_exc()
{
    QString result;
    PyObject *etype, *exception, *etrace;
    PyErr_Fetch(&etype, &exception, &etrace);
    if (etype)
        result = format_exception(etype, exception, etrace);
    Py_XDECREF(etype);
    Py_XDECREF(exception);
    Py_XDECREF(etrace);
    return result;
}

}
}

