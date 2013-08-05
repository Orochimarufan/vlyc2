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
#include "fullscreencontroller.h"
#include "ui_fullscreencontroller.h"
#include "vlyc.h"
#include "pluginmanager.h"
#include "vlycbrowser.h"
#include <siteplugin.h>

#include <QtCore>
#include <QInputDialog>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTemporaryFile>

#ifdef Q_OS_LINUX
#include "vlyc_xcb.h"
#endif

// BlockChanged
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

// VideoCaller
VideoCaller::VideoCaller()
{
    video = nullptr;
}

VideoCaller::VideoCaller(Video *v)
{
    video = v;
    connect(this, SIGNAL(_load()), video, SLOT(load()));
    connect(this, SIGNAL(_getMedia(VideoQualityLevel)), video, SLOT(getMedia(VideoQualityLevel)));
    connect(this, SIGNAL(_getSubtitles(QString)), video, SLOT(getSubtitles(QString)));
    connect(video, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(video, SIGNAL(done()), SIGNAL(done()));
    connect(video, SIGNAL(media(VideoMedia)), SIGNAL(media(VideoMedia)));
    connect(video, SIGNAL(subtitles(VideoSubtitles)), SIGNAL(subtitles(VideoSubtitles)));
}

VideoCaller &VideoCaller::operator =(Video *v)
{
    if (video)
    {
        disconnect(this, 0, video, 0);
        disconnect(video, 0, this, 0);
    }
    video = v;
    if (video)
    {
        connect(this, SIGNAL(_load()), video, SLOT(load()));
        connect(this, SIGNAL(_getMedia(VideoQualityLevel)), video, SLOT(getMedia(VideoQualityLevel)));
        connect(this, SIGNAL(_getSubtitles(QString)), video, SLOT(getSubtitles(QString)));
        connect(video, SIGNAL(error(QString)), SIGNAL(error(QString)));
        connect(video, SIGNAL(done()), SIGNAL(done()));
        connect(video, SIGNAL(media(VideoMedia)), SIGNAL(media(VideoMedia)));
        connect(video, SIGNAL(subtitles(VideoSubtitles)), SIGNAL(subtitles(VideoSubtitles)));
    }
}

void VideoCaller::load()
{
    emit _load();
}

void VideoCaller::getMedia(const VideoQualityLevel &level)
{
    emit _getMedia(level);
}

void VideoCaller::getSubtitles(const QString &lang)
{
    emit _getSubtitles(lang);
}

// MainWindow
MainWindow::MainWindow(Vlyc *self) :
    QMainWindow(),
    ui(new Ui::MainWindow),
    m_player_audio(m_player),
    m_player_video(m_player),
    mp_video(nullptr),
    mp_self(self)
{
#ifdef Q_OS_LINUX
    XCB::setWMClass(winId(), qApp->applicationName(), qApp->applicationName());
#endif
    ui->setupUi(this);

    fsc = new FullScreenController(ui->video);

    m_player.setVideoDelegate(ui->video);

    // Menu
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(openUrl()));
    connect(ui->actionBrowser, SIGNAL(triggered()), SLOT(browser()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));

    // Ui
    connect(ui->btn_play, SIGNAL(clicked()), &m_player, SLOT(togglePause()));
    connect(ui->btn_stop, SIGNAL(clicked()), &m_player, SLOT(stop()));
    connect(fsc->ui->btn_playpause, SIGNAL(clicked()), &m_player, SLOT(togglePause()));
    connect(fsc->ui->btn_stop, SIGNAL(clicked()), &m_player, SLOT(stop()));
    connect(ui->volume, SIGNAL(volumeChanged(int)), &m_player_audio, SLOT(setVolume(int)));
    connect(ui->volume, SIGNAL(muteChanged(bool)), &m_player_audio, SLOT(setMuted(bool)));
    connect(fsc->ui->volume, SIGNAL(volumeChanged(int)), &m_player_audio, SLOT(setVolume(int)));
    connect(fsc->ui->volume, SIGNAL(muteChanged(bool)), &m_player_audio, SLOT(setMuted(bool)));
    connect(fsc->ui->position, SIGNAL(sliderDragged(float)), SLOT(on_position_sliderDragged(float)));
    connect(ui->btn_fullscreen, SIGNAL(clicked()), SLOT(toggleFullScreen()));
    connect(fsc->ui->btn_defullscreen, SIGNAL(clicked()), SLOT(toggleFullScreen()));

    // Player
    connect(&m_player, SIGNAL(endReached()), &m_player, SLOT(stop()));
    connect(&m_player, SIGNAL(positionChanged(float)), SLOT(updatePosition(float)));
    connect(&m_player, SIGNAL(mediaChanged(libvlc_media_t*)), SLOT(mediaChanged(libvlc_media_t*)));
    connect(&m_player, SIGNAL(stateChanged(VlcState::Type)), SLOT(updateState(VlcState::Type)));

    ui->video->installEventFilter(this);

    // Shortcuts
    shortcut_Space = new QShortcut(QKeySequence(" "), ui->video);
    connect(shortcut_Space, SIGNAL(activated()), &m_player, SLOT(togglePause()));
    shortcut_F11 = new QShortcut(QKeySequence("F11"), ui->video);
    connect(shortcut_F11, SIGNAL(activated()), SLOT(toggleFullScreen()));
    shortcut_AltReturn = new QShortcut(QKeySequence("Alt + Return"), ui->video);
    connect(shortcut_AltReturn, SIGNAL(activated()), SLOT(toggleFullScreen()));
    shortcut_Esc = new QShortcut(QKeySequence("Esc"), ui->video);
    connect(shortcut_Esc, SIGNAL(activated()), SLOT(setFullScreenFalse()));

    // Videos
    connect(&m_video, SIGNAL(error(QString)), SLOT(_videoError(QString)));
    connect(&m_video, SIGNAL(done()), SLOT(_playVideo()));
    connect(&m_video, SIGNAL(media(VideoMedia)), SLOT(_videoMedia(VideoMedia)));
    connect(&m_video, SIGNAL(subtitles(VideoSubtitles)), SLOT(_videoSubs(VideoSubtitles)));

    loadState();
}

