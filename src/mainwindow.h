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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtVlc/VlcMediaPlayer.h>
#include <QtVlc/VlcMedia.h>
#include <QtVlc/VlcMediaPlayerAudio.h>

namespace Ui {
class MainWindow;
}

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

private slots:
    void openUrl();
    void browser();
    void updatePosition(float);
    void updateState(const VlcState::Type &);
    
private:
    Vlyc *mp_self;
    Ui::MainWindow *ui;

    VlcMedia m_media;
    VlcMediaPlayer m_player;
    VlcMediaPlayerAudio m_audio;

    Video* mp_video;
};

#endif // MAINWINDOW_H
