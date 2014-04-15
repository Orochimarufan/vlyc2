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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtWidgets/QTabWidget>

class WebView;
class QNetworkRequest;
class TabBar;
class Browser;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(Browser *browser, QWidget *parent = 0);
    virtual ~TabWidget();

    Browser *browser();

    WebView *tabAt(int index);
    WebView *currentTab() const;
    
signals:
    void activeTitleChanged(const QString &new_title);
    void activeIconChanged(const QIcon &new_icon);
    void activeUrlChanged(const QUrl &new_url);
    void activeTabChanged(WebView *new_tab);

public slots:
    WebView *newTab(bool takeFocus = true);
    WebView *newTab(const QNetworkRequest &req, bool takeFocus = true);
    WebView *newTab(const QUrl &url, bool takeFocus = true);

    WebView *cloneTab(int index = -1);
    void closeTab(int index = -1);
    void closeOtherTabs(int index = -1);
    void reloadTab(int index = -1);
    void reloadAllTabs();

    void nextTabLeft();
    void nextTabRight();

private slots:
    void onCurrentChanged(int index);

    void webViewIconChanged();
    void webViewTitleChanged(const QString &new_title);
    void webViewUrlChanged(const QUrl &new_url);
    void moveTab(int from, int to);

private:
    Browser *mp_browser;
    TabBar *mp_tabbar;
};

#endif // TABWIDGET_H
