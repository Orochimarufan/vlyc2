/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
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

/* like boost::shared_ptr::intrusive_ptr
 *
 * To use Vlyc2Pointer with a class T, either
 * * make T inherit from Vlyc2Object or
 * * define Vlyc2Pointer_incref(T *o) and Vlyc2Pointer_decref(T *o).
 *
 * you can inherit from both QObject and Vlyc2Object:
 *  class T : public QObject, public Vlyc2Object
 * just make sure to put the QObject-derived class first.
 */

#ifndef VLYC2POINTER_H
#define VLYC2POINTER_H

#include <QtCore/QAtomicInt>

class Vlyc2Object
{
    QAtomicInt ref;
    friend void Vlyc2Pointer_incref(Vlyc2Object *o);
    friend void Vlyc2Pointer_decref(Vlyc2Object *o);
public:
    virtual ~Vlyc2Object();
};

void Vlyc2Pointer_incref(Vlyc2Object *o);
void Vlyc2Pointer_decref(Vlyc2Object *o);

template <typename T>
class Vlyc2Pointer
{
    T * mp_target;

public:
    Vlyc2Pointer() :
        mp_target(nullptr)
    {}

    Vlyc2Pointer(T *target) :
        mp_target(target)
    {
        if (mp_target)
            Vlyc2Pointer_incref(mp_target);
    }

    Vlyc2Pointer(const Vlyc2Pointer<T> &ptr) :
        mp_target(ptr.mp_target)
    {
        if (mp_target)
            Vlyc2Pointer_incref(mp_target);
    }

    ~Vlyc2Pointer()
    {
        if (mp_target)
            Vlyc2Pointer_decref(mp_target);
    }

    T *operator->() const
    {
        return mp_target;
    }

    Vlyc2Pointer<T> &operator=(T *target)
    {
        if (mp_target)
            Vlyc2Pointer_decref(mp_target);
        mp_target = target;
        if (mp_target)
            Vlyc2Pointer_incref(mp_target);
        return *this;
    }

    Vlyc2Pointer<T> &operator=(const Vlyc2Pointer<T> &ptr)
    {
        if (mp_target)
            Vlyc2Pointer_decref(mp_target);
        mp_target = ptr.mp_target;
        if (mp_target)
            Vlyc2Pointer_incref(mp_target);
        return *this;
    }

    bool operator==(const Vlyc2Pointer<T> &other) const
    {
        return mp_target == other.mp_target;
    }

    bool operator!=(const Vlyc2Pointer<T> &other) const
    {
        return mp_target != other.mp_target;
    }

    T *operator&() const
    {
        return mp_target;
    }

    T &operator*() const
    {
        return *mp_target;
    }
};

#endif // VLYC2POINTER_H
