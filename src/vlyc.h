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

#ifndef VLYC_H
#define VLYC_H

#include <QObject>

#include <video.h>

class VlycBrowser;
class MainWindow;
class PluginManager;

class Vlyc : public QObject
{
    Q_OBJECT
public:
    explicit Vlyc(QObject *parent = 0);

    VlycBrowser *browser() const;
    MainWindow *window() const;
    PluginManager *plugins() const;

    virtual ~Vlyc();

private:
    VlycBrowser *mp_browser;
    MainWindow *mp_window;
    PluginManager *mp_plugins;
};

#endif // VLYC_H
