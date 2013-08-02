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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QtVlc/IVlcVideoDelegate.h>
#include <QHBoxLayout>

class VideoWidget : public QWidget, public IVlcVideoDelegate
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);

    virtual WId request(bool b_keep_size, unsigned int i_width, unsigned int i_height);
    virtual void release();

    void setSizing(int w, int h);

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool eventFilter(QObject *, QEvent *);
    
signals:
    void doubleClicked();
    void mouseMoved(int x, int y);

private:
    QWidget *mp_stable;
    QHBoxLayout *mp_layout;
};

#endif // VIDEOWIDGET_H