MainWindow::~MainWindow()
{
    delete mp_video;
    delete fsc;
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

    Video *video = mp_self->plugins()->sites_video(url);

    if (!video)
    {
        QMessageBox::critical(this, "Error", QStringLiteral("Cannot open URL %1").arg(url));
        return;
    }

    video->site();
    video->videoId();
    qDebug("video from Plugin %s", qPrintable(video->site()->name()));

    m_video = video;
    m_video.load();
}

void MainWindow::_playVideo()
{
    playVideo(m_video.video);
}

void MainWindow::_videoError(const QString &message)
{
    QMessageBox::critical(this, "Video Error", message, "Ok");
}

void MainWindow::playVideo(Video *v)
{
    auto qa = v->availableQualities();
    if (!qa.length())
        return;

    BlockChanged block(this);
    ui->quality->clear();

    qSort(qa.begin(), qa.end(), qGreater<VideoQuality>());

    foreach (VideoQuality q, qa)
        ui->quality->addItem(q.description);
    ui->quality->setCurrentIndex(0);

    m_player.stop();

    ml_qa = qa;
    mp_video = v;

    m_video = v;
    m_video.getMedia(qa.first().q);

    ui->subtitles->clear();
    ui->subtitles->addItem("No Subtitles");

    auto subs = v->availableSubtitleLanguages();
    if (!subs.length())
        return;

    foreach(QString language, subs)
        ui->subtitles->addItem(language);
    ui->subtitles->setCurrentIndex(0);
}

void MainWindow::_videoMedia(const VideoMedia &media)
{
    m_video_media = media;

    m_media = VlcMedia(media.url);
    if (media.video->useFileMetadata() && !m_media.isParsed())
        m_media.parse(false);
    else
    {
        m_media.setMeta(VlcMeta::Title, media.video->title());
        m_media.setMeta(VlcMeta::Artist, media.video->author());
        m_media.setMeta(VlcMeta::Description, media.video->description());
    }

    bool restore = m_player.state() == VlcState::Playing || m_player.state() == VlcState::Paused;
    float position = m_player.position();

    m_player.open(m_media);

    if (restore)
        m_player.setPosition(position);
}

void MainWindow::_videoSubs(const VideoSubtitles &subs)
{
    if (subs.data.type() == QMetaType::QUrl)
        m_player_video.setSubtitleFile(subs.data.value<QUrl>().toString());
    else
    {
        QString templat ("vlyc2-XXXXXX.");
        templat.append(subs.type);
        QTemporaryFile file(QDir::temp().absoluteFilePath(templat));
        file.setAutoRemove(false);
        file.open();
        file.write(subs.data.toByteArray());
        file.close();
        m_player_video.setSubtitleFile(file.fileName());
    }
}

