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

#include "Object.h"

namespace Vlyc {
namespace Memory {

Object::Object() :
    refcount(0)
{
}

Object::~Object()
{
}

void incref(Object *object)
{
    object->refcount.fetch_add(1u, std::memory_order_acquire);
}

void decref(Object *object)
{
    if (object->refcount.fetch_sub(1u, std::memory_order_release) == 1u)
    {
        // TODO: Be more careful about memory ordering when deallocating?
        delete object;
    }
}

}
}
