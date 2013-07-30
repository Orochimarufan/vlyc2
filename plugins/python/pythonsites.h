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

#ifndef PYTHONSITES_H
#define PYTHONSITES_H

#include <PythonQt/PythonQt.h>

#include <siteplugin.h>

class PythonVideo : public StandardVideo
{
    Q_OBJECT
    friend class PythonSitePlugin;
    friend class PythonVideoWrapper;
public:
    PythonVideo(SitePlugin *site, QString videoId);
    QMap<VideoQualityLevel, Media> m_urls;
    virtual Media media(VideoQualityLevel q);
    virtual void load();
};

class PythonSitePlugin : public QObject, public SitePlugin
{
    Q_OBJECT
    Q_INTERFACES(VlycBasePlugin SitePlugin)
public:
    PythonSitePlugin(QString name, QString author, int rev, PyObject *fn_forUrl, PyObject *fn_video);

    virtual QString forUrl(QUrl url);
    virtual Video* video(QString video_id);

    PythonQtObjectPtr mof_forUrl;
    PythonQtObjectPtr mof_video;
    QString ms_name;
    QString ms_author;
    int mi_rev;

    virtual QString name() { return ms_name; }
    virtual QString author() { return ms_author; }
    virtual int rev() { return mi_rev; }
};

class PythonVideoWrapper : public QObject
{
    Q_OBJECT
    friend class PythonVideo;
    PythonVideoWrapper(PythonVideo *v);
    PythonVideo *mp_vid;

public:
    PythonVideoWrapper();
    PythonVideoWrapper(const PythonVideoWrapper &w);
    ~PythonVideoWrapper();

public Q_SLOTS:
    QString videoId() const;
    void setTitle(const QString &title);
    void setAuthor(const QString &author);
    void setDescription(const QString &description);
    void setLikes(const int &likes);
    void setDislikes(const int &dislikes);
    void setFavorites(const int &favorites);
    void addQuality(const int &level, const QString &descr, const QString &url);
    void error(QString message);
    QString getError();
    bool isDone();
};

#endif // PYTHONSITES_H
