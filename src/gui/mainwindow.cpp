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
#include "about.h"
#include <video.h>

#include <VlycPluginManager.h>
#include <VlycToolPlugin.h>

#include <QtCore>
#include <QInputDialog>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTemporaryFile>
#include <QFileDialog>

#include "logic/VlycPlayer.h"
#include "logic/PlaylistModel.h"
#include "logic/PlaylistNode.h"

#ifdef Q_OS_LINUX
#include "vlyc_xcb.h"
#endif

// ----------------------------------------------------------------------------
// Legacy Video helpers
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// MainWindow
// ----------------------------------------------------------------------------
MainWindow::MainWindow(VlycApp *self) :
    QMainWindow(),
    ui(new Ui::MainWindow),
    mp_self(self),
    m_player(self->player()->player()),
    m_player_audio(m_player),
    m_player_video(m_player),
    m_repeat_mode(0)
{
#ifdef Q_OS_LINUX
    if (qApp->platformName() == "xcb")
        XCB::setWMClass(winId(), qApp->applicationName(), qApp->applicationName());
#endif

    // Setup UI
    ui->setupUi(this);
    fsc = new FullScreenController(ui->video);
    ui->video->installEventFilter(this);

    ui->treeView->setModel(&self->player()->model());

    connectUiMisc();

    // Setup vlc
    setupPlayer();

    loadState();
}

void MainWindow::addPluginActions()
{
    for (Vlyc::ToolPlugin *p : mp_self->plugins()->getPlugins<Vlyc::ToolPlugin>())
        ui->menuTools->addAction(p->toolMenuAction());
}

MainWindow::~MainWindow()
{
    delete fsc;
    delete ui;
}

// ----------------------------------------------------------------------------
// Video stuff
// ----------------------------------------------------------------------------
void MainWindow::updateQualityList(QList<QString> qa, int current)
{
    BlockChanged block(this);
    ui->quality->clear();
    ui->quality->addItems(qa);
    ui->quality->setCurrentIndex(current);
    ui->quality->setEnabled(qa.length() > 1);
}

void MainWindow::updateSubsList(QList<QString> subs, int current)
{
    BlockChanged block(this);
    ui->subtitles->clear();
    ui->subtitles->addItems(subs);
    ui->subtitles->setCurrentIndex(current);
    ui->subtitles->setEnabled(subs.length() > 1);
}

void MainWindow::on_quality_currentIndexChanged(const int &index)
{
    if (block_changed) return;
    mp_self->player()->setQuality(index);
}

void MainWindow::on_subtitles_currentIndexChanged(const int &index)
{
    if (block_changed) return;
    mp_self->player()->setSubtitles(index);
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->video && e->type() == QEvent::MouseButtonDblClick)
        toggleFullScreen();
    return false;
}

