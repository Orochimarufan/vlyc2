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

#include <QtCore/QAtomicInt>

namespace Vlyc {
namespace Result {

/**
 * @brief The Result class
 * The baseclass for all result types
 */
class Result
{
public:
    virtual ~Result();

    void incref();
    void decref();

private:
    QAtomicInt m_ref;
};

/**
 * @brief The ResultPointer<Result> class
 * Describes an intrusive pointer to a Result object.
 */
template<typename Result>
class ResultPointer
{
    Result *mp_result;

    inline void del()
    {
        if (mp_result != nullptr)
            mp_result->decref();
        mp_result = nullptr;
    }

    inline void set(Result *res)
    {
        if (res != nullptr)
            res->incref();
        mp_result = res;
    }

    inline void reset(Result *res)
    {
        if (res != nullptr)
            res->incref();
        del();
        mp_result = res;
    }

public:
    ResultPointer() :
        mp_result(nullptr)
    {
    }

    ResultPointer(const ResultPointer &ptr)
    {
        set(ptr.mp_result);
    }

    ResultPointer(Result *res)
    {
        set(res);
    }

    ~ResultPointer()
    {
        del();
    }

    ResultPointer &operator=(const ResultPointer &ptr)
    {
        reset(ptr.mp_result);
    }

    ResultPointer &operator=(Result *res)
    {
        reset(res);
    }

    template <typename T>
    bool operator== (const ResultPointer<T> &ptr)
    {
        return static_cast<Vlyc::Result::Result*>(ptr.mp_result) ==
                static_cast<Vlyc::Result::Result*>(mp_result);
    }

    Result *operator->()
    {
        return mp_result;
    }

    Result *get()
    {
        return mp_result;
    }

    Result &operator *()
    {
        return *mp_result;
    }

    template <typename T>
    ResultPointer<T> cast()
    {
        return ResultPointer<T>(dynamic_cast<T*>(mp_result));
    }

    template <typename T>
    bool is()
    {
        return dynamic_cast<T*>(mp_result) != nullptr;
    }

    bool isValid()
    {
        return mp_result != nullptr;
    }
};

/**
 * @brief ResultPtr
 * A pointer to a base Result object
 */
typedef ResultPointer<Result> ResultPtr;

}
} // namespace Vlyc

