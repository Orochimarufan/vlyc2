/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#pragma once

#include <video.h>
#include <QtVlc/VlcMedia.h>

#include <VlycResult/Promise.h>

#include "TempEventLoop.h"

class LegacyVideoResult : public Vlyc::Result::Result
{
    VideoPtr mp_video;

public:
    LegacyVideoResult(VideoPtr video);
    VideoPtr video();
};

class LegacyVideoPromise : public QObject, public Vlyc::Result::Promise
{
    Q_OBJECT
    VideoPtr mp_video;

public:
    LegacyVideoPromise(VideoPtr video);
    VideoPtr video();

private:
    void work() override;

private slots:
    void done();
    void error(QString message);
};

std::tuple<QList<QString>, QList<int>> __lv_qualities(VideoPtr v);

class __lv_get_media : public TempEventLoop
{
    Q_OBJECT
public:
    QUrl url;
    VlcMedia operator()(VideoPtr, int);
public slots:
    void media(const VideoMedia &);
};
