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

#include <atomic>

#include "Pointer.h"

namespace Vlyc {
namespace Memory {

/**
 * @brief The Object class
 * A base object class to inherit from.
 * does reference counting.
 */
class Object
{
    mutable std::atomic_uint refcount;

    friend void ::Vlyc::Memory::incref(Object *);
    friend void ::Vlyc::Memory::decref(Object *);

protected:
    Object();

public:
    virtual ~Object();
};

void incref(Object *);
void decref(Object *);

typedef Pointer<Object> ObjectPtr;

}
}
