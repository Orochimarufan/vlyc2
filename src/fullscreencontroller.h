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

#ifndef FULLSCREENCONTROLLER_H
#define FULLSCREENCONTROLLER_H

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
    int targetScreen();
    bool isFullScreen();

public slots:
    void setFullScreen(bool fs);
    void setFullWidth(bool fw);
    bool toggleFullWidth();
    void mouseChanged(int mouse_x, int mouse_y);

private slots:
    void showController();
    void restoreController();
    void updateFullWidthGeometry(const QRect &screen_resolution);

    void saveState();
    void loadState();

    void on_btn_togglewide_clicked();

private:
    QPoint computeCenter(const QRect &screen_resolution);

protected:
    //void customEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    
private:
    Ui::FullScreenController *ui;
    VideoWidget *mp_video;

    int mi_mouse_last_x;
    int mi_mouse_last_y;
    bool mb_mouse_over;
    int mi_mouse_last_move_x;
    int mi_mouse_last_move_y;
    bool mb_fullscreen;
    int mi_screen_number;
    QTimer m_hide_timer;
    bool mb_is_wide;
    QSize m_size;
    QPoint m_previous_position;
    QRect m_previous_resolution;

public:
    //int HideEvent = QEvent::registerEventType();
    //int ShowEvent = QEvent::registerEventType();
    //int ToggleEvent = QEvent::registerEventType();
    //int PlanHideEvent = QEvent::registerEventType();

signals:
    void keyPressed(QKeyEvent *);
};

#endif // FULLSCREENCONTROLLER_H
