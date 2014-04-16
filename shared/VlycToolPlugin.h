/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#pragma once

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
