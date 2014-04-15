#include "WrapPlugin.h"

namespace Vlyc {
namespace Python {
namespace Wrap {

WrapPlugin::WrapPlugin(PyObject *plugin)
{
    mo_plugin.setNewRef(plugin);
    m_id = mo_plugin.getVariable("id").toString();
}

void WrapPlugin::init(InitEvent &init)
{
    Q_UNUSED(init);
}

QString WrapPlugin::id() const
{
    return m_id;
}

} // namespace Wrap
} // namespace Python
} // namespace Vlyc
