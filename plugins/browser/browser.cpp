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

#include "browser.h"
#include "browserwindow.h"
#include "webview.h"
#include "LinkContextMenu.h"
#include "bookmarkdialog.h"

#include <QListIterator>
#include <QEvent>
#include <QSettings>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include <QPixmap>
#include <QBuffer>

// construct/destruct
Browser::Browser(QObject *parent) :
    QObject(parent),
    mp_window(nullptr),
    ms_title_postfix("Browser")
{
    loadState();

    // TODO: less frequent.
    connect(&m_bookmarks, &BookmarkModel::bookmarksChanged, this, &Browser::saveState);
}

Browser::~Browser()
{
    saveState();
    // delete the windows
    QListIterator<BrowserWindow *> iter(ml_windows);
    while (iter.hasNext())
        iter.next()->deleteLater();
    // the other things get collected by Qt
}

// getters
QList<BrowserWindow *> Browser::windows() const
{
    return ml_windows;
}

BrowserWindow *Browser::currentWindow() const
{
    return mp_window;
}

WebView *Browser::currentTab() const
{
    return mp_window->currentTab();
}

QString Browser::titlePostfix() const
{
    return ms_title_postfix;
}

void Browser::setTitlePostfix(const QString &postfix)
{
    ms_title_postfix = postfix;
}

QUrl Browser::homeUrl() const
{
    return m_home_url;
}

void Browser::setHomeUrl(const QUrl &homeUrl)
{
    m_home_url = homeUrl;
    saveState();
}

BookmarkModel *Browser::bookmarks()
{
    return &m_bookmarks;
}

void Browser::manageBookmarks()
{
    auto dlg = new BookmarksDialog(&m_bookmarks, currentWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

// new Window
BrowserWindow *Browser::newWindow0(QWidget *parent)
{
    auto win = new BrowserWindow(this, parent);
    win->installEventFilter(this);
    ml_windows.append(win);
    return win;
}

BrowserWindow *Browser::newWindow1(bool takeFocus, QWidget *parent)
{
    auto win = newWindow0(parent);

    if (!takeFocus)
        win->setAttribute(Qt::WA_ShowWithoutActivating);

    win->show();

    if (takeFocus)
    {
        win->raise();
        win->activateWindow();
        win->currentTab()->setFocus();
    }

    return win;
}

BrowserWindow *Browser::newWindow(bool takeFocus, QWidget *parent)
{
    auto win = newWindow1(takeFocus, parent);
    win->currentTab()->setUrl(homeUrl());
    return win;
}

BrowserWindow *Browser::newWindow(const QUrl &url, bool takeFocus, QWidget *parent)
{
    auto win = newWindow1(takeFocus, parent);
    win->currentTab()->setUrl(url);
    return win;
}

// event filter
bool Browser::eventFilter(QObject *recv, QEvent *e)
{
    BrowserWindow *win = qobject_cast<BrowserWindow *>(recv);
    // we use it to track the currently/last active window :)
    if (win && e->type() == QEvent::WindowActivate)
        mp_window = win;
    return QObject::eventFilter(recv, e);
}

bool Browser::navigationRequest(QUrl url)
{
    Q_UNUSED(url);
    return true;
}

// state
void Browser::saveState()
{
    QSettings config;
    config.beginGroup("Browser");

    // Homepage
    config.setValue("Home", m_home_url.toString());

    // Bookmarks
    QJsonArray bookmarks;
    for (const Bookmark &b : m_bookmarks.bookmarks())
    {
        QByteArray pixmap;
        {
            QBuffer buf(&pixmap);
            b.icon.pixmap(50, 50).save(&buf, "JPG");
        }
        bookmarks << QJsonObject({
                                     {"icon", QString(pixmap.toBase64())},
                                     {"name", b.name},
                                     {"url", b.url.toString()},
                                 });
    }
    config.setValue("Bookmarks", QJsonDocument(bookmarks).toJson(QJsonDocument::Compact));
}

void Browser::loadState()
{
    QSettings config;
    config.beginGroup("Browser");

    // Homepage
    m_home_url = QUrl(config.value("Home", "about:blank").toString());

    // Bookmarks
    QJsonDocument bd = QJsonDocument::fromJson(config.value("Bookmarks", "[]").toByteArray());
    if (bd.isArray())
    {
        BookmarkList bookmarks;
        for (QJsonValue v : bd.array())
            if (v.isObject())
            {
                QJsonObject j = v.toObject();
                Bookmark bmk;
                if (j["icon"].isString())
                {
                    QPixmap pix;
                    pix.loadFromData(QByteArray::fromBase64(j["icon"].toString().toLatin1()));
                    bmk.icon.addPixmap(pix);
                }
                if (j["name"].isString())
                    bmk.name = j["name"].toString();
                if (j["url"].isString())
                    bmk.url = j["url"].toString();
                bookmarks << bmk;
            }
        m_bookmarks.replace(bookmarks);
    }
}

void Browser::linkContextMenu(LinkContextMenu *menu)
{
    menu->addAction(LinkContextMenu::Open);
    menu->addAction(LinkContextMenu::OpenNewTab);
    menu->addAction(LinkContextMenu::OpenNewWindow);
    menu->addAction(LinkContextMenu::OpenBackgroundTab);
    menu->addSeparator();
    menu->addAction(LinkContextMenu::SaveLinkTarget);
    menu->addSeparator();
    menu->addAction(LinkContextMenu::CopyLinkToClipboard);
}
