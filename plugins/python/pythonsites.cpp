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

#include "pythonplugin.h"
#include "pythonsites.h"

#include <QtCore/QUrl>
#include <PythonQtConversion.h>

// Utilities

// escape const
template <class T>
inline T &nonconst(const T &in)
{
    return *((T *)&in);
}

static inline bool checkattr(PyObject *plugin, const char *attr)
{
    if (!PyObject_HasAttrString(plugin, attr))
    {
        PyErr_Format(PyExc_TypeError, "Plugin %R does not define member '%s'", plugin, attr);
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
        PyErr_Format(PyExc_TypeError, "Member '%s' of plugin %R has wrong type %R (should be %R)", attr, plugin, tp, type);
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
        PyErr_Format(PyExc_TypeError, "Member '%s' of plugin %R is not callable.", attr, plugin);
        Py_DECREF(it);
        return false;
    }
    Py_DECREF(it);

    return true;
}

// PythonSitePlugin
PythonSitePlugin *PythonSitePlugin::create(PyObject *plugin)
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
    PythonSitePlugin *r = new PythonSitePlugin(plugin);
    Py_DECREF(plugin);
    return r;
}

PythonSitePlugin::PythonSitePlugin(PyObject *plugin) :
    mo_plugin(plugin)
{
}

QString PythonSitePlugin::forUrl(QUrl url)
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

Video *PythonSitePlugin::video(QString video_id)
{
    PyObject *func = PyObject_GetAttrString(mo_plugin.object(), "video");
    PyObject *vidobj = PythonQtConv::QVariantToPyObject(video_id);
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
    PythonVideo *r = PythonVideo::create(this, video);
    Py_DECREF(video);
    return r;
}

QString PythonSitePlugin::name() const
{
    return nonconst(mo_plugin).getVariable("name").toString();
}

QString PythonSitePlugin::author() const
{
    return nonconst(mo_plugin).getVariable("author").toString();
}

int PythonSitePlugin::rev() const
{
    return nonconst(mo_plugin).getVariable("rev").toInt();
}


// PythonVideo
PythonVideo *PythonVideo::create(PythonSitePlugin *site, PyObject *video)
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
    PythonVideo *r = new PythonVideo(site, video);
    Py_DECREF(video);
    return r;
}

QString PythonVideo::videoId() const
{
    return nonconst(mo_video).getVariable("videoId").toString();
}

QString PythonVideo::title() const
{
    return nonconst(mo_video).getVariable("title").toString();
}

QString PythonVideo::author() const
{
    return nonconst(mo_video).getVariable("author").toString();
}

QString PythonVideo::description() const
{
    return nonconst(mo_video).getVariable("description").toString();
}

int PythonVideo::views() const
{
    return nonconst(mo_video).getVariable("views").toInt();
}

int PythonVideo::likes() const
{
    return nonconst(mo_video).getVariable("likes").toInt();
}

int PythonVideo::dislikes() const
{
    return nonconst(mo_video).getVariable("dislikes").toInt();
}

int PythonVideo::favorites() const
{
    return nonconst(mo_video).getVariable("favorited").toInt();
}

bool PythonVideo::useFileMetadata() const
{
    return nonconst(mo_video).getVariable("useFileMetadata").toBool();
}

QString PythonVideo::getError() const
{
    return ms_lastError;
}

void PythonVideo::emitError(const QString &message)
{
    ms_lastError = message;
    emit error(message);
}

// PyCFunction stuffs
char const *PythonVideo::capsule_name = "vlyc2_PythonVideo_function_self";
inline PyObject *PythonVideo::capsule_create(PythonVideo *self)
{
    return PyCapsule_New(self, capsule_name, NULL);
}
inline PythonVideo *PythonVideo::capsule_get(PyObject *self)
{
    if (!PyCapsule_IsValid(self, capsule_name))
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid PythonVideo capsule");
        return NULL;
    }
    return (PythonVideo*)PyCapsule_GetPointer(self, capsule_name);
}

PyObject *PythonVideo::f_error_func(PyObject *_self, PyObject *args)
{
    PythonVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    qDebug("error");

    PyObject *message;
    if (!PyArg_ParseTuple(args, "U:throw", &message))
    {
        self->emitError("Python: Wrongly called throw() callback!");
        return NULL;
    }

    self->emitError(PythonQtConv::PyObjGetString(message));

    Py_RETURN_NONE;
}
PyMethodDef PythonVideo::f_error {
    "throw",
    (PyCFunction) &PythonVideo::f_error_func,
    METH_VARARGS,
    "The Video method throw(str) callback"
};

