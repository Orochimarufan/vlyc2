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


namespace Vlyc {
namespace Util {

template <typename... Args>
class VariantArgs
{
    const QVariantList &args;

    // Determine Type of __i'th element
    template <std::size_t __i, typename... __Args>
    struct __T_at;

    template <std::size_t __i, typename __T, typename... __Tail>
    struct __T_at<__i, __T, __Tail...> : __T_at<__i - 1, __Tail...> {};

    template <typename __T, typename... __Tail>
    struct __T_at<0, __T, __Tail...>
    {
        typedef __T type;
    };

public:
    constexpr VariantArgs(const QVariantList &args) : args(args) {}

    static const std::size_t size = sizeof...(Args);

    template <std::size_t __i>
    struct element {
        typedef typename __T_at<__i, Args...>::type type;

        static inline constexpr type value(const QVariantList &args)
        {
            return qvariant_cast<type>(args.value(__i));
        }
    };

    template <std::size_t __i>
    inline constexpr typename element<__i>::type get()
    {
        return element<__i>::value(args);
    }
};

}
}

namespace std {

template <std::size_t __i, typename... Args>
constexpr typename Vlyc::Util::VariantArgs<Args...>::template element<__i>::type
    get(const Vlyc::Util::VariantArgs<Args...> &va)
{
    return va.get<__i>();
}

}
