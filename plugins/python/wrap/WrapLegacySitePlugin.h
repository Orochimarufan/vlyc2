#ifndef VLYC_PYTHON_WRAP_WRAPLEGACYSITEPLUGIN_H
#define VLYC_PYTHON_WRAP_WRAPLEGACYSITEPLUGIN_H

#include "WrapPlugin.h"
#include <VlycLegacySitePlugin.h>

#include <video.h>

namespace Vlyc {
namespace Python {
namespace Wrap {

class WrapLegacySitePlugin : public virtual LegacySitePlugin
{
    PythonQtObjectPtr mo_plugin;
public:
    WrapLegacySitePlugin(PyObject *plugin);

    virtual void init(Vlyc::InitEvent &e);

    virtual QString id() const;
    virtual QString name() const;
    virtual QString author() const;
    virtual QString version() const;

    virtual QString forUrl(QUrl url);
    virtual VideoPtr video(QString video_id);

    static WrapLegacySitePlugin *create(PyObject *site);
};

class WrapLegacyVideo : public Video
{
    Q_OBJECT

    void emitError(const QString &message);
    QString ms_lastError;

    WrapLegacySitePlugin *mp_plugin;
    PythonQtObjectPtr mo_video;

    WrapLegacyVideo(WrapLegacySitePlugin *site, PyObject *video);

    PyObject *cb_error;
    PyObject *cb_done;
    PyObject *cb_media;
    PyObject *cb_subtitles;
    PyObject *self_capsule;

    static char const * capsule_name;
    static PyObject *capsule_create(WrapLegacyVideo *);
    static WrapLegacyVideo *capsule_get(PyObject *);

    static PyObject *f_error_func(PyObject *, PyObject *);
    static PyMethodDef f_error;

    static PyObject *f_done_func(PyObject *, PyObject *);
    static PyMethodDef f_done;

    static PyObject *f_media_func(PyObject *, PyObject *);
    static PyMethodDef f_media;

    static PyObject *f_subtitles_func(PyObject *, PyObject *);
    static PyMethodDef f_subtitles;

public:
    static WrapLegacyVideo *create(WrapLegacySitePlugin *site, PyObject *video);

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

    virtual ~WrapLegacyVideo();

    virtual QStringList getChildrenUrls() const;

public Q_SLOTS:
    virtual void load();
    virtual void getMedia(const VideoQualityLevel &q);
    virtual void getSubtitles(const QString &language);
};

} // namespace Wrap
} // namespace Python
} // namespace Vlyc

#endif // VLYC_PYTHON_WRAP_WRAPLEGACYSITEPLUGIN_H
