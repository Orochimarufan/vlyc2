/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include "LinkContextMenu.h"
#include "webview.h"
#include "webpage.h"
#include "tabwidget.h"
#include "browser.h"
#include "browserwindow.h"

LinkContextMenu::LinkContextMenu(WebView *view, const QUrl &url) :
    QMenu(view), mp_view(view), m_url(url)
{
}

void LinkContextMenu::addAction(const QWebPage::WebAction &action)
{
    addAction(mp_view->pageAction(action));
}

void LinkContextMenu::addAction(const Action &action)
{
    switch(action) {
    case Open:
        addAction(QWebPage::OpenLink);
        break;
    case OpenNewTab:
        addAction(tr("Open in new Tab"), this, SLOT(openNewTab()));
        break;
    case OpenBackgroundTab:
        addAction(tr("Open in new background Tab"), this, SLOT(openBackgroundTab()));
        break;
    case OpenNewWindow:
        addAction(QWebPage::OpenLinkInNewWindow);
        break;
    case SaveLinkTarget:
        addAction(QWebPage::DownloadLinkToDisk);
        break;
    case CopyLinkToClipboard:
        addAction(QWebPage::CopyLinkToClipboard);
        break;
    case DeveloperInspectElement:
        if (mp_view->page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
            addAction(QWebPage::InspectElement);
        break;
    }
}

QUrl LinkContextMenu::url() const
{
    return m_url;
}

WebView *LinkContextMenu::tab() const
{
    return mp_view;
}

void LinkContextMenu::openNewTab()
{
    mp_view->tabs()->browser()->currentWindow()->newTab(m_url);
}

void LinkContextMenu::openBackgroundTab()
{
    mp_view->tabs()->browser()->currentWindow()->newTab(m_url, false);
}
