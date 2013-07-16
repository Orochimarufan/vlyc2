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

#include "tabbar.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>

#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>

#include <QtWidgets/QApplication>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuRequested(QPoint)));

    QString meta = QStringLiteral("Ctrl+%1");
    for (int i=1; i <= 10; i++)
    {
        QShortcut *self = new QShortcut(meta.arg(i==10?0:i), this);
        ml_shorts << self;
        connect(self, SIGNAL(activated()), SLOT(selectTabAction()));
    }

    setTabsClosable(true);
    connect(this, SIGNAL(tabCloseRequested(int)),
            this, SIGNAL(cloneTab(int)));
    setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);

    setMovable(true);
}

void TabBar::selectTabAction()
{
    if (QShortcut *self = qobject_cast<QShortcut *>(sender()))
        setCurrentIndex(ml_shorts.indexOf(self));
}

void TabBar::contextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(tr("New &Tab"), this, SIGNAL(newTab()), QKeySequence::AddTab);

    int index = tabAt(pos);
    if (index != -1)
    {
        QAction *action = menu.addAction(tr("Clone Tab"), this, SLOT(cloneTab()));
        action->setData(index);

        menu.addSeparator();

        action = menu.addAction(tr("&Close Tab"), this, SLOT(closeTab()), QKeySequence::Close);
        action->setData(index);

        action = menu.addAction(tr("Close all &other Tabs"), this, SLOT(closeOtherTabs()));
        action->setData(index);

        menu.addSeparator();

        action = menu.addAction(tr("Reload Tab"), this, SLOT(reloadTab()), QKeySequence::Refresh);
        action->setData(index);
    }
    else
    {
        menu.addSeparator();
    }

    menu.addAction(tr("Reload all Tabs"), this, SIGNAL(reloadAllTabs()));

    menu.exec(QCursor::pos());
}

void TabBar::cloneTab()
{
    if (QAction *self = qobject_cast<QAction *>(sender()))
        emit cloneTab(self->data().toInt());
}

void TabBar::closeTab()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
        emit closeTab(action->data().toInt());
}

void TabBar::closeOtherTabs()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
        emit closeOtherTabs(action->data().toInt());
}

void TabBar::reloadTab()
{
    if (QAction *action = qobject_cast<QAction *>(sender()))
        emit reloadTab(action->data().toInt());
}


void TabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragStartPos = event->pos();
    QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        int diffX = event->pos().x() - m_dragStartPos.x();
        int diffY = event->pos().y() - m_dragStartPos.y();

        if ((event->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()
            && diffX < 3 && diffX > -3 && diffY < -10)
        {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            int index = tabAt(event->pos());

            QList<QUrl> urls;
            urls << tabData(index).toUrl();

            mimeData->setUrls(urls);
            mimeData->setText(tabText(index));
            mimeData->setData(QStringLiteral("action"), "tab-reordering");

            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
    QTabBar::mouseMoveEvent(event);
}
