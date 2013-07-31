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
/* the browser is modeled after the "Tab Browser" example found in the Qt
 * documentation, available under GPLv3 */

#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>

class BrowserWindow;
class QLineEdit;

class NavigationBar : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationBar(BrowserWindow *parent);

private:
    BrowserWindow *mp_window;
    QLineEdit *mp_line;

public slots:
    void urlChanged(const QUrl &new_url);

private slots:
    void go();
    void back();
    void fwd();
    void reload();
};

#endif // NAVIGATIONBAR_H
