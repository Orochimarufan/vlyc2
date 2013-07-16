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

#include "navigationbar.h"
#include "browserwindow.h"
#include "webview.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>

NavigationBar::NavigationBar(BrowserWindow *parent) :
    QWidget(parent),
    mp_window(parent)
{
    auto layout = new QHBoxLayout(this);

    auto back = new QToolButton(this);
    back->setText("Back");
    connect(back, SIGNAL(clicked()), SLOT(back()));
    layout->addWidget(back);

    auto fwd = new QToolButton(this);
    fwd->setText("Forward");
    connect(fwd, SIGNAL(clicked()), SLOT(fwd()));
    layout->addWidget(fwd);

    auto reload = new QToolButton(this);
    reload->setText("Reload");
    connect(reload, SIGNAL(clicked()), SLOT(reload()));
    layout->addWidget(reload);

    mp_line = new QLineEdit(this);
    mp_line->setText(parent->currentTab()->url().toString());
    connect(mp_line, SIGNAL(returnPressed()), SLOT(go()));
    layout->addWidget(mp_line);

    auto go = new QToolButton(this);
    go->setText("Go!");
    connect(go, SIGNAL(clicked()), SLOT(go()));
    layout->addWidget(go);
}

void NavigationBar::urlChanged(const QString &new_url)
{
    mp_line->setText(new_url);
}

void NavigationBar::go()
{
    mp_window->currentTab()->setUrl(QUrl(mp_line->text()));
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
