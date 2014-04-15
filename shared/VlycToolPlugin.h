#ifndef VLYC_VLYCTOOLPLUGIN_H
#define VLYC_VLYCTOOLPLUGIN_H

#include "VlycPlugin.h"

class QAction;

namespace Vlyc {

/**
 * @brief The ToolPlugin class
 * A vlyc2 plugin that adds an entry to the tool menu
 */
class ToolPlugin : public virtual Plugin
{
public:
    /// Get the tool menu entry
    virtual QAction *toolMenuAction() = 0;
};

} // namespace Vlyc

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Vlyc::ToolPlugin, "me.sodimm.oro.vlyc.ToolPlugin/1.0")
QT_END_NAMESPACE

#endif // VLYC_VLYCTOOLPLUGIN_H
