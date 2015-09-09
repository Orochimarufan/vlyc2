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
/* taken from https://github.com/yuyichao/qtcurve-qt5/blob/master/style/xcb_utils.h */
/***************************************************************************
 *   Copyright (C) 2013~2013 by Yichao Yu                                  *
 *   yyc1992@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifndef VLYC_XCB_H
#define VLYC_XCB_H

#include <xcb/xcb.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QWindow>

namespace XCB {

void setWMClass(WId id, QString name, QString x);

xcb_connection_t *_getConnection();

static inline xcb_connection_t* getConnection()
{
    static xcb_connection_t *conn = XCB::_getConnection();
    return conn;
}

static inline void flush()
{
    xcb_flush(XCB::getConnection());
}

void getAtoms(size_t n, xcb_atom_t *atoms, const char *const names[], bool create=false);

static inline xcb_atom_t getAtom(const char *name, bool create=false)
{
    xcb_atom_t atom;
    XCB::getAtoms(1, &atom, &name, create);
    return atom;
}

template<typename RetType, typename CookieType, typename... ArgTypes, typename... ArgTypes2>
static inline RetType* XcbCall(
        CookieType (*func)(xcb_connection_t*, ArgTypes...),
        RetType *(reply_func)(xcb_connection_t*, CookieType, xcb_generic_error_t**),
        ArgTypes2... args)
{
    xcb_connection_t *conn = XCB::getConnection();
    CookieType cookie = func(conn, args...);
    return reply_func(conn, cookie, 0);
}

#define XcbCall(name, args...) XCB::XcbCall(xcb_##name, xcb_##name##_reply, args)

template<typename... ArgTypes, typename... ArgTypes2>
static inline unsigned int XcbCallVoid(
        xcb_void_cookie_t (*func)(xcb_connection_t*, ArgTypes...),
        ArgTypes2... args)
{
    xcb_connection_t *conn = XCB::getConnection();
    return func(conn, args...).sequence;
}

#define XcbCallVoid(name, args...) XCB::XcbCallVoid(xcb_##name, args)

}


#endif // VLYC_XCB_H
