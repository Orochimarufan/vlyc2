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

#pragma once

#include <QtWidgets/QMenu>
#include <QtWebKitWidgets/QWebPage>

class WebView;

class LinkContextMenu : public QMenu
{
    Q_OBJECT

    WebView *mp_view;
    QUrl m_url;

public:
    LinkContextMenu(WebView *view, const QUrl &url);

    enum Action {
        Open,
        OpenNewTab,
        OpenBackgroundTab,
        OpenNewWindow,
        SaveLinkTarget,
        CopyLinkToClipboard,
        DeveloperInspectElement
    };

    using QMenu::addAction;
    void addAction(const Action &action);
    void addAction(const QWebPage::WebAction &action);

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    template <typename T>
    QAction *addAction(const QString &text, T action)
    {
        QAction *a = addAction(text);
        connect(a, &QAction::triggered, action);
        return a;
    }
#endif

    QUrl url() const;
    WebView *tab() const;

private slots:
    void openNewTab();
    void openBackgroundTab();
};