PyObject *PythonVideo::f_done_func(PyObject *_self, PyObject *args)
{
    PythonVideo *self;
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
PyMethodDef PythonVideo::f_done {
    "done",
    (PyCFunction) &PythonVideo::f_done_func,
    METH_VARARGS,
    "The Video method done() callback"
};

PyObject *PythonVideo::f_media_func(PyObject *_self, PyObject *args)
{
    PythonVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    qDebug("media");

    int q;
    PyObject *descobj, *urlobj;
    if (!PyArg_ParseTuple(args, "iUU:media", &q, &descobj, &urlobj))
    {
        self->emitError("Python: Wrongly called media(int quality, str description, str url) callback.");
        return NULL;
    }

    QString desc(PythonQtConv::PyObjGetString(descobj));
    QUrl url(PythonQtConv::PyObjGetString(urlobj));

    emit self->media(VideoMedia{self, VideoQuality{(VideoQualityLevel)q, desc}, url});

    Py_RETURN_NONE;
}
PyMethodDef PythonVideo::f_media {
    "media",
    (PyCFunction) & PythonVideo::f_media_func,
    METH_VARARGS,
    "The Video method media(int quality, str description, str url) callback."
};

PyObject *PythonVideo::f_subtitles_func(PyObject *_self, PyObject *args)
{
    PythonVideo *self;
    if (!(self = capsule_get(_self)))
        return NULL;

    qDebug("subtitles");

    char *language, *type;
    PyObject *data;
    if (!PyArg_ParseTuple(args, "esesO:subtitles", "utf-8", &language, "utf-8", &type, &data))
    {
        self->emitError("Python: Wrongly called subtitles(str language, str type, object data) callback.");
        return NULL;
    }

    emit self->subtitles(VideoSubtitles{self, QString(language), QString(type), PythonQtConv::PyObjToQVariant(data)});

    PyMem_Free(data);

    Py_RETURN_NONE;
}
PyMethodDef PythonVideo::f_subtitles {
    "subtitles",
    (PyCFunction) &PythonVideo::f_subtitles_func,
    METH_VARARGS,
    "The Video method subtitles subtitles(str language, str type, bool isUrl, str|bytes data) callback."
};

// use of the PyCFunction stuff
PythonVideo::PythonVideo(PythonSitePlugin *site, PyObject *video) :
    mo_video(video), mp_plugin(site)
{
    self_capsule = capsule_create(this);
    cb_error = PyCFunction_New(&f_error, self_capsule);
    cb_done = PyCFunction_New(&f_done, self_capsule);
    cb_media = PyCFunction_New(&f_media, self_capsule);
    cb_subtitles = PyCFunction_New(&f_subtitles, self_capsule);
}

SitePlugin *PythonVideo::site() const
{
    return (SitePlugin*)mp_plugin;
}

PythonVideo::~PythonVideo()
{
    Py_DECREF(cb_error);
    Py_DECREF(cb_done);
    Py_DECREF(cb_media);
    Py_DECREF(cb_subtitles);
    Py_DECREF(self_capsule);
}

void PythonVideo::load()
{
    QVariantList args;
    args << PythonQtConv::PyObjToQVariant(cb_done);
    args << PythonQtConv::PyObjToQVariant(cb_error);

    mo_video.call("load", args);

    if (PyErr_Occurred())
    {
        PyObject *a, *b, *c;
        PyErr_Fetch(&a, &b, &c);
        emitError(PythonQtConv::PyObjGetString(b));
        Py_DECREF(a);
        Py_DECREF(b);
        Py_DECREF(c);
    }
}

void PythonVideo::getMedia(const VideoQualityLevel &q)
{
    QVariantList args;
    args << QVariant::fromValue<int>((int)q);
    args << PythonQtConv::PyObjToQVariant(cb_media);
    args << PythonQtConv::PyObjToQVariant(cb_error);

    mo_video.call("getMedia", args);

    if (PyErr_Occurred())
    {
        PyObject *a, *b, *c;
        PyErr_Fetch(&a, &b, &c);
        emitError(PythonQtConv::PyObjGetString(b));
        Py_DECREF(a);
        Py_DECREF(b);
        Py_DECREF(c);
    }
}

void PythonVideo::getSubtitles(const QString &language)
{
    QVariantList args;
    args << language;
    args << PythonQtConv::PyObjToQVariant(cb_subtitles);
    args << PythonQtConv::PyObjToQVariant(cb_error);

    mo_video.call("getSubtitles", args);

    if (PyErr_Occurred())
    {
        PyObject *a, *b, *c;
        PyErr_Fetch(&a, &b, &c);
        emitError(PythonQtConv::PyObjGetString(b));
        Py_DECREF(a);
        Py_DECREF(b);
        Py_DECREF(c);
    }
}

QStringList PythonVideo::availableSubtitleLanguages() const
{
    return nonconst(mo_video).getVariable("availableSubtitleLanguages").toStringList();
}

QList<VideoQuality> PythonVideo::availableQualities() const
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
