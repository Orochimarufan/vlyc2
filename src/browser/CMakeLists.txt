#/*****************************************************************************
# * vlyc2 - A Desktop YouTube client
# * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
# *
# * This library is free software: you can redistribute it and/or modify
# * it under the terms of the GNU Lesser General Public License as published
# * by the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This library is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# * GNU Lesser General Public License for more details.
# *
# * You should have received a copy of the GNU Lesser General Public License
# * along with this library. If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/
#/* NOTE: the browser is modeled after the "Tab Browser" example found in the Qt
# * documentation, available under GPLv3 */

SET(BROWSER_CPP
browser/browser.cpp

browser/networkaccessmanager.cpp

browser/browserwindow.cpp
browser/navigationbar.cpp

browser/tabbar.cpp
browser/tabwidget.cpp

browser/webpage.cpp
browser/webview.cpp
)

SET(BROWSER_HPP
browser/browser.h

browser/networkaccessmanager.h

browser/browserwindow.h
browser/navigationbar.h

browser/tabbar.h
browser/tabwidget.h

browser/webpage.h
browser/webview.h
)

SET(VLYC_BROWSER ${BROWSER_CPP} ${BROWSER_HPP} PARENT_SCOPE)
