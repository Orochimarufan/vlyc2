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

#include <QtCore/QVector>

#include "VlycConfig.h"
#include "Object.h"


namespace Vlyc {
namespace Result {

class VLYC_EXPORT ObjectList : public Object, public QVector<ResultPtr>
{
public:
    ObjectList();
    ObjectList(std::initializer_list<std::pair<QString, QVariant>> il);
    ObjectList(std::initializer_list<ResultPtr> il);
    ObjectList(std::initializer_list<std::pair<QString, QVariant>> il, std::initializer_list<ResultPtr> il2);
};

} // namespace Result
} // namespace Vlyc
