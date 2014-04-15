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

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>

class Browser;
class TabWidget;
class WebView;
class NavigationBar;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(Browser *browser, QWidget *parent=0);
    virtual ~BrowserWindow();

    WebView *currentTab() const;
    TabWidget *tabs() const;

    Browser *browser() const;

public slots:
    // work on currently active tab.
    void home();
    void reload();

    // control tabs
    WebView *newTab(bool takeFocus = true);
    WebView *newTab(const QUrl &url, bool takeFocus = true);

private slots:
    void _openUrl();
    void _updateWindowTitle(const QString &title = QString());
    void _updateWindowIcon(const QIcon &icon = QIcon());
    void _setAsHome();

private:
    void setupMenu();

private:
    Browser *mp_browser;
    TabWidget *mp_body;
    NavigationBar *mp_navi;
};

#endif // BROWSERWINDOW_H
