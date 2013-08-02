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

#include "videowidget.h"

#include <QEvent>
#include <QMouseEvent>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *xlayout = new QHBoxLayout(this);
    xlayout->setContentsMargins(0,0,0,0);
    QWidget *xstable = new QWidget();
    xlayout->addWidget(xstable);
    mp_stable = NULL;
    mp_layout = new QHBoxLayout(xstable);
    mp_layout->setContentsMargins(0,0,0,0);
    QPalette plt = palette();
    plt.setColor(QPalette::Window, Qt::black);
    setPalette(plt);
    setAutoFillBackground(true);
}

WId VideoWidget::request(bool b_keep_size, unsigned int i_width, unsigned int i_height)
{
    if (mp_stable) return mp_stable->winId();

    if (b_keep_size)
    {
        i_width = width();
        i_height = height();
    }

    mp_stable = new QWidget();
    //QPalette plt = mp_stable->palette();
    //plt.setColor(QPalette::Window, Qt::black);
    //mp_stable->setPalette(plt);
    //mp_stable->setAutoFillBackground(true);
    mp_layout->addWidget(mp_stable);
    mp_stable->installEventFilter(this);
    mp_stable->setMouseTracking(true);
    return mp_stable->winId();
}

void VideoWidget::release()
{
    if (mp_stable)
    {
        mp_layout->removeWidget(mp_stable);
        mp_stable->deleteLater();
        mp_stable = NULL;
    }
    updateGeometry();
}

void VideoWidget::setSizing(int w, int h)
{
    resize(w, h);
    if (width() == w && height() == h)
        updateGeometry();
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked();
}

void VideoWidget::mouseMoveEvent(QMouseEvent *e)
{
    emit mouseMoved(e->globalX(), e->globalY());
}

bool VideoWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == mp_stable)
        if(e->type() == QEvent::MouseButtonDblClick)
            emit doubleClicked();
        else if (e->type() == QEvent::MouseMove)
            emit mouseMoved(((QMouseEvent*)e)->globalX(), ((QMouseEvent*)e)->globalY());
    return false;
}
