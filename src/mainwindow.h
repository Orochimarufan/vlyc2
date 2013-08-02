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
#include <QtWidgets/QShortcut>

#include <video.h>

namespace Ui {
class MainWindow;
}

class FullScreenController;

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;
class Video;
class Vlyc;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(Vlyc *self);
    ~MainWindow();

public slots:
    void playVideo(Video *);

    void setFullScreen(bool fs);
    bool toggleFullScreen();

private slots:
    void _playVideo();
    void _videoError(const QString &);
    void openUrl();
    void browser();
    void updatePosition(const float &);
    void updateState(const VlcState::Type &);
    void on_position_sliderDragged(const float &);
    void on_quality_currentIndexChanged(const int &);
    void mediaChanged(libvlc_media_t *media);

    void setFullScreenVideo(bool fs);

    void saveState();
    void loadState();

protected:
    bool eventFilter(QObject *, QEvent *);
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    
private:
    Vlyc *mp_self;
    Ui::MainWindow *ui;
    FullScreenController *fsc;

    bool block_changed;
    friend class BlockChanged;

    VlcMedia m_media;
    VlcMediaPlayer m_player;
    VlcMediaPlayerAudio m_audio;

    Video* mp_video;
    QList<VideoQuality> ml_qa;

    QShortcut *shortcut_Space;
    QShortcut *shortcut_Esc;
    QShortcut *shortcut_F11;
    QShortcut *shortcut_AltReturn;

    QRect m_geometry;
};

#endif // MAINWINDOW_H
