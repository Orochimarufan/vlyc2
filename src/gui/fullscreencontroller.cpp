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
    mi_mouse_last_move_x(-1), mi_mouse_last_move_y(-1),
    mb_enabled(false), mb_is_wide(false)
{
    ui = new Ui::FullScreenController();
    ui->setupUi(this);

    setAutoFillBackground(true);
    m_size = minimumSize();

    m_hide_timer.setInterval(5000);
    m_hide_timer.setSingleShot(true);
    connect(&m_hide_timer, &QTimer::timeout, this, &QWidget::hide);

    loadState();
    hide();
}

void FullScreenController::setEnabled(bool fs)
{
    mb_enabled = fs;
    if (fs)
        connect(mp_video, &VideoWidget::mouseMoved, this, &FullScreenController::mouseChanged);
    else
    {
        hide();
        disconnect(mp_video, &VideoWidget::mouseMoved, this, &FullScreenController::mouseChanged);
    }
}

bool FullScreenController::isEnabled()
{
    return mb_enabled;
}

// ----------------------------------------------------------------------------
// Geometry
// ----------------------------------------------------------------------------
static QPoint placeAtCenterBottom(const QSize &size, const QRect &bounds)
{
    return QPoint(bounds.x() + (bounds.width() / 2) - (size.width() / 2),
                  bounds.y() + bounds.height() - size.height());
}

static QPoint placeInBounds(const QRect &geometry, const QRect &bounds)
{
    return QPoint(qMin(bounds.x() + bounds.width() - geometry.width(), qMax(bounds.x(), geometry.x())),
                  qMin(bounds.y() + bounds.height() - geometry.height(), qMax(bounds.y(), geometry.y())));
}

void FullScreenController::restoreController()
{
    QRect bounds = parentWidget()->rect();
    if (!mb_is_wide)
    {
        setMinimumWidth(m_size.width());
        adjustSize();

        if (m_previous_bounds == bounds &&
                bounds.contains(m_previous_position))
            move(m_previous_position);
        else
        {
            move(placeAtCenterBottom(size(), bounds));
            m_previous_bounds = bounds;
            m_previous_position = pos();
        }
    } else
        updateFullWidthGeometry(bounds);
}

void FullScreenController::updateFullWidthGeometry(const QRect &bounds)
{
    setMinimumWidth(bounds.width());
    setGeometry(bounds.x(), bounds.y() + bounds.height() - height(), bounds.width(), height());
    adjustSize();
}

void FullScreenController::setFullWidth(bool fw)
{
    mb_is_wide = fw;
    restoreController();
}

bool FullScreenController::isFullWidth()
{
    return mb_is_wide;
}

void FullScreenController::showController()
{
    restoreController();
    show();
}

// ----------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------
void FullScreenController::mouseMoveEvent(QMouseEvent *e)
{
    // Move position of compact UI
    if (mb_is_wide)
        return;
    if (e->buttons() == Qt::LeftButton)
    {
        if (mi_mouse_last_x == -1 || mi_mouse_last_y == -1)
            return;

        int i_move_x = e->globalX() - mi_mouse_last_x;
        int i_move_y = e->globalY() - mi_mouse_last_y;

        move(placeInBounds(geometry().adjusted(i_move_x, i_move_y, i_move_x, i_move_y), m_previous_bounds));

        mi_mouse_last_x = e->globalX();
        mi_mouse_last_y = e->globalY();

        e->accept();
    }
}

void FullScreenController::mousePressEvent(QMouseEvent *e)
{
    // Move position of compact UI
    if (mb_is_wide)
        return;
    mi_mouse_last_x = e->globalX();
    mi_mouse_last_y = e->globalY();
    e->accept();
}

void FullScreenController::mouseReleaseEvent(QMouseEvent *e)
{
    // Move position of compact UI
    if (mb_is_wide)
        return;
    mi_mouse_last_x = -1;
    mi_mouse_last_y = -1;
    e->accept();

    m_previous_position = pos();
}

void FullScreenController::enterEvent(QEvent *e)
{
    // Don't hide while Mouse over UI
    mb_mouse_over = true;
    m_hide_timer.stop();
    e->accept();
}

void FullScreenController::leaveEvent(QEvent *e)
{
    // Hide after Mouse leaves UI
    mb_mouse_over = false;
    m_hide_timer.start();
    e->accept();
}

void FullScreenController::mouseChanged(int mouse_x, int mouse_y)
{
    // Show UI when mouse is moved on video
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

// ----------------------------------------------------------------------------
// Keep state
// ----------------------------------------------------------------------------
void FullScreenController::saveState()
{
    QSettings config;
    config.beginGroup("FullScreen");
    config.setValue("controllerPosition", pos());
    config.setValue("controllerIsWide", mb_is_wide);
    config.setValue("lastScreenResolution", m_previous_bounds);
}

void FullScreenController::loadState()
{
    QSettings config;
    config.beginGroup("FullScreen");
    m_previous_position = config.value("controllerPosition").toPoint();
    mb_is_wide = config.value("controllerIsWide", false).toBool();
    m_previous_bounds = config.value("lastScreenResolution").toRect();
}

FullScreenController::~FullScreenController()
{
    saveState();
}

// ----------------------------------------------------------------------------
// UI
// ----------------------------------------------------------------------------
void FullScreenController::on_btn_togglewide_clicked()
{
    // Cycle through Compact -> Centered Compact -> Wide -> Compact
    if (!mb_is_wide)
    {
        QPoint centerPosition = placeAtCenterBottom(size(), parentWidget()->rect());
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