void MainWindow::mediaChanged(libvlc_media_t *media)
{
    VlcMedia m(media);
    setWindowTitle(QStringLiteral("%1 - %2").arg(m.meta(VlcMeta::Title)).arg(m.meta(VlcMeta::Artist)));
}

void MainWindow::updatePosition(const float &pos)
{
    auto time = m_player.time();
    auto length = m_player.length();
    ui->position->setPosition(pos, time, length);
    ui->timeLabel->setDisplayPosition(pos, time, length);
    fsc->ui->position->setPosition(pos, time, length);
    fsc->ui->timeLabel->setDisplayPosition(pos, time, length);
    // TODO: find a better way to do this
    ui->volume->updateVolume(m_player_audio.volume());
    ui->volume->setMuted(m_player_audio.isMuted());
    fsc->ui->volume->updateVolume(m_player_audio.volume());
    fsc->ui->volume->setMuted(m_player_audio.isMuted());
}

void MainWindow::updateState(const VlcState::Type &new_state)
{
    if (new_state == VlcState::Stopped)
    {
        ui->position->setPosition(-1, 0, 0);
        ui->timeLabel->setDisplayPosition(-1, 0, 0);
        fsc->ui->position->setPosition(-1, 0, 0);
        fsc->ui->timeLabel->setDisplayPosition(-1, 0, 0);
    }
    bool playing = new_state == VlcState::Playing ||
            new_state == VlcState::Buffering ||
            new_state == VlcState::Opening;
    ui->btn_play->updateButtonIcons(playing);
    fsc->ui->btn_playpause->updateButtonIcons(playing);
}

void MainWindow::on_position_sliderDragged(const float &new_position)
{
    m_player.setPosition(new_position);
}

void MainWindow::on_subtitles_currentIndexChanged(const int &index)
{
    if (block_changed) return;
    if (index == 0)
        m_player_video.setSpu(0);
    else
    {
        m_video = mp_video;
        m_video.getSubtitles(ui->subtitles->itemText(index));
    }
}

void MainWindow::on_quality_currentIndexChanged(const int &index)
{
    if (block_changed) return;
    m_video = mp_video;
    m_video.getMedia(ml_qa.at(index).q);
}

void MainWindow::setFullScreenVideo(bool fs)
{
    // Reparenting breaks stuff for some reason.
    // therefore we modify the MainWindow to
    // only show the video part and fullscreen that.
    if (fs) {
        //ui->video->setParent(NULL, Qt::Window);
        //ui->video->showFullScreen();
        showFullScreen();
        ui->menubar->setVisible(false);
        ui->statusbar->setVisible(false);
        ui->controls->setVisible(false);
        ui->centralwidget->layout()->setContentsMargins(0,0,0,0);
    } else {
        //ui->video->setParent(ui->centralwidget, 0);
        //ui->verticalLayout->insertWidget(0, ui->video);
        showNormal();
        ui->menubar->setVisible(true);
        ui->statusbar->setVisible(true);
        ui->controls->setVisible(true);
        ui->centralwidget->layout()->setContentsMargins(9,9,9,9);
    }
}

void MainWindow::setFullScreen(bool fs)
{
    if (fs != fsc->mb_fullscreen)
    {
        fsc->setFullScreen(fs);
        setFullScreenVideo(fs);
    }
}

void MainWindow::setFullScreenFalse()
{
    setFullScreen(false);
}

bool MainWindow::toggleFullScreen()
{
    setFullScreen(!fsc->mb_fullscreen);
    return fsc->mb_fullscreen;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->video && e->type() == QEvent::MouseButtonDblClick)
        toggleFullScreen();
    return false;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);
    saveState();
    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (~windowState() & Qt::WindowMaximized)
        m_geometry = geometry();
}

void MainWindow::moveEvent(QMoveEvent *e)
{
    QMainWindow::moveEvent(e);
    if (~windowState() & Qt::WindowMaximized)
        m_geometry = geometry();
}

void MainWindow::saveState()
{
    QSettings config;
    config.beginGroup("MainWindow");
    config.setValue("geometry", m_geometry);
    config.setValue("maximised", (windowState() & Qt::WindowMaximized) != 0);
}

void MainWindow::loadState()
{
    QSettings config;
    config.beginGroup("MainWindow");
    setGeometry(config.value("geometry", geometry()).toRect());
    if (config.value("maximised", false).toBool())
        setWindowState(windowState() | Qt::WindowMaximized);
}
