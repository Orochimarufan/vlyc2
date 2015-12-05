/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include "WrapLegacySitePlugin.h"

#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <PythonQt/PythonQt.h>
#include <PythonQt/PythonQtConversion.h>

namespace Vlyc {
namespace Python {
namespace Wrap {

static inline bool checkattr(PyObject *plugin, const char *attr)
{
    if (!PyObject_HasAttrString(plugin, attr))
    {
        qWarning("Plugin %s does not define member '%s'",
                 qPrintable(PythonQtConv::PyObjGetRepresentation(plugin)),
                 attr);
        //PyErr_Format(PyExc_TypeError, "Plugin %R does not define member '%s'", plugin, attr);
        return false;
    }
    return true;
}

static inline bool checkattrtype(PyObject *plugin, const char *attr, PyTypeObject *type)
{
    if (!checkattr(plugin, attr))
        return false;

    PyObject *it = PyObject_GetAttrString(plugin, attr);
    if (!PyObject_TypeCheck(it, type))
    {
        PyObject *tp = PyObject_Type(it);
        qWarning("Member '%s' of plugin %s has wrong type %s (expected %s)",
                 attr,
                 qPrintable(PythonQtConv::PyObjGetRepresentation(plugin)),
                 qPrintable(PythonQtConv::PyObjGetRepresentation(tp)),
                 type->tp_name);
        //PyErr_Format(PyExc_TypeError, "Member '%s' of plugin %R has wrong type %R (should be %R)", attr, plugin, tp, type);
        Py_DECREF(tp);
        Py_DECREF(it);
        return false;
    }
    Py_DECREF(it);

    return true;
}

static inline bool checkattrcallable(PyObject *plugin, const char *attr)
{
    if (!checkattr(plugin,attr))
        return false;

    PyObject *it = PyObject_GetAttrString(plugin, attr);
    if (!PyCallable_Check(it))
    {
        qWarning("Member '%s' of plugin %s is not callable",
                 attr,
                 qPrintable(PythonQtConv::PyObjGetRepresentation(plugin)));
        //PyErr_Format(PyExc_TypeError, "Member '%s' of plugin %R is not callable.", attr, plugin);
        Py_DECREF(it);
        return false;
    }
    Py_DECREF(it);

    return true;
}

// ----------------------------------------------------------------------------
// Plugin Wrapper
WrapLegacySitePlugin *WrapLegacySitePlugin::create(PyObject *plugin)
{
    Py_INCREF(plugin);
#define CHECKATTR(ATTR, TYPE) if (!checkattrtype(plugin, #ATTR, &TYPE)) return nullptr
    CHECKATTR(name, PyUnicode_Type);
    CHECKATTR(author, PyUnicode_Type);
    CHECKATTR(rev, PyLong_Type);
#undef CHECKATTR
#define CHECKATTR(ATTR) if (!checkattrcallable(plugin, #ATTR)) return nullptr;
    CHECKATTR(forUrl);
    CHECKATTR(video);
#undef CHECKATTR
    WrapLegacySitePlugin *r = new WrapLegacySitePlugin(plugin);
    //Py_DECREF(plugin);
    return r;
}

WrapLegacySitePlugin::WrapLegacySitePlugin(PyObject *plugin)
{
    mo_plugin.setNewRef(plugin);
}

QString WrapLegacySitePlugin::forUrl(QUrl url)
{
    PyObject *func = PyObject_GetAttrString(mo_plugin, "forUrl");

    PyObject *urlobj = PythonQtConv::QVariantToPyObject(url);
    PyObject *args = PyTuple_Pack(1, urlobj);
    PyObject *v = PyObject_CallObject(func, args);
    if (PyErr_Occurred())
    {
        PythonQt::self()->handleError();
        return QString();
    }
    if (v == Py_None)
    {
        Py_DECREF(Py_None);
        return QString();
    }
    else
        return PythonQtConv::PyObjGetString(v);
}

VideoPtr WrapLegacySitePlugin::video(QString video_id)
{
    PyObject *func = PyObject_GetAttrString(mo_plugin.object(), "video");
    PyObject *vidobj = PythonQtConv::QStringToPyObject(video_id);
    PyObject *args = PyTuple_Pack(1, vidobj);
    Py_DECREF(vidobj);
    PyObject *video = PyObject_CallObject(func, args);
    Py_DECREF(args);
    Py_DECREF(func);
    if (PyErr_Occurred())
    {
        PythonQt::self()->handleError();
        return nullptr;
    }
    if (video == Py_None)
    {
        Py_DECREF(Py_None);
        return nullptr;
    }
    WrapLegacyVideo *r = WrapLegacyVideo::create(this, video);
    Py_DECREF(video);
    return VideoPtr(r);
}

#define MO_PLUGIN (const_cast<PythonQtObjectPtr&>(this->mo_plugin))

QString WrapLegacySitePlugin::id() const
{
    PythonQtObjectPtr &p = MO_PLUGIN;
    QString id = p.getVariable("id").toString();
    if (!id.isEmpty())
        return id;
    else
        return p.getVariable("name").toString().prepend("me.sodimm.oro.vlyc.Python.LEGACY:");
}

QString WrapLegacySitePlugin::name() const
{
    return MO_PLUGIN.getVariable("name").toString();
}

QString WrapLegacySitePlugin::author() const
{
    return MO_PLUGIN.getVariable("author").toString();
}

QString WrapLegacySitePlugin::version() const
{
    return QStringLiteral("r%1").arg(MO_PLUGIN.getVariable("rev").toInt());
}

void WrapLegacySitePlugin::init(InitEvent &e)
{
    e.metadata->insert("ID", id());
    e.metadata->insert("Name", name());
    e.metadata->insert("Author", author());
    e.metadata->insert("Version", version());
    e.metadata->insert("Description", QStringLiteral("A Legacy python plugin"));
}


// ----------------------------------------------------------------------------
// Video Object Wrapper
WrapLegacyVideo *WrapLegacyVideo::create(WrapLegacySitePlugin *site, PyObject *video)
{
    Py_INCREF(video);
#define CHECKATTR(ATTR, TYPE) if (!checkattrtype(video, #ATTR, &TYPE)) return nullptr
    CHECKATTR(videoId, PyUnicode_Type);
#undef CHECKATTR
#define CHECKATTR(ATTR) if (!checkattrcallable(video, #ATTR)) return nullptr;
    CHECKATTR(load);
    CHECKATTR(getMedia);
    CHECKATTR(getSubtitles);
#undef CHECKATTR
    WrapLegacyVideo *r = new WrapLegacyVideo(site, video);
    Py_DECREF(video);
    return r;
}

#define MO_VIDEO (const_cast<PythonQtObjectPtr&>(this->mo_video))

QString WrapLegacyVideo::videoId() const
{
    return MO_VIDEO.getVariable("videoId").toString();
}

QString WrapLegacyVideo::title() const
{
    return MO_VIDEO.getVariable("title").toString();
}

QString WrapLegacyVideo::author() const
{
    return MO_VIDEO.getVariable("author").toString();
}

QString WrapLegacyVideo::description() const
{
    return MO_VIDEO.getVariable("description").toString();
}

int WrapLegacyVideo::views() const
{
    return MO_VIDEO.getVariable("views").toInt();
}

int WrapLegacyVideo::likes() const
{
    return MO_VIDEO.getVariable("likes").toInt();
}

int WrapLegacyVideo::dislikes() const
{
    return MO_VIDEO.getVariable("dislikes").toInt();
}

int WrapLegacyVideo::favorites() const
{
    return MO_VIDEO.getVariable("favorited").toInt();
}

bool WrapLegacyVideo::useFileMetadata() const
{
    return MO_VIDEO.getVariable("useFileMetadata").toBool();
}

bool WrapLegacyVideo::mayBeDownloaded() const
{
    return false; // TODO implement
}

QString WrapLegacyVideo::getError() const
{
    return ms_lastError;
}

void WrapLegacyVideo::emitError(const QString &message)
{
    ms_lastError = message;
    emit error(message);
}

// PyCFunction stuffs
char const *WrapLegacyVideo::capsule_name = "vlyc2_WrapLegacyVideo_function_self";
inline PyObject *WrapLegacyVideo::capsule_create(WrapLegacyVideo *self)
{
    return PyCapsule_New(self, capsule_name, NULL);
}
inline WrapLegacyVideo *WrapLegacyVideo::capsule_get(PyObject *self)
{
    if (!PyCapsule_IsValid(self, capsule_name))
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid WrapLegacyVideo capsule");
        return NULL;
    }
    return (WrapLegacyVideo*)PyCapsule_GetPointer(self, capsule_name);
}

PyObject *WrapLegacyVideo::f_error_func(PyObject *_self, PyObject *args)
{
    WrapLegacyVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    PyObject *message;
    if (!PyArg_ParseTuple(args, "U:throw", &message))
    {
        self->emitError("Python: Wrongly called throw() callback!");
        return NULL;
    }

    QString msg = PythonQtConv::PyObjGetString(message);
    Py_DECREF(message);

    qDebug("error: %s", qPrintable(msg));
    self->emitError(msg);

    Py_RETURN_NONE;
}
PyMethodDef WrapLegacyVideo::f_error {
    "throw",
    (PyCFunction) &WrapLegacyVideo::f_error_func,
    METH_VARARGS,
    "The Video method throw(str) callback"
};

PyObject *WrapLegacyVideo::f_done_func(PyObject *_self, PyObject *args)
{
    WrapLegacyVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    qDebug("done");

    if (!PyArg_ParseTuple(args, ":done"))
    {
        self->emitError("Python: Wrongly called done() callback.");
        return NULL;
    }

    emit self->done();

    Py_RETURN_NONE;
}
PyMethodDef WrapLegacyVideo::f_done {
    "done",
    (PyCFunction) &WrapLegacyVideo::f_done_func,
    METH_VARARGS,
    "The Video method done() callback"
};

PyObject *WrapLegacyVideo::f_media_func(PyObject *_self, PyObject *args)
{
    WrapLegacyVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    int q;
    PyObject *descobj, *urlobj;
    if (!PyArg_ParseTuple(args, "iUU:media", &q, &descobj, &urlobj))
    {
        self->emitError("Python: Wrongly called media(int quality, str description, str url) callback.");
        return NULL;
    }

    QString desc(PythonQtConv::PyObjGetString(descobj));
    QUrl url(PythonQtConv::PyObjGetString(urlobj));

    qDebug("media: %i '%s' at '%s'", q, qPrintable(desc), qPrintable(url.toString()));

    emit self->media(VideoMedia{VideoPtr(self), VideoQuality{(VideoQualityLevel)q, desc}, url});

    Py_RETURN_NONE;
}
PyMethodDef WrapLegacyVideo::f_media {
    "media",
    (PyCFunction) & WrapLegacyVideo::f_media_func,
    METH_VARARGS,
    "The Video method media(int quality, str description, str url) callback."
};

PyObject *WrapLegacyVideo::f_subtitles_func(PyObject *_self, PyObject *args)
{
    WrapLegacyVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    char *language, *type;
    PyObject *data;
    if (!PyArg_ParseTuple(args, "esesO:subtitles", "utf-8", &language, "utf-8", &type, &data))
    {
        self->emitError("Python: Wrongly called subtitles(str language, str type, object data) callback.");
        return NULL;
    }

    qDebug("subtitles: %s %s", language, type);

    emit self->subtitles(VideoSubtitles{VideoPtr(self), QString(language), QString(type), PythonQtConv::PyObjToQVariant(data)});

    Py_XDECREF(data);

    Py_RETURN_NONE;
}
PyMethodDef WrapLegacyVideo::f_subtitles {
    "subtitles",
    (PyCFunction) &WrapLegacyVideo::f_subtitles_func,
    METH_VARARGS,
    "The Video method subtitles subtitles(str language, str type, str|bytes data) callback."
};

// use of the PyCFunction stuff
WrapLegacyVideo::WrapLegacyVideo(WrapLegacySitePlugin *site, PyObject *video) :
    mp_plugin(site), mo_video(video)
{
    self_capsule = capsule_create(this);
    cb_error = PyCFunction_New(&f_error, self_capsule);
    cb_done = PyCFunction_New(&f_done, self_capsule);
    cb_media = PyCFunction_New(&f_media, self_capsule);
    cb_subtitles = PyCFunction_New(&f_subtitles, self_capsule);
}

SitePlugin *WrapLegacyVideo::site() const
{
    return (SitePlugin*)mp_plugin;
}

WrapLegacyVideo::~WrapLegacyVideo()
{
    Py_DECREF(cb_error);
    Py_DECREF(cb_done);
    Py_DECREF(cb_media);
    Py_DECREF(cb_subtitles);
    Py_DECREF(self_capsule);
}

#define PY if (!PyErr_Occurred())

inline static QString handleException()
{
    // Use the traceback module to generate a error message!
    QString message("<CAUSE UNKNOWN>");

    PyObject *tp, *exc, *tb, *exc_info;
    PyErr_Fetch(&tp, &exc, &tb);
    exc_info = PyTuple_Pack(3, tp, exc, tb);

    PyObject *traceback = NULL, *format_exception = NULL, *message_p = NULL;
    traceback = PyImport_ImportModule("traceback");
    if (traceback)
        format_exception = PyObject_GetAttrString(traceback, "format_exception");
    if (format_exception && exc_info)
        message_p = PyObject_CallObject(format_exception, exc_info);
    //if (traceback)
    //    format_exception = PyObject_GetAttrString(traceback, "format_exc");
    if (format_exception)
        message_p = PyObject_CallObject(format_exception, NULL);

    Py_XDECREF(format_exception);
    Py_XDECREF(traceback);

    if (message_p != NULL) {
        bool ok;
        QStringList message_parts = PythonQtConv::PyObjToStringList(message_p, true, ok);
        Py_DECREF(message_p);
        if (ok)
            message = message_parts.join(QString::null);
    } else if (tp != NULL && exc != NULL){
        message = QStringLiteral("[%1]: %2").arg(PythonQtConv::PyObjGetString(tp), PythonQtConv::PyObjGetString(exc));
    }

    Py_XDECREF(exc_info);

    PyErr_Clear();

    puts(qPrintable(message));
    return message;
}

void WrapLegacyVideo::load()
{
    PyObject *load, *args, *result;

    load = PyObject_GetAttrString(mo_video, "load");

    PY {
        Py_INCREF(cb_done);
        Py_INCREF(cb_error);
        args = PyTuple_Pack(2, cb_done, cb_error);

        PY result = PyObject_CallObject(load, args);

        Py_XDECREF(result);
        Py_XDECREF(args);
        Py_XDECREF(load);
    }

    if (PyErr_Occurred())
        emitError(handleException());
}

void WrapLegacyVideo::getMedia(const VideoQualityLevel &q)
{
    PyObject *getMedia, *args, *result;

    getMedia = PyObject_GetAttrString(mo_video, "getMedia");

    PY {
        Py_INCREF(cb_media);
        Py_INCREF(cb_error);
        args = PyTuple_Pack(3, PyLong_FromLong((int)q), cb_media, cb_error);

        PY result = PyObject_CallObject(getMedia, args);

        Py_XDECREF(result);
        Py_XDECREF(args);
        Py_XDECREF(getMedia);
    }

    if (PyErr_Occurred())
        emitError(handleException());
}

void WrapLegacyVideo::getSubtitles(const QString &language)
{
    PyObject *getSubtitles, *args, *result;

    getSubtitles = PyObject_GetAttrString(mo_video, "getSubtitles");

    PY {
        Py_INCREF(cb_subtitles);
        Py_INCREF(cb_error);
        args = PyTuple_Pack(3, PythonQtConv::QStringToPyObject(language), cb_subtitles, cb_error);

        PY result = PyObject_CallObject(getSubtitles, args);

        Py_XDECREF(result);
        Py_XDECREF(args);
        Py_XDECREF(getSubtitles);
    }

    if (PyErr_Occurred())
        emitError(handleException());
}

QStringList WrapLegacyVideo::availableSubtitleLanguages() const
{
    return MO_VIDEO.getVariable("availableSubtitleLanguages").toStringList();
}

QList<VideoQuality> WrapLegacyVideo::availableQualities() const
{
    QList<VideoQuality> list;
    PyObject *o = PyObject_GetAttrString(mo_video, "availableQualities");
    if (PyErr_Occurred())
    {
        PythonQt::self()->handleError();
        return list;
    }
    if (!PySequence_Check(o))
    {
        qWarning("Expected 'availableQualities' to be a list of 2-tuples. got something else");
        return list;
    }
    for (int i=0; i<PySequence_Length(o); ++i)
    {
        PyObject *x = PySequence_GetItem(o, i);
        if (PySequence_Check(x) && PySequence_Length(x) >= 2)
        {
            PyObject *first = PySequence_GetItem(x, 0);
            if (!PyLong_Check(first))
            {
                qWarning("Expected Quality tuple [0] to be int");
                Py_DECREF(first);
                Py_DECREF(x);
                continue;
            }
            PyObject *second = PySequence_GetItem(x, 1);
            if (!PyUnicode_Check(second))
            {
                qWarning("Expected Quality tuple [1] to be str");
                Py_DECREF(second);
                Py_DECREF(first);
                Py_DECREF(x);
                continue;
            }
            list << VideoQuality {
                (VideoQualityLevel) PyLong_AsLong(first),
                PythonQtConv::PyObjGetString(second)
            };
            Py_DECREF(second);
            Py_DECREF(first);
        }
        else
            qWarning("Expected 'availableQualitites' items to be 2-tuples");
        Py_DECREF(x);
    }
    Py_DECREF(o);
    return list;
}


} // namespace Wrap
} // namespace Python
} // namespace Vlyc
