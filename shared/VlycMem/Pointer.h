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

#include "VlycConfig.h"

/**
 * like boost::shared_ptr::intrusive_ptr
 *
 * To use Vlyc::Memory::Pointer with a class T,
 * define Vlyc::Memory::incref(T *o) and Vlyc::Memory::decref(T *o).
 */

namespace Vlyc {
namespace Memory {

template <class Type>
class VLYC_EXPORT Pointer
{
    Type *mp_object;

    inline void set(Type *object)
    {
        if (object != nullptr)
            incref(object);
        if (mp_object != nullptr)
            decref(mp_object);
        mp_object = object;
    }

public:
    // Construct
    inline Pointer() :
        mp_object(nullptr)
    {
    }

    inline Pointer(const Pointer &ptr) :
        mp_object(ptr.mp_object)
    {
        if (mp_object != nullptr)
            incref(mp_object);
    }

    inline Pointer(Pointer &&ptr) :
        mp_object(ptr.mp_object)
    {
        ptr.mp_object = nullptr;
    }

    inline Pointer(Type *object) :
        mp_object(object)
    {
        if (mp_object != nullptr)
            incref(mp_object);
    }

    // Destroy
    ~Pointer()
    {
        if (mp_object != nullptr)
            decref(mp_object);
    }

    inline void release()
    {
        if (mp_object != nullptr)
            decref(mp_object);
        mp_object = nullptr;
    }

    // Assign
    inline Pointer &operator =(const Pointer &ptr)
    {
        set(ptr.mp_object);
        return *this;
    }

    inline Pointer &operator =(Pointer &&ptr)
    {
        // XXX: do we need to null out ptr before decrefing?
        if (mp_object != nullptr)
            decref(mp_object);
        mp_object = ptr.mp_object;
        ptr.mp_object = nullptr;
        return *this;
    }

    inline Pointer &operator =(Type *object)
    {
        set(object);
        return *this;
    }

    // Compare
    template <typename T>
    bool operator ==(const Pointer<T> &other) const
    {
        return static_cast<Type *>(other.mp_object) == mp_object;
    }

    inline bool operator ==(Type *object) const
    {
        return object == mp_object;
    }

    template <typename T>
    inline bool operator !=(const Pointer<T> &other) const
    {
        return static_cast<Type *>(other.mp_object) != mp_object;
    }

    inline bool operator !=(Type *object)
    {
        return object != mp_object;
    }

    template <typename T>
    inline bool operator <(const Pointer<T> &other) const
    {
        // Needed to store it in a map.
        return static_cast<Type *>(other.mp_object) > mp_object;
    }

    // Access
    inline Type *operator ->() const
    {
        return mp_object;
    }

    inline Type *get() const
    {
        return mp_object;
    }

    inline Type &operator *() const
    {
        return *mp_object;
    }

    // Cast
    template <typename T>
    inline Pointer<T> cast() const
    {
        return Pointer<T>(dynamic_cast<T*>(mp_object));
    }

    template <typename T>
    inline bool is() const
    {
        return dynamic_cast<T*>(mp_object) != nullptr;
    }

    inline bool isValid() const
    {
        return mp_object != nullptr;
    }
};

} // namespace Memory
} // namespace Vlyc

// STL Hash function
#include <functional>

namespace std {

template <class Type>
struct VLYC_EXPORT hash<Vlyc::Memory::Pointer<Type>> {
    typedef Vlyc::Memory::Pointer<Type> argument_type;
    typedef std::size_t value_type;

    value_type operator()(argument_type const& ptr) const
    {
        return std::hash<Type *>()(ptr.get());
    }
};

} // namespace std
