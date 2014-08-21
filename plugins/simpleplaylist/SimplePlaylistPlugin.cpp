#include "SimplePlaylistPlugin.h"

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>

#include <VlycResult/Object.h>
#include <VlycResult/Media.h>

#include "../../src/vlyc.h"
#include "../../src/logic/VlycPlayer.h"
#include "../../src/logic/PlaylistModel.h"
#include "../../src/logic/PlaylistNode.h"


QVariant save_progress(Vlyc::Result::ObjectPtr o, QVariantList args)
{
    // Args is empty
    QString path = o->property<QString>("vml_path");
    QFile f(path);
    VlycApp *vlyc = (VlycApp*)qApp->property("vlyc").value<QObject*>();
    PlaylistNode *cur = vlyc->player()->current();
    PlaylistNode *self = (PlaylistNode*)o->property<void*>("__vlyc_playlist_node");
    while (cur && cur->parent() != self)
        cur = cur->parent();
    if (!cur)
        return QVariant();
    f.open(QFile::ReadOnly | QFile::Text);
    QString line;
    QStringList lines;
    while (!(line = f.readLine()).isEmpty())
        lines << line;
    if (lines.at(0).startsWith("#"))
        lines.takeFirst();
    lines.prepend(QStringLiteral("#%1\n").arg(cur->index()));
    f.close();
    f.open(QFile::WriteOnly | QFile::Text);
    for (QString line : lines)
        f.write(line.toUtf8());
    f.close();
    o->setProperty("start_index", QVariant::fromValue(cur->index()));
    return QVariant();
}


SimplePlaylistPlugin::SimplePlaylistPlugin()
{
    super = new Vlyc::Result::Object{
    //{"type", "urllist"},
    {"type", "objectlist"},
    {"library_context_menu", QVariantHash({
                                             {"save_progress", "Save progress"}
                                         })}
};
    super->setMethod("save_progress", &save_progress);
}

Vlyc::Result::ResultPtr SimplePlaylistPlugin::handleUrl(const QUrl &url)
{
    if (url.scheme() == "file" && url.fileName().endsWith(".vml"))
    {
        QFile f(url.path());
        f.open(QFile::ReadOnly | QFile::Text);
        QString line = f.readLine();
        int startPos = 0;
        if (line.startsWith('#'))
            startPos = line.mid(1).toInt();
        else
            f.seek(0);
        QStringList urls;
        QDir dir(QFileInfo(url.path()).absolutePath());
        while (!(line = f.readLine()).isEmpty())
        {
            QFileInfo fi(dir, line.trimmed());
            if (!fi.isFile())
                continue;
            QUrl x;
            x.setScheme("file");
            x.setPath(fi.absoluteFilePath());
            urls << x.toString();
        }
        f.close();
        QVariantList l;
        for (QString url : urls)
            l << QVariant::fromValue(Vlyc::Result::ResultPtr(Vlyc::Result::File(url)));
        return new Vlyc::Result::Object(super, {
            {"name", url.fileName()},
            {"start_index", startPos},
            //{"urls", urls},
            {"children", l},
            {"vml_path", url.path()}
        });
    }
    return nullptr;
}
