#ifndef VLYC_PYTHON_PLUGINLOADER_H
#define VLYC_PYTHON_PLUGINLOADER_H

#include <QtCore/QFileInfo>

#include <VlycLoaderPlugin.h>

#include "VlycPython.h"
#include "wrap/WrapPlugin.h"

namespace Vlyc {
namespace Python {

class PythonPluginLoader : public Vlyc::PluginLoader
{
    VlycPython *mp_state;
    QFileInfo m_file;
    QJsonObject m_metadata;
    Wrap::WrapPlugin *mp_instance;
    QString m_errorString;

public:
    PythonPluginLoader(VlycPython *state, QFileInfo file);

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

#endif // VLYC_PYTHON_PLUGINLOADER_H