// ----------------------------------------------------------------------------
// Video Player
// ----------------------------------------------------------------------------
void MainWindow::setupPlayer()
{
    m_player.setVideoDelegate(ui->video);

    //connect(&m_player, &VlcMediaPlayer::endReached, &m_player, &VlcMediaPlayer::stop);
    connect(&m_player, &VlcMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    //connect(&m_player, &VlcMediaPlayer::mediaChanged, this, &MainWindow::updateMedia);
    connect(&m_player, SIGNAL(mediaChanged(libvlc_media_t*)), this, SLOT(updateMedia(libvlc_media_t*)));
    connect(&m_player, &VlcMediaPlayer::stateChanged, this, &MainWindow::updateState);
    connect(mp_self->player(), &VlycPlayer::qualityListChanged, this, &MainWindow::updateQualityList);
    connect(mp_self->player(), &VlycPlayer::subsListChanged, this, &MainWindow::updateSubsList);
}

void MainWindow::updateMedia(libvlc_media_t *media)
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


// ----------------------------------------------------------------------------
// Ui events
// ----------------------------------------------------------------------------
void MainWindow::connectUiMisc()
{
    // Menu
    connect(ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);

    // Ui
    connect(ui->btn_stop, &QAbstractButton::clicked, &m_player, &VlcMediaPlayer::stop);
    connect(ui->position, &SeekSlider::sliderDragged, &m_player, &VlcMediaPlayer::setPosition);
    connect(ui->volume, &SoundWidget::volumeChanged, &m_player_audio, &VlcMediaPlayerAudio::setVolume);
    connect(ui->volume, &SoundWidget::muteChanged, &m_player_audio, &VlcMediaPlayerAudio::setMuted);
    connect(ui->btn_fullscreen, &QAbstractButton::clicked, this, &MainWindow::toggleFullScreen);
    connect(ui->btn_next, &QAbstractButton::clicked, mp_self->player(), &VlycPlayer::next);
    connect(ui->btn_prev, &QAbstractButton::clicked, mp_self->player(), &VlycPlayer::prev);
    connect(ui->treeView, &QAbstractItemView::activated, mp_self->player(), &VlycPlayer::playNow);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::onLibraryContextMenu);

    // Fullscreen
    connect(fsc->ui->btn_playpause, &QAbstractButton::clicked, this, &MainWindow::on_btn_play_clicked);
    connect(fsc->ui->btn_stop, &QAbstractButton::clicked, &m_player, &VlcMediaPlayer::stop);
    connect(fsc->ui->position, &SeekSlider::sliderDragged, &m_player, &VlcMediaPlayer::setPosition);
    connect(fsc->ui->volume, &SoundWidget::volumeChanged, &m_player_audio, &VlcMediaPlayerAudio::setVolume);
    connect(fsc->ui->volume, &SoundWidget::muteChanged, &m_player_audio, &VlcMediaPlayerAudio::setMuted);
    connect(fsc->ui->btn_defullscreen, &QAbstractButton::clicked, [=] () {setFullScreen(false);});

    // Shortcuts
    shortcut_Space = new QShortcut(QKeySequence(" "), ui->video);
    connect(shortcut_Space, &QShortcut::activated, &m_player, &VlcMediaPlayer::togglePause);

    shortcut_F11 = new QShortcut(QKeySequence("F11"), ui->video);
    connect(shortcut_F11, &QShortcut::activated, this, &MainWindow::toggleFullScreen);

    shortcut_AltReturn = new QShortcut(QKeySequence("Alt + Return"), ui->video);
    connect(shortcut_AltReturn, &QShortcut::activated, this, &MainWindow::toggleFullScreen);

    shortcut_Esc = new QShortcut(QKeySequence("Esc"), ui->video);
    connect(shortcut_Esc, &QShortcut::activated, [=] () {setFullScreen(false);});

    shortcut_n = new QShortcut(QKeySequence("N"), ui->video);
    connect(shortcut_n, &QShortcut::activated, mp_self->player(), &VlycPlayer::next);

    connect(new QShortcut(QKeySequence("P"), ui->video), &QShortcut::activated, mp_self->player(), &VlycPlayer::prev);
    connect(new QShortcut(QKeySequence("O"), ui->video), &QShortcut::activated, [this](){m_player.setTime(90000);});
    connect(new QShortcut(QKeySequence("I"), ui->video), &QShortcut::activated, [this](){m_player.setTime(m_player.time() + 80000);});
    connect(new QShortcut(QKeySequence("F"), ui->video), &QShortcut::activated, [this](){m_player.setTime(m_player.time());});
}

void MainWindow::on_btn_play_clicked()
{
    switch(m_player.state()) {
    case VlcState::Opening:
    case VlcState::Buffering:
    case VlcState::Playing:
        m_player.pause();
        break;
    case VlcState::Paused:
    case VlcState::Stopped:
    case VlcState::Ended:
    case VlcState::NothingSpecial:
        mp_self->player()->play();
        break;
    case VlcState::Error:
        break;
    }
}

void MainWindow::on_actionOpenFile_triggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, "Open File", history.lastFileOpenDir());

    if (url.isEmpty())
        return;

	QString p = QFileInfo(url.toLocalFile()).dir().path();
	qDebug() << "Path:" << p;
    history.setFileOpenDir(p);

    addRecent(url);

    mp_self->play(url);
}

void MainWindow::on_actionOpenFolder_triggered()
{
    QUrl folder = QFileDialog::getExistingDirectoryUrl(this, "Open Folder", history.lastFileOpenDir());

    history.setFileOpenDir(QFileInfo(folder.path()).dir().path());

    addRecent(folder);
    mp_self->play(folder);
}

void MainWindow::on_actionOpenURL_triggered()
{
    QString url = QInputDialog::getText(this, tr("Open Url"), tr("Enter URL"));

    if (url.isEmpty())
        return;

    mp_self->play(url);
}

void MainWindow::on_actionAbout_triggered()
{
    About dialog(this);
    dialog.exec();
}

void MainWindow::on_btn_library_clicked(bool checked)
{
    ui->stackedWidget->setCurrentIndex(checked ? 1 : 0);
    // Move Video
    if (checked)
    {
        QGridLayout *layout = ((QGridLayout*)ui->pageLibrary->layout());
        layout->addWidget(ui->video, 1, 0);
    }
    else
    {
        QBoxLayout *layout = (QBoxLayout*)ui->pageVideo->layout();
        layout->addWidget(ui->video);
    }
}

/**
 * @brief Populate a QMenu from a node's menu definition called "menu_key"
 * @param menu The QMenu to populate
 * @param node The PlaylistNode the menu belongs to
 * @param menu_key The name of the menu
 */
inline void populate_menu(QMenu &menu, PlaylistNode* node, QString menu_key)
{
    for (QVariant e : node->property2<QVariantList>(menu_key))
    {
        QVariantMap entry = e.toMap();

        QAction *a = menu.addAction(entry["text"].toString());
        if (entry["action"].toString() == "call" && node->hasMethod(entry["method"].toString()))
            a->connect(a, &QAction::triggered, [entry, node](){
                node->call(entry["method"].toString(), entry.value("args").toList());
            });
        else
            a->setEnabled(false);
    }
}

