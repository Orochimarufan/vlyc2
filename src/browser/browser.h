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

#ifndef BROWSER_H
#define BROWSER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class NetworkAccessManager;
//class HistoryManager;
//class BookmarkManager;
//class DownloadManager;
class BrowserWindow;
class WebView;
class QNetworkRequest;

class Browser : public QObject
{
    Q_OBJECT

public:
    explicit Browser(QObject *parent = 0);
    virtual ~Browser();

    QList<BrowserWindow *> windows() const;
    BrowserWindow *currentWindow() const;
    WebView *currentTab() const;

    NetworkAccessManager *network() const;

    BrowserWindow *newWindow0(QWidget *parent);
    BrowserWindow *newWindow1(bool takeFocus=true, QWidget *paret=nullptr);
    BrowserWindow *newWindow(bool takeFocus=true, QWidget *parent=nullptr);
    BrowserWindow *newWindow(const QUrl &url, bool takeFocus = true, QWidget *parent=nullptr);

    virtual bool navigationRequest(QUrl);

    QString titlePostfix() const;
    void setTitlePostfix(const QString &postfix);

    QUrl homeUrl() const;
    void setHomeUrl(const QUrl &homeUrl);

protected:
    bool eventFilter(QObject *, QEvent *);

    void saveState();
    void loadState();

    NetworkAccessManager *mp_network;
    QList<BrowserWindow *> ml_windows;
    BrowserWindow *mp_window;
    QString ms_title_postfix;
    QUrl m_home_url;
};

#endif // BROWSER_H
