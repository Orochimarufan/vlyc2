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

#include "webview.h"

#include <QtGui/QWheelEvent>
#include <QtGui/QClipboard>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include <QtWebKitWidgets/QWebHitTestResult>

#include "webpage.h"
#include "tabwidget.h"
#include "LinkContextMenu.h"
#include "browser.h"

WebView::WebView(TabWidget *tabs) :
    QWebView(tabs),
    mp_tabs(tabs),
    mp_page(new WebPage(this)),
    mi_progress(0)
{
    setPage(mp_page);

    //connect(mp_page, SIGNAL(statusBarMessage(QString)), SLOT(setStatusBarText(QString)));
    //connect(this, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    //connect(this, SIGNAL(loadFinished(bool)), SLOT(loadFinished()));

    mp_page->setNetworkAccessManager(mp_tabs->browser()->network());
    mp_page->setForwardUnsupportedContent(true);
}

WebView::~WebView()
{
}

// accessors
WebPage *WebView::page() const
{
    return mp_page;
}

TabWidget *WebView::tabs() const
{
    return mp_tabs;
}

// events
void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (!r.linkUrl().isEmpty())
    {
        LinkContextMenu menu(this, r.linkUrl());
        mp_tabs->browser()->linkContextMenu(&menu);
        menu.exec(mapToGlobal(event->pos()));
        return;
    }

    QWebView::contextMenuEvent(event);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        setTextSizeMultiplier(textSizeMultiplier() + numSteps * 0.1);
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    mp_page->m_mouseButtons = event->buttons();
    mp_page->m_keyboardMods = event->modifiers();
    QWebView::mousePressEvent(event);
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
    QWebView::mouseReleaseEvent(event);
    if (!event->isAccepted() && (mp_page->m_mouseButtons & Qt::MidButton)) {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
            setUrl(url);
        }
    }
}
