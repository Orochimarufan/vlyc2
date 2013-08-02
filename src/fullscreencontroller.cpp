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

#include "fullscreencontroller.h"
#include "ui_fullscreencontroller.h"
#include "videowidget.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QDesktopWidget>

#include <QtCore/QTimer>
#include <QtCore/QSettings>


FullScreenController::FullScreenController(VideoWidget *video) :
    QFrame(video), mp_video(video),
    mi_mouse_last_x(-1), mi_mouse_last_y(-1), mb_mouse_over(false),
    mi_mouse_last_move_x(-1), mi_mouse_last_move_y(-1), mb_fullscreen(false),
    mi_screen_number(-1), mb_is_wide(false)
{
    ui = new Ui::FullScreenController();
    ui->setupUi(this);

    setWindowFlags(Qt::ToolTip);
    m_size = minimumSize();

    m_hide_timer.setInterval(5000);
    m_hide_timer.setSingleShot(true);
    connect(&m_hide_timer, SIGNAL(timeout()), SLOT(hide()));

    loadState();
}

void FullScreenController::setFullScreen(bool fs)
{
    mb_fullscreen = fs;
    if (fs)
        connect(mp_video, SIGNAL(mouseMoved(int,int)), SLOT(mouseChanged(int,int)));
    else
    {
        hide();
        disconnect(mp_video, SIGNAL(mouseMoved(int,int)), this, SLOT(mouseChanged(int,int)));
    }
}

int FullScreenController::targetScreen()
{
    if (mi_screen_number == -1 || mi_screen_number > QApplication::desktop()->screenCount())
        return QApplication::desktop()->screenNumber(mp_video);
    return mi_screen_number;
}

void FullScreenController::restoreController()
{
    int target_screen = targetScreen();
    QRect current_resolution = QApplication::desktop()->screenGeometry(target_screen);
    if (!mb_is_wide)
    {
        setMinimumWidth(m_size.width());
        adjustSize();

        if (m_previous_resolution == current_resolution &&
                current_resolution.contains(m_previous_position))
            move(m_previous_position);
        else
        {
            move(computeCenter(current_resolution));
            m_previous_resolution = current_resolution;
            m_previous_position = pos();
        }
    } else
        updateFullWidthGeometry(current_resolution);
}

QPoint FullScreenController::computeCenter(const QRect &screen_resolution)
{
    return QPoint(screen_resolution.x() + (screen_resolution.width() / 2) - (width() / 2),
                        screen_resolution.y() + screen_resolution.height() - height());
}

void FullScreenController::showController()
{
    restoreController();
    show();
}

void FullScreenController::updateFullWidthGeometry(const QRect &screen_resolution)
{
    setMinimumWidth(screen_resolution.width());
    setGeometry(screen_resolution.x(), screen_resolution.y() + screen_resolution.height() - height(), screen_resolution.width(), height());
    adjustSize();
}

void FullScreenController::setFullWidth(bool fw)
{
    mb_is_wide = fw;
    restoreController();
}

bool FullScreenController::toggleFullWidth()
{
    setFullWidth(!mb_is_wide);
    return mb_is_wide;
}

/*void FullScreenController::customEvent(QEvent *e)
{
    switch(e->type()) {
    case ToggleEvent:
        if (mb_fullscreen)
            if (isHidden())
            {
                m_hide_timer.stop();
                showController();
            }
            else
                hide();
        break;
    case ShowEvent:
        if (mb_fullscreen)
            showController();
        break;
    case PlanHideEvent:
        if (!mb_mouse_over)
            m_hide_timer.start();
        break;
    case HideEvent:
        hide();
        break;
    }
}*/

void FullScreenController::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() == Qt::LeftButton)
    {
        if (mi_mouse_last_x == -1 || mi_mouse_last_y == -1)
            return;

        int i_move_x = e->globalX() - mi_mouse_last_x;
        int i_move_y = e->globalY() - mi_mouse_last_y;

        move(x() + i_move_x, y() + i_move_y);

        mi_mouse_last_x = e->globalX();
        mi_mouse_last_y = e->globalY();
    }
}

void FullScreenController::mousePressEvent(QMouseEvent *e)
{
    if (mb_is_wide)
        return;
    mi_mouse_last_x = e->globalX();
    mi_mouse_last_y = e->globalY();
    e->accept();
}

void FullScreenController::mouseReleaseEvent(QMouseEvent *e)
{
    if (mb_is_wide)
        return;
    mi_mouse_last_x = -1;
    mi_mouse_last_y = -1;
    e->accept();

    m_previous_position = pos();
}

void FullScreenController::enterEvent(QEvent *e)
{
    mb_mouse_over = true;
    m_hide_timer.stop();
    e->accept();
}

void FullScreenController::leaveEvent(QEvent *e)
{
    mb_mouse_over = false;
    m_hide_timer.start();
    e->accept();
}

void FullScreenController::keyPressEvent(QKeyEvent *e)
{
    emit keyPressed(e);
}

void FullScreenController::mouseChanged(int mouse_x, int mouse_y)
{
    if (mi_mouse_last_move_x == -1 || mi_mouse_last_move_y == -1 ||
            qAbs(mi_mouse_last_move_x - mouse_x) > 2 ||
            qAbs(mi_mouse_last_move_y - mouse_y) > 2)
    {
        mi_mouse_last_move_x = mouse_x;
        mi_mouse_last_move_y = mouse_y;
        showController();
        m_hide_timer.start();
    }
}

bool FullScreenController::isFullScreen()
{
    return mb_fullscreen;
}

/*bool FullScreenController::eventFilter(QObject *o, QEvent *e)
{
    if (mb_fullscreen && e->type() == QEvent::MouseMove)
    {
        QMouseEvent *m = static_cast<QMouseEvent *>(e);
        mouseChanged(m->globalX(), m->globalY());
    }
    return false;
}*/

void FullScreenController::saveState()
{
    QSettings config;
    config.beginGroup("FullScreen");
    config.setValue("controllerPosition", pos());
    config.setValue("controllerIsWide", mb_is_wide);
    config.setValue("lastScreenResolution", m_previous_resolution);
}

void FullScreenController::loadState()
{
    QSettings config;
    config.beginGroup("FullScreen");
    m_previous_position = config.value("controllerPosition").toPoint();
    mb_is_wide = config.value("controllerIsWide", false).toBool();
    m_previous_resolution = config.value("lastScreenResolution").toRect();
}

FullScreenController::~FullScreenController()
{
    saveState();
}

void FullScreenController::on_btn_togglewide_clicked()
{
    if (!mb_is_wide)
    {
        QPoint centerPosition = computeCenter(QApplication::desktop()->screenGeometry(parentWidget()));
        if (centerPosition == pos())
            setFullWidth(true);
        else
        {
            m_previous_position = centerPosition;
            move(centerPosition);
        }
    }
    else
        setFullWidth(false);
}
