#ifndef TOOLPLUGIN_H
#define TOOLPLUGIN_H

#include "plugin.h"
#include <QtWidgets/QAction>

class VlycToolPlugin : public virtual VlycBasePlugin
{
public:
    virtual QAction *toolMenuAction() = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(VlycToolPlugin, "me.sodimm.oro.vlyc.ToolPlugin/1.0")
QT_END_NAMESPACE

#endif // TOOLPLUGIN_H
