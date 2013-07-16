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

#include "webpage.h"

#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>

#include <QtWidgets/QStyle>

#include <QtWebKitWidgets/QWebFrame>

#include <QtNetwork/QNetworkReply>

#include "webview.h"
#include "tabwidget.h"
#include "browser.h"
#include "browserwindow.h"

WebPage::WebPage(WebView *view) :
    QWebPage(view),
    mp_view(view),
    m_keyboardMods(Qt::NoModifier),
    m_mouseButtons(Qt::NoButton),
    mb_openInTab(false)
{
    connect(this, SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(handleUnsupportedContent(QNetworkReply*)));
}

QWebPage *WebPage::createWindow(WebWindowType type)
{
    Q_UNUSED(type)
    if (m_keyboardMods & Qt::ControlModifier || m_mouseButtons == Qt::MidButton)
        mb_openInTab = true;

    if (mb_openInTab)
    {
        mb_openInTab = false;
        return mp_view->tabs()->newTab()->page();
    }
    else
        return mp_view->tabs()->browser()->newWindow()->currentTab()->page();
}

void WebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    QString errorString = reply->errorString();

    /*if (m_loadingUrl != reply->url()) {
        // sub resource of this page
        qWarning() << "Resource" << reply->url().toEncoded() << "has unknown Content-Type, will be ignored.";
        reply->deleteLater();
        return;
    }*/

    if (reply->error() == QNetworkReply::NoError && !reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        errorString = "Unknown Content-Type";
    }

    QFile file(QLatin1String(":/notfound.html"));
    bool isOpened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(isOpened);
    Q_UNUSED(isOpened)

    QString title = tr("Error loading page: %1").arg(reply->url().toString());
    QString html = QString(QLatin1String(file.readAll()))
                        .arg(title)
                        .arg(errorString)
                        .arg(reply->url().toString());

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    QIcon icon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
    QPixmap pixmap = icon.pixmap(QSize(32,32));
    if (pixmap.save(&imageBuffer, "PNG")) {
        html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QString(QLatin1String(imageBuffer.buffer().toBase64())));
    }

    QList<QWebFrame*> frames;
    frames.append(mainFrame());
    while (!frames.isEmpty()) {
        QWebFrame *frame = frames.takeFirst();
        if (frame->url() == reply->url()) {
            frame->setHtml(html, reply->url());
            return;
        }
        QList<QWebFrame *> children = frame->childFrames();
        foreach(QWebFrame *frame, children)
            frames.append(frame);
    }
    //if (m_loadingUrl == reply->url()) {
    //    mainFrame()->setHtml(html, reply->url());
    //}
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    // ctrl-alt         open in new window
    // ctrl-alt-shift   open in new window (background)
    // ctrl             open in new tab
    // ctrl-shift       open in new tab (background)
    // shift            force open in this frame (skip url check)
    if (type == QWebPage::NavigationTypeLinkClicked)
    {
        if (m_keyboardMods & Qt::ControlModifier || m_mouseButtons == Qt::MidButton)
        {
            WebView *target;
            if (m_keyboardMods & Qt::AltModifier)
            {
                if (m_keyboardMods & Qt::ShiftModifier)
                    // open in new window (background)
                    target = mp_view->tabs()->browser()->newWindow(false)->currentTab();
                else
                    // open in new window
                    target = mp_view->tabs()->browser()->newWindow(true)->currentTab();
            }
            else if (m_keyboardMods & Qt::ShiftModifier)
                // open in new tab (background)
                target = mp_view->tabs()->newTab(false);
            else
                // open in new tab
                target = mp_view->tabs()->newTab(true);

            // load it
            target->load(request);
            return false;
        }
        else if (m_keyboardMods & Qt::ShiftModifier)
            // skip check
            return true;
        else
            // check url
            return mp_view->tabs()->browser()->navigationRequest(request.url());
    }
    /*if (frame == mainFrame()) {
        m_loadingUrl = request.url();
        emit loadingUrl(m_loadingUrl);
    }*/
    return QWebPage::acceptNavigationRequest(frame, request, type);
}
