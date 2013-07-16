/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
/* NOTE: the browser is modeled after the "Tab Browser" example found in the Qt
 * documentation, available under GPLv3 */

#include "tabwidget.h"

#include "tabbar.h"
#include "webview.h"

TabWidget::TabWidget(Browser *browser, QWidget *parent) :
    QTabWidget(parent),
    mp_browser(browser),
    mp_tabbar(new TabBar(this))
{
    setElideMode(Qt::ElideRight);

    connect(mp_tabbar, SIGNAL(newTab()), SLOT(newTab()));
    connect(mp_tabbar, SIGNAL(cloneTab(int)), SLOT(cloneTab(int)));
    connect(mp_tabbar, SIGNAL(closeTab(int)), SLOT(closeTab(int)));
    connect(mp_tabbar, SIGNAL(closeOtherTabs(int)), SLOT(closeOtherTabs(int)));
    connect(mp_tabbar, SIGNAL(reloadTab(int)), SLOT(reloadTab(int)));
    connect(mp_tabbar, SIGNAL(reloadAllTabs()), SLOT(reloadAllTabs()));
    connect(mp_tabbar, SIGNAL(tabMoved(int,int)), SLOT(moveTab(int,int)));
    setTabBar(mp_tabbar);
    setDocumentMode(true);
}

TabWidget::~TabWidget()
{
}

Browser *TabWidget::browser()
{
    return mp_browser;
}

WebView *TabWidget::tabAt(int index)
{
    return qobject_cast<WebView *>(widget(index));
}

WebView *TabWidget::currentTab() const
{
    return qobject_cast<WebView *>(currentWidget());
}

// new tab
WebView *TabWidget::newTab(bool takeFocus)
{
    auto tab = new WebView(this);
    connect(tab, SIGNAL(titleChanged(QString)), SLOT(webViewTitleChanged(QString)));
    connect(tab, SIGNAL(iconChanged()), SLOT(webViewIconChanged()));
    int i = addTab(tab, QStringLiteral("New Tab"));
    if (takeFocus) setCurrentIndex(i);
    return tab;
}

WebView *TabWidget::newTab(const QNetworkRequest &req, bool takeFocus)
{
    auto tab = newTab(takeFocus);
    tab->load(req);
    return tab;
}

WebView *TabWidget::newTab(const QUrl &url, bool takeFocus)
{
    auto tab = newTab(takeFocus);
    tab->load(url);
    return tab;
}

// title slots
void TabWidget::webViewTitleChanged(const QString &new_title)
{
    if (WebView *tab = qobject_cast<WebView *>(sender()))
    {
        setTabText(indexOf(tab), new_title);
        if (indexOf(tab) == currentIndex())
            emit activeTitleChanged(new_title);
    }
}

void TabWidget::webViewIconChanged()
{
    if (WebView *tab = qobject_cast<WebView *>(sender()))
    {
        setTabIcon(indexOf(tab), tab->icon());
        if (indexOf(tab) == currentIndex())
            emit activeIconChanged(tab->icon());
    }
}

void TabWidget::currentChanged(int index)
{
    auto tab = tabAt(index);
    emit activeIconChanged(tab->icon());
    emit activeTitleChanged(tab->title());
}

// Tab control
WebView *TabWidget::cloneTab(int index)
{
    if (index == -1) index = currentIndex();
    auto tab = newTab(false);
    tab->setUrl(tabAt(index)->url());
}

void TabWidget::closeTab(int index)
{
    if (index == -1) index = currentIndex();
    auto tab = tabAt(index);
    removeTab(index);
    delete tab;
}

void TabWidget::closeOtherTabs(int index)
{
    if (index == -1) index = currentIndex();
    for (int i = 0; i < index; i++)
        closeTab(i);
    for (int i = index + 1; i < count(); i++)
        closeTab(i);
}

void TabWidget::reloadTab(int index)
{
    if (index == -1) index = currentIndex();
    tabAt(index)->reload();
}

void TabWidget::reloadAllTabs()
{
    for (int i = 0; i < count(); i++)
        tabAt(i)->reload();
}

// move
void TabWidget::nextTabLeft()
{
    int current = currentIndex();
    if (current > 0)
        setCurrentIndex(current - 1);
}

void TabWidget::nextTabRight()
{
    int current = currentIndex();
    if (current < count())
        setCurrentIndex(current + 1);
}

void TabWidget::moveTab(int from, int to)
{
}
