#include "testplugin2.h"

TestPlugin2::TestPlugin2() :
    QObject()
{
}

void TestPlugin2::init(Vlyc::InitEvent &init)
{
    Plugin::init(init);
}

#include <VlycResult/Playlist.h>
#include <VlycResult/Url.h>

Vlyc::Result::ResultPtr TestPlugin2::handleUrl(const QUrl &url)
{
    if (url.scheme() != "test")
        return nullptr;

    if (url.host() == "playlist")
    {
        Vlyc::Result::StandardPlaylist *playlist = new Vlyc::Result::StandardPlaylist(1);
        *playlist << new Vlyc::Result::Url("https://www.youtube.com/watch?v=pv5bD2w456Q");
        *playlist << new Vlyc::Result::Url("https://www.youtube.com/watch?v=btqtftCnC-s");
        return playlist;
    }
    else if (url.host() == "url")
    {
        return new Vlyc::Result::Url("https://www.youtube.com/watch?v=baJpkNC_ilY");
    }
    else if (url.host() == "all")
    {
        Vlyc::Result::StandardPlaylist *all = new Vlyc::Result::StandardPlaylist;
        *all << new Vlyc::Result::Url("test://url");
        *all << new Vlyc::Result::Url("test://playlist");
        return all;
    }

    return nullptr;
}
