/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2016 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include <QFrame>

#include <QtCore/QEvent>
#include <QtCore/QTimer>

namespace Ui {
class FullScreenController;
}

class VideoWidget;

class FullScreenController : public QFrame
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit FullScreenController(VideoWidget *videoWidget);
    ~FullScreenController();

    bool isEnabled();
    bool isFullWidth();

public slots:
    void setEnabled(bool fs);
    void setFullWidth(bool fw);
    void mouseChanged(int mouse_x, int mouse_y);

private slots:
    void showController();
    void restoreController();
    void updateFullWidthGeometry(const QRect &bounds);

    void saveState();
    void loadState();

    void on_btn_togglewide_clicked();

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    
private:
    Ui::FullScreenController *ui;
    VideoWidget *mp_video;

    bool mb_enabled;

    bool mb_is_wide;
    QSize m_size;

    QTimer m_hide_timer;

    int mi_mouse_last_x;
    int mi_mouse_last_y;
    bool mb_mouse_over;
    int mi_mouse_last_move_x;
    int mi_mouse_last_move_y;

    QPoint m_previous_position;
    QRect m_previous_bounds;
};
