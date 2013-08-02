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

#include "navigationbar.h"
#include "browserwindow.h"
#include "webview.h"
#include "browser.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>

NavigationBar::NavigationBar(BrowserWindow *parent) :
    QWidget(parent),
    mp_window(parent)
{
    auto layout = new QHBoxLayout(this);

    auto back = new QToolButton(this);
    back->setIcon(QIcon(":/browser/back"));
    back->setText("Back");
    connect(back, SIGNAL(clicked()), SLOT(back()));
    layout->addWidget(back);

    auto fwd = new QToolButton(this);
    fwd->setIcon(QIcon(":/browser/forward"));
    fwd->setText("Forward");
    connect(fwd, SIGNAL(clicked()), SLOT(fwd()));
    layout->addWidget(fwd);

    auto reload = new QToolButton(this);
    reload->setIcon(QIcon(":/browser/refresh"));
    reload->setText("Reload");
    connect(reload, SIGNAL(clicked()), SLOT(reload()));
    layout->addWidget(reload);

    auto home = new QToolButton(this);
    home->setIcon(QIcon(":/browser/home"));
    home->setText("Home");
    connect(home, SIGNAL(clicked()), SLOT(home()));
    layout->addWidget(home);

    mp_line = new QLineEdit(this);
    mp_line->setText(parent->currentTab()->url().toString());
    connect(mp_line, SIGNAL(returnPressed()), SLOT(go()));
    layout->addWidget(mp_line);

    auto go = new QToolButton(this);
    go->setIcon(QIcon(":/browser/go"));
    go->setText("Go!");
    connect(go, SIGNAL(clicked()), SLOT(go()));
    layout->addWidget(go);
}

void NavigationBar::urlChanged(const QUrl &new_url)
{
    mp_line->setText(new_url.toString());
}

void NavigationBar::go()
{
    QUrl url(mp_line->text());
    if (url.scheme().isEmpty())
        url.setScheme("http");
    mp_window->currentTab()->setUrl(url);
}

void NavigationBar::back()
{
    mp_window->currentTab()->triggerPageAction(QWebPage::Back);
}

void NavigationBar::fwd()
{
    mp_window->currentTab()->triggerPageAction(QWebPage::Forward);
}

void NavigationBar::reload()
{
    mp_window->currentTab()->triggerPageAction(QWebPage::Reload);
}

void NavigationBar::home()
{
    mp_window->currentTab()->setUrl(mp_window->browser()->homeUrl());
}
