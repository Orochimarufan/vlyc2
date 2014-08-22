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

#include "SimplePlaylistPlugin.h"

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>

#include <QtVlc/VlcMediaPlayer.h>

#include <VlycResult/Object.h>
#include <VlycResult/Media.h>
#include <VlycResult/Menu.h>

#include "../../src/vlyc.h"
#include "../../src/logic/VlycPlayer.h"
#include "../../src/logic/PlaylistModel.h"
#include "../../src/logic/PlaylistNode.h"


QVariant save_progress(Vlyc::Result::ObjectPtr o, QVariantList args)
{
    // Mode:
    // 0 | Save the current position and time
    // 1 | Save the current position but not time
    // 2 | Save the position of the next video
    int mode = args.value(0, 0).toInt();

    QString path = o->property<QString>("vml_path");
    QFile f(path);

    VlycApp *vlyc = (VlycApp*)qApp->property("vlyc").value<QObject*>();

    PlaylistNode *self = (PlaylistNode*)o->property<void*>("__vlyc_playlist_node");
    PlaylistNode *cur = vlyc->player()->current();

    while (cur && cur->parent() != self)
        cur = cur->parent();

    if (!cur)
        return QVariant();

    int pos = mode != 2 ? cur->index() : cur->index() + 1;
    qint64 time = mode == 0 ? vlyc->player()->player().time() : 0;

    QStringList lines;
    if (!path.endsWith("/.folder.vml"))
    {
        f.open(QFile::ReadOnly | QFile::Text);

        QString line;
        while (!(line = f.readLine()).isEmpty())
            lines << line;
        f.close();

        if (lines.at(0).startsWith("#"))
            lines.takeFirst();
    }
    else
        lines << "*";

    QVariantMap meta = o->property<QVariantMap>("vml_meta");
    meta.insert("pos", pos);
    meta.insert("time", time);
    o->setProperty("vml_meta", meta);

    QJsonDocument json(QJsonObject::fromVariantMap(meta));
    lines.prepend(QStringLiteral("#%1\n").arg(QString(json.toJson(QJsonDocument::Compact))));

    f.open(QFile::WriteOnly | QFile::Text);
    for (QString line : lines)
        f.write(line.toUtf8());
    f.close();

    o->setProperty("start_index", pos);
    o->setProperty("start_pos", time);

    return QVariant();
}


SimplePlaylistPlugin::SimplePlaylistPlugin()
{
    super = new Vlyc::Result::Object{
        //{"type", "urllist"},
        {"type", "objectlist"},
        {"menu", Menu({
                          MenuEntry("save_progress", "Save Progress", QString::null, {0}),
                          MenuEntry("save_progress", "Set Progress to this Item", QString::null, {1}),
                          MenuEntry("save_progress", "Set Progress to next Item", QString::null, {2}),
                      })},
    };
    super->setMethod("save_progress", &save_progress);
    //super->setMethod("on_quit", &save_progress);
}

Vlyc::Result::ResultPtr SimplePlaylistPlugin::handleUrl(const QUrl &url)
{
    if (url.scheme() == "file")
    {
        QFileInfo f(url.path());

        QJsonObject meta;
        QStringList urls;
        QString vml_path;

        if (f.isDir())
        {
            QDir dir(f.absoluteFilePath());

            QStringList children = dir.entryList(QDir::Files | QDir::Readable, QDir::Name);
            for (QString child : children)
                urls << "file://" + dir.absoluteFilePath(child);

            QFileInfo metaFile(dir, ".folder.vml");
            QFile mf(metaFile.absoluteFilePath());
            mf.open(QFile::ReadOnly);

            meta = QJsonDocument::fromJson(mf.readLine().mid(1)).object();

            mf.close();

            vml_path = metaFile.absoluteFilePath();
        }

        else if (f.fileName().endsWith(".vml"))
        {
            QFile f(url.path());
            f.open(QFile::ReadOnly | QFile::Text);

            QString line = f.readLine();
            int startPos = 0;

            // Read meta information
            if (line.startsWith('#'))
            {
                QString data = line.mid(1);

                bool ok;
                startPos = data.toInt(&ok);

                if (ok)
                    meta.insert("pos", startPos);
                else
                    meta = QJsonDocument::fromJson(data.toUtf8()).object();
            }
            else
                f.seek(0);

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

            vml_path = f.fileName();
        }

        else
            return nullptr;



        QVariantList l;
        for (QString url : urls)
            l << QVariant::fromValue(Vlyc::Result::ResultPtr(Vlyc::Result::File(url)));

        return new Vlyc::Result::Object(super, {
            {"name", meta.value("name").toString(url.fileName())},
            {"start_index", meta.value("pos").toInt(0)},
            {"start_time", meta.value("time").toInt(0)},
            {"urls", urls},
            {"children", l},
            {"vml_path", vml_path},
            {"vml_meta", meta.toVariantMap()},
        });
    }

    return nullptr;
}
