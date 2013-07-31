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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vlyc.h"
#include "pluginmanager.h"
#include "vlycbrowser.h"

#include <QtCore>
#include <QInputDialog>
#include <QMessageBox>

#ifdef Q_OS_LINUX
#include "vlyc_xcb.h"
#endif

struct BlockChanged
{
    MainWindow *w;
    BlockChanged(MainWindow *target)
    {
        w = target;
        w->block_changed = true;
    }
    ~BlockChanged()
    {
        w->block_changed = false;
    }
};

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
    connect(&m_player, SIGNAL(endReached()), &m_player, SLOT(stop()));
    connect(&m_player, SIGNAL(positionChanged(float)), SLOT(updatePosition(float)));
    connect(&m_player, SIGNAL(mediaChanged(libvlc_media_t*)), SLOT(mediaChanged(libvlc_media_t*)));
    connect(&m_player, SIGNAL(stateChanged(VlcState::Type)), SLOT(updateState(VlcState::Type)));
    connect(ui->volume, SIGNAL(volumeChanged(int)), &m_audio, SLOT(setVolume(int)));
    connect(ui->volume, SIGNAL(muteChanged(bool)), &m_audio, SLOT(setMuted(bool)));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
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
    mp_video = mp_self->plugins()->sites_video(url);

    if (mp_video == nullptr)
    {
        QMessageBox::critical(this, "Error", QStringLiteral("Cannot open URL %1").arg(url));
        return;
    }

    connect(mp_video, SIGNAL(done()), SLOT(_playVideo()));
    connect(mp_video, SIGNAL(error(QString)), SLOT(_videoError(QString)));
    mp_video->load();
}

void MainWindow::_playVideo()
{
    playVideo(qobject_cast<Video *>(sender()));
}

void MainWindow::_videoError(const QString &message)
{
    QMessageBox::critical(this, "Video Error", message, "Ok");
}

void MainWindow::playVideo(Video *v)
{
    auto qa = v->available();
    if (!qa.length())
        return;

    BlockChanged block(this);
    ui->quality->clear();

    qSort(qa.begin(), qa.end(), qGreater<VideoQuality>());

    foreach (VideoQuality q, qa)
        ui->quality->addItem(q.description);
    ui->quality->setCurrentIndex(0);

    Media m = v->media(qa.first().q);
    m_media = VlcMedia(m.url);
    if (v->useVlcMeta() && !m_media.isParsed())
        m_media.parse(false);
    else
    {
        m_media.setMeta(VlcMeta::Title, v->title());
        m_media.setMeta(VlcMeta::Artist, v->author());
        m_media.setMeta(VlcMeta::Description, v->description());
    }

    m_player.open(m_media);

    ml_qa = qa;
    //if(mp_v)
    //    delete mp_v;
    mp_v = v;
}

void MainWindow::mediaChanged(libvlc_media_t *media)
{
    VlcMedia m(media);
    setWindowTitle(QStringLiteral("%1 - %2 [VLYC2]").arg(m.meta(VlcMeta::Title)).arg(m.meta(VlcMeta::Artist)));
}

void MainWindow::updatePosition(const float &pos)
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

void MainWindow::on_position_sliderDragged(const float &new_position)
{
    m_player.setPosition(new_position);
}

void MainWindow::on_quality_currentIndexChanged(const int &index)
{
    if (block_changed) return;
    float pos = m_player.position();
    Media m = mp_v->media(ml_qa.at(index).q);
    m_media = VlcMedia(m.url);
    m_player.open(m_media);
    m_player.setPosition(pos);
}