void MainWindow::onLibraryContextMenu(const QPoint &point)
{
    QMenu menu(ui->treeView);

    if (ui->treeView->selectionModel()->selectedRows().length() < 2)
    {
        PlaylistNode *node = mp_self->player()->model().getNodeFromIndex(ui->treeView->indexAt(point));

        if (node->hasFailed())
        {
            QAction *a = menu.addAction("Retry");
            connect(a, &QAction::triggered, [node, this] () {});
        }

        if (node->hasProperty("library_context_menu"))
        {
            populate_menu(menu, node, "library_context_menu");
            menu.addSeparator();
        }
        else if (node->hasProperty("menu"))
        {
            populate_menu(menu, node, "menu");
            menu.addSeparator();
        }
    }

    QAction *a = menu.addAction("Remove");
    connect(a, &QAction::triggered, [this](){
        for (QModelIndex index : ui->treeView->selectionModel()->selectedIndexes())
            mp_self->player()->remove(index);
    });
    a = menu.addAction("Clear Playlist", mp_self->player(), SLOT(clearPlaylist()));

    menu.exec(ui->treeView->mapToGlobal(point));
}

void MainWindow::on_btn_menu_clicked()
{
    QMenu menu(ui->btn_menu);
    PlaylistNode *node = mp_self->player()->current();

    while (node)
    {
        if (node->hasProperty("video_menu"))
        {
            if (node != mp_self->player()->current())
                menu.addSection(node->displayName());

            populate_menu(menu, node, "video_menu");
        }
        else if (node->hasProperty("menu"))
        {
            if (node != mp_self->player()->current())
                menu.addSection(node->displayName());

            populate_menu(menu, node, "menu");
        }

        node = node->parent();
    }

    if (menu.isEmpty())
        menu.addSection("No Actions available for video.");

    ui->btn_menu->setMenu(&menu);
    ui->btn_menu->showMenu();
    ui->btn_menu->setMenu(nullptr);
}

void MainWindow::on_btn_repeat_clicked()
{
    if (++m_repeat_mode > 2)
        m_repeat_mode = 0;

    ui->btn_repeat->updateButtonIcons(m_repeat_mode);

    mp_self->player()->setPlaybackFlags((PlaybackFlags)m_repeat_mode); // Add shuffle
}

// ----------------------------------------------------------------------------
// Fullscreen
// ----------------------------------------------------------------------------
void MainWindow::setFullScreenVideo(bool fs)
{
    // Reparenting breaks stuff for some reason.
    // therefore we modify the MainWindow to
    // only show the video part and fullscreen that.
    if (fs) {
        // always fullscreen the video page
        if (ui->btn_library->isChecked())
            on_btn_library_clicked(false);
        //ui->video->setParent(NULL, Qt::Window);
        //ui->video->showFullScreen();
        showFullScreen();
        ui->menubar->setVisible(false);
        ui->statusbar->setVisible(false);
        ui->controls->setVisible(false);
    } else {
        //ui->video->setParent(ui->centralwidget, 0);
        //ui->verticalLayout->insertWidget(0, ui->video);
        showNormal();
        ui->menubar->setVisible(true);
        ui->statusbar->setVisible(true);
        ui->controls->setVisible(true);
        // restore library view
        if (ui->btn_library->isChecked())
            on_btn_library_clicked(true);
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

bool MainWindow::toggleFullScreen()
{
    setFullScreen(!fsc->mb_fullscreen);
    return fsc->mb_fullscreen;
}

// ----------------------------------------------------------------------------
// Preserve window state
// ----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);
    saveState();
    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (~windowState() & Qt::WindowMaximized && !fsc->mb_fullscreen)
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

    // Recent files
    ui->actionClearRecent->setParent(this);
    ui->menuRecent->clear();
    for (QString file : history.recentFiles())
        addRecent(file, true);
    ui->menuRecent->addSeparator();
    ui->menuRecent->addAction(ui->actionClearRecent);
}

void MainWindow::addRecent(const QUrl &url, bool restoring)
{
    // Save it to the config
    if (!restoring)
        history.addRecentFile(url.toString());

    // Add the Menu entry
    QString name = url.scheme() == "file" ? url.fileName() : url.toString();

    QAction *action = new QAction(name, ui->menuRecent);
    action->setData(url);
    connect(action, &QAction::triggered, this, &MainWindow::openRecent);

    // Insert it on the top
    if (!restoring)
        ui->menuRecent->insertAction(ui->menuRecent->actions().at(0), action);
    // Insert it above the separator
    else
        ui->menuRecent->addAction(action);

    // Enable the menu, if this was the first entry
    ui->menuRecent->setEnabled(true);
}

void MainWindow::openRecent()
{
    mp_self->play(static_cast<QAction*>(sender())->data().toUrl());
}

void MainWindow::on_actionClearRecent_triggered()
{
    history.clearRecentFiles();
    ui->menuRecent->clear();
    ui->menuRecent->addSeparator();
    ui->menuRecent->addAction(ui->actionClearRecent);
    ui->menuRecent->setEnabled(false);
}
