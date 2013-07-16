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
/* NOTE: the browser is modeled after the "Tab Browser" example found in the Qt
 * documentation, available under GPLv3 */

#include "browserwindow.h"

#include <QtCore/QUrl>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QVBoxLayout>

#include "tabwidget.h"
#include "webview.h"
#include "navigationbar.h"

#ifdef Q_OS_LINUX
#include "vlyc_xcb.h"
#endif

BrowserWindow::BrowserWindow(Browser *browser, QWidget *parent) :
    QMainWindow(parent), mp_browser(browser), mp_body(new TabWidget(browser))
{
#ifdef Q_OS_LINUX
    XCB::setWMClass(winId(), "vlyc2", "vlyc2");
#endif
    setWindowTitle(BROWSER_TITLE_SFX);

    setupMenu();

    mp_body->newTab("about:blank");
    mp_navi = new NavigationBar(this);

    auto central = new QWidget(this);
    auto layout = new QVBoxLayout(central);
    layout->addWidget(mp_navi);
    layout->addWidget(mp_body);
    setCentralWidget(central);

    connect(mp_body, SIGNAL(activeIconChanged(QIcon)), SLOT(_updateWindowIcon(QIcon)));
    connect(mp_body, SIGNAL(activeTitleChanged(QString)), SLOT(_updateWindowTitle(QString)));
}

BrowserWindow::~BrowserWindow()
{
}

void BrowserWindow::setupMenu()
{
    auto menubar = new QMenuBar(this);

    auto menu = menubar->addMenu("&File");
    auto action = menu->addAction("Open URL");
    connect(action, SIGNAL(triggered()), SLOT(_openUrl()));
    menu->addSeparator();
    action = menu->addAction("Close");
    connect(action, SIGNAL(triggered()), SLOT(close()));

    setMenuBar(menubar);
}

// private slots
/*void BrowserWindow::_loadProgress(int)
{
}

void BrowserWindow::_updateStatusBar(const QString &)
{
}*/

void BrowserWindow::_updateWindowTitle(const QString &title)
{
    setWindowTitle(title + QStringLiteral(" - ") + BROWSER_TITLE_SFX);
}

void BrowserWindow::_updateWindowIcon(const QIcon &icon)
{
    setWindowIcon(icon);
}

// private ui slots
void BrowserWindow::_openUrl()
{
    QString url;
    if (url.isNull())
        url = QInputDialog::getText(this, "enter url", "Enter new url");
    if (url.isNull())
        return;
    currentTab()->setUrl(QUrl(url));
}

// new tab
WebView *BrowserWindow::newTab(bool takeFocus)
{
    return mp_body->newTab(takeFocus);
}

WebView *BrowserWindow::newTab(const QUrl &url, bool takeFocus)
{
    return mp_body->newTab(url, takeFocus);
}

// stuff
WebView *BrowserWindow::currentTab() const
{
    return mp_body->currentTab();
}

TabWidget *BrowserWindow::tabs() const
{
    return mp_body;
}

// slots
void BrowserWindow::home()
{
}

void BrowserWindow::reload()
{
    mp_body->reloadTab();
}
