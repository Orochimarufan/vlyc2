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

#ifndef PYTHONSITES_H
#define PYTHONSITES_H

#include <PythonQt.h>

#include <siteplugin.h>

class PythonSitePlugin : public QObject, public SitePlugin
{
    Q_OBJECT
    Q_INTERFACES(VlycBasePlugin SitePlugin)

    PythonQtObjectPtr mo_plugin;

    PythonSitePlugin(PyObject *plugin);

public:
    static PythonSitePlugin *create(PyObject *plugin);

    virtual QString forUrl(QUrl url);
    virtual VideoPtr video(QString video_id);

    virtual QString name() const;
    virtual QString author() const;
    virtual int rev() const;
};

class PythonVideo : public Video
{
    Q_OBJECT

    void emitError(const QString &message);
    QString ms_lastError;

    PythonSitePlugin *mp_plugin;
    PythonQtObjectPtr mo_video;

    PythonVideo(PythonSitePlugin *site, PyObject *video);

    PyObject *cb_error;
    PyObject *cb_done;
    PyObject *cb_media;
    PyObject *cb_subtitles;
    PyObject *self_capsule;

    static char const * capsule_name;
    static PyObject *capsule_create(PythonVideo *);
    static PythonVideo *capsule_get(PyObject *);

    static PyObject *f_error_func(PyObject *, PyObject *);
    static PyMethodDef f_error;

    static PyObject *f_done_func(PyObject *, PyObject *);
    static PyMethodDef f_done;

    static PyObject *f_media_func(PyObject *, PyObject *);
    static PyMethodDef f_media;

    static PyObject *f_subtitles_func(PyObject *, PyObject *);
    static PyMethodDef f_subtitles;

public:
    static PythonVideo *create(PythonSitePlugin *site, PyObject *video);

    virtual SitePlugin *site() const;

    virtual bool useFileMetadata() const;

    virtual QString videoId() const;
    virtual QString title() const;
    virtual QString author() const;
    virtual QString description() const;

    virtual int views() const;
    virtual int likes() const;
    virtual int dislikes() const;
    virtual int favorites() const;

    virtual QList<VideoQuality> availableQualities() const;
    virtual QStringList availableSubtitleLanguages() const;

    virtual bool mayBeDownloaded() const;

    virtual QString getError() const;

    virtual ~PythonVideo();

public Q_SLOTS:
    virtual void load();
    virtual void getMedia(const VideoQualityLevel &q);
    virtual void getSubtitles(const QString &language);
};

#endif // PYTHONSITES_H
