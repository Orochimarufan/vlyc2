#include "testplugin2.h"

TestPlugin2::TestPlugin2() :
    QObject()
{
}

void TestPlugin2::init(Vlyc::InitEvent &init)
{
    Plugin::init(init);
}

#include <VlycResult/Url.h>
#include <VlycResult/Media.h>
#include <VlycResult/ObjectList.h>

Vlyc::Result::ResultPtr TestPlugin2::handleUrl(const QUrl &url)
{
    if (url.scheme() != "test")
        return nullptr;

    if (url.host() == "playlist")
        return new Vlyc::Result::ObjectList({
                    {"name", "ObjectList Test"},
                    {"start_index", 1}
                }, {
                   new Vlyc::Result::Url("https://www.youtube.com/watch?v=pv5bD2w456Q"),
                   new Vlyc::Result::Url("https://www.youtube.com/watch?v=btqtftCnC-s")
                });

    else if (url.host() == "url")
        return new Vlyc::Result::Url("https://www.youtube.com/watch?v=baJpkNC_ilY");

    else if (url.host() == "all")
        return Vlyc::Result::UrlList({
            "test://url",
            "test://playlist"
        });

    else if (url.host() == "8dayz")
        return Vlyc::Result::File("file:///run/media/hinata/TOSHIBA EXT/메건리(MEGANLEE)  - 8dayz M_V (뮤직비디오)-jhlZuTn04w4.mp4");

    return nullptr;
}
