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
#include "VlycResult/Result.h"

namespace Vlyc {

class UrlHandlerPlugin : public virtual Plugin
{
public:
    virtual Result::ResultPtr handleUrl(const QUrl &url) = 0;
};

} // namespace Vlyc

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Vlyc::UrlHandlerPlugin, "me.sodimm.oro.vlyc.UrlHandlerPlugin/1.0")
QT_END_NAMESPACE
