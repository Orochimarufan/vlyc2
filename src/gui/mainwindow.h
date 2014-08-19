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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtVlc/VlcMediaPlayer.h>
#include <QtVlc/VlcMedia.h>
#include <QtVlc/VlcMediaPlayerAudio.h>
#include <QtVlc/VlcMediaPlayerVideo.h>
#include <QtWidgets/QShortcut>

#include <VlycResult/Result.h>

#include <video.h>

#include <util/History.h>

namespace Ui {
class MainWindow;
}

class FullScreenController;

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;
class VlycApp;

using namespace Vlyc::Result;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(VlycApp *self);
    ~MainWindow();

    void addPluginActions();

public slots:
    void setFullScreen(bool fs);
    bool toggleFullScreen();

private slots:
    void updateQualityList(QList<QString> qa, int current);
    void updateSubsList(QList<QString> subs, int current);

    void updatePosition(const float &);
    void updateState(const VlcState::Type &);
    void updateMedia(libvlc_media_t *media);

    void setFullScreenVideo(bool fs);

    void saveState();
    void loadState();

    void on_actionAbout_triggered();
    void on_actionOpenFile_triggered();
    void on_actionOpenURL_triggered();

    void on_btn_play_clicked();
    void on_quality_currentIndexChanged(const int &);
    void on_subtitles_currentIndexChanged(const int &);

    void on_btn_repeat_clicked();

    void on_btn_library_clicked(bool checked);

    void onLibraryContextMenu(const QPoint &point);

    void openRecent();
    void addRecent(const QUrl &url, bool restoring=false);
    void on_actionClearRecent_triggered();

protected:
    bool eventFilter(QObject *, QEvent *);
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    
private:
    void connectUiMisc();
    void setupPlayer();

    Ui::MainWindow *ui;
    FullScreenController *fsc;

    VlycApp *mp_self;

    bool block_changed;
    friend class BlockChanged;
    friend class VlycApp;

    VlcMediaPlayer m_player;
    VlcMediaPlayerAudio m_player_audio;
    VlcMediaPlayerVideo m_player_video;

    QShortcut *shortcut_Space;
    QShortcut *shortcut_Esc;
    QShortcut *shortcut_F11;
    QShortcut *shortcut_AltReturn;
    QShortcut *shortcut_n;

    QRect m_geometry;

    History history;
    int m_repeat_mode;
};

#endif // MAINWINDOW_H
