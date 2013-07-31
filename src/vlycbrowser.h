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

#ifndef VLYCBROWSER_H
#define VLYCBROWSER_H

#include <browser/browser.h>
#include <browser/simplefilecookiejar.h>

class Vlyc;

class VlycBrowser : public Browser
{
    Q_OBJECT
public:
    explicit VlycBrowser(Vlyc *self);
    virtual ~VlycBrowser();

    virtual bool navigationRequest(QUrl);

private:
    Vlyc *mp_self;
    SimpleFileCookieJar cookies;
};

#endif // VLYCBROWSER_H
