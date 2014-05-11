#ifndef PYTHONPLUGIN_H
#define PYTHONPLUGIN_H

#include <VlycLoaderPlugin.h>
#include <VlycToolPlugin.h>

#include "VlycPython.h"

namespace Vlyc {
namespace Python {

class PythonPlugin : public QObject, public LoaderPlugin, public ToolPlugin
{
    Q_OBJECT
    Q_INTERFACES(Vlyc::Plugin)
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.Plugin/2.0" FILE "python.json")
    VLYC_PLUGIN_HEAD(me.sodimm.oro.vlyc.Python)

    PythonPlugin();
    virtual void init(InitEvent &init);

    // ----------------------------------------------------------------------------
    // LoaderPlugin
    /// Get the PluginLoader instance for fileName. Return nullptr if unapplicable.
    virtual PluginLoader *loaderFor(const QString &fileName);

    /// A LoaderPlugin may bring builtin plugins.
    virtual QList<PluginLoader *> builtinPlugins();

    // ----------------------------------------------------------------------------
    // ToolPlugin
    /// Get the tool menu entry
    virtual QAction *toolMenuAction();

private:
    VlycPython state;

private Q_SLOTS:
    void openPythonConsole();
};

}
}

#endif // PYTHONPLUGIN_H
