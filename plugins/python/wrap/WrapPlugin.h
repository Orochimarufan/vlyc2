#ifndef VLYC_PYTHON_WRAP_PLUGIN_H
#define VLYC_PYTHON_WRAP_PLUGIN_H

#include <PythonQt/PythonQt.h>

#include <VlycPlugin.h>

namespace Vlyc {
namespace Python {
namespace Wrap {

class WrapPlugin : public virtual Plugin
{
    QString m_id;

protected:
    PythonQtObjectPtr mo_plugin;

public:
    WrapPlugin(PyObject *plugin);

    virtual void init(InitEvent &init);
    virtual QString id() const;
};

} // namespace Wrap
} // namespace Python
} // namespace Vlyc

#endif // VLYC_PYTHON_WRAP_PLUGIN_H
