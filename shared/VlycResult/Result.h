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

#include "VlycMem/Object.h"
#include "VlycMem/Pointer.h"

namespace Vlyc {
namespace Result {

/**
 * @brief The Result class
 * The baseclass for all result types
 */
class Result : public Vlyc::Memory::Object
{
};

template <class T>
using ResultPointer = Vlyc::Memory::Pointer<T>;

/**
 * @brief ResultPtr
 * A pointer to a base Result object
 */
using ResultPtr = ResultPointer<Result>;

}
} // namespace Vlyc

// Qt Metatype declaration
#include <QtCore/QMetaType>
Q_DECLARE_METATYPE(Vlyc::Memory::Pointer<Vlyc::Result::Result>)
