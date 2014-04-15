#ifndef VLYC_PYTHON_LEGACYPLUGINLOADER_H
#define VLYC_PYTHON_LEGACYPLUGINLOADER_H

#include <QtCore/QFileInfo>

#include <VlycLoaderPlugin.h>

#include "VlycPython.h"
#include "wrap/WrapPlugin.h"

namespace Vlyc {
namespace Python {

class LegacyPluginLoader : public Vlyc::PluginLoader
{
    VlycPython *mp_state;
    QFileInfo m_file;
    QJsonObject m_metadata;
    Plugin *mp_instance;
    QString m_errorString;

    friend PyObject *f_register_func(PyObject *,PyObject*);
    friend class RegScope;

public:
    LegacyPluginLoader(VlycPython *state, QFileInfo file);

    virtual LoaderPlugin *loaderPlugin();

    virtual QString fileName();
    virtual QJsonObject *metaData();
    virtual QString errorString();

    virtual Plugin *instance();

    virtual bool load();
    virtual bool isLoaded();
    virtual bool unload();
};

} // namespace Python
} // namespace Vlyc

#endif // VLYC_PYTHON_LEGACYPLUGINLOADER_H
