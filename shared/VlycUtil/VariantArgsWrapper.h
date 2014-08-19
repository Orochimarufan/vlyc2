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

#include <QtCore/QVariant>

#include "VariantArgs.h"


namespace Vlyc {
namespace Util {

/**
 * Unpack a VariantArgs call
 * FIXME: HOW to unpack it?
 */
/*template <typename Ret, typename... Args>
class VariantArgsProxy
{
public:
    typedef std::function <Ret(Args...)> function;
    typedef VariantArgs<Args...> VArgs;

    VariantArgsProxy(function f) : func(f) {}

    Ret operator () (QVariantList args)
    {
        assert(args.length() == VArgs::size);
        return func(VArgs::element<...>::value(args));
    }

private:
    function func;
};*/

/**
 * Wrap a simple VariantArgs function
 */
template <typename Ret, typename... Args>
class VariantArgsWrapper
{
public:
    typedef std::function <QVariant(const QVariantList &)> function;

    VariantArgsWrapper(function f) : func(f) {}

    Ret operator() (Args... args)
    {
        return qvariant_cast<Ret>(func(QVariantList{args...}));
    }

private:
    function func;
};

}
}
