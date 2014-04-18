#pragma once

#include "TempEventLoop.h"
#include <video.h>
#include <VlcMedia.h>

struct __lv_get_media : TempEventLoop
{
    QUrl url;
    VlcMedia operator()(VideoPtr, int);
public slots:
    void media(const VideoMedia &);
};
