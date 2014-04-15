#include "PluginLoader.h"
#include "pythonplugin.h"

namespace Vlyc {
namespace Python {

PythonPluginLoader::PythonPluginLoader(VlycPython *state, QFileInfo f):
    mp_state(state), m_file(f)
{
}

LoaderPlugin *PythonPluginLoader::loaderPlugin()
{
    return mp_state->plugin();
}

QString PythonPluginLoader::fileName()
{
    return m_file.absoluteFilePath();
}

QJsonObject *PythonPluginLoader::metaData()
{
    return &m_metadata;
}

QString PythonPluginLoader::errorString()
{
    return m_errorString;
}

Plugin *PythonPluginLoader::instance()
{
    if (!isLoaded())
        if (!load())
            return nullptr;
    return mp_instance;
}

bool PythonPluginLoader::load()
{
    return false;
}

bool PythonPluginLoader::isLoaded()
{
    return mp_instance != nullptr;
}

bool PythonPluginLoader::unload()
{
    return false;
}

} // namespace Python
} // namespace Vlyc
