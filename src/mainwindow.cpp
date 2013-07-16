/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vlyc.h"
#include "sitemanager.h"
#include "vlycbrowser.h"

#include <QtCore>
#include <QInputDialog>
#include <QMessageBox>

#ifdef Q_OS_LINUX
#include "vlyc_xcb.h"
#endif

MainWindow::MainWindow(Vlyc *self) :
    QMainWindow(),
    ui(new Ui::MainWindow),
    m_audio(m_player),
    mp_video(nullptr),
    mp_self(self)
{
#ifdef Q_OS_LINUX
    XCB::setWMClass(winId(), "vlyc2", "vlyc2");
#endif
    ui->setupUi(this);

    m_player.setVideoDelegate(ui->video);

    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(openUrl()));
    connect(ui->btn_play, SIGNAL(clicked()), &m_player, SLOT(togglePause()));
    connect(ui->btn_stop, SIGNAL(clicked()), &m_player, SLOT(stop()));
    connect(ui->actionBrowser, SIGNAL(triggered()), SLOT(browser()));
    connect(&m_player, SIGNAL(positionChanged(float)), SLOT(updatePosition(float)));
    connect(&m_player, SIGNAL(endReached()), &m_player, SLOT(stop()));
}

MainWindow::~MainWindow()
{
    delete mp_video;
    delete ui;
}

void MainWindow::browser()
{
    mp_self->browser()->newWindow(true);
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(this, tr("Open Url"), tr("Enter URL"));

    if (url.isEmpty())
        return;

    if (mp_video != nullptr)
        delete mp_video;
    mp_video = mp_self->sites()->video(url);

    if (mp_video == nullptr)
    {
        QMessageBox::critical(this, "Error", QStringLiteral("Cannot open URL %1").arg(url));
        return;
    }

    connect(mp_video, SIGNAL(loaded(Video*)), SLOT(playVideo(Video*)));
    mp_video->load();
}

void MainWindow::playVideo(Video *v)
{
    m_media = v->media(v->available()[0]);

    m_player.open(m_media);
}

void MainWindow::updatePosition(float pos)
{
    auto time = m_player.time();
    auto length = m_player.length();
    ui->position->setPosition(pos, time, length);
    ui->timeLabel->setDisplayPosition(pos, time, length);
    // TODO: find a better way to do this
    ui->volume->updateVolume(m_audio.volume());
    ui->volume->setMuted(m_audio.isMuted());
}

void MainWindow::updateState(const VlcState::Type &new_state)
{
    if (new_state == VlcState::Stopped)
    {
        ui->position->setPosition(-1, 0, 0);
        ui->timeLabel->setDisplayPosition(-1, 0, 0);
    }
    ui->btn_play->updateButtonIcons(new_state == VlcState::Playing ||
                                      new_state == VlcState::Buffering ||
                                      new_state == VlcState::Opening);
}
