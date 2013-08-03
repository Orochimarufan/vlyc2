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

#ifndef VIDEO_H
#define VIDEO_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#ifdef LIBVLYC2PLUGIN_LIBRARY
#define LIBVLYC2PLUGIN_EXPORT Q_DECL_EXPORT
#else
#define LIBVLYC2PLUGIN_EXPORT Q_DECL_IMPORT
#endif

class SitePlugin;
class Video;

/**
 * @brief The VideoQualityLevel enum
 * The video quality levels.
 * Try to sort your videos into these
 */
enum class VideoQualityLevel : int {
    QA_INVALID = -1,
    QA_LOWEST  = 0,
    QA_360     = 100,
    QA_480     = 200,
    QA_720     = 300,
    QA_1080    = 400,
    QA_4K      = 600,
    QA_HIGHEST = 1024
};

/**
 * @brief The VideoQuality struct
 * Describes a single quality level for a video
 */
struct LIBVLYC2PLUGIN_EXPORT VideoQuality
{
    VideoQualityLevel q;
    QString description;

    bool operator <(const VideoQuality &o) const;
    bool operator >(const VideoQuality &o) const;
};

/**
 * @brief The VideoSubtitle struct
 * Describes subtitles for a single language
 * a format of (NULL) means 'data' is an url pointing to a subtitle file
 * that VLC can understand.
 * a (NULL) language makes it invalid.
 */
struct LIBVLYC2PLUGIN_EXPORT VideoSubtitle
{
    QString language;
    QString format;
    QString data;
};

/**
 * @brief The Media struct
 * Describes a phaysical video
 */
struct LIBVLYC2PLUGIN_EXPORT Media
{
    Video *video;
    VideoQuality q;
    QUrl url;
};

/**
 * @brief The Video class
 * Describes a Video.
 * call load() to populate
 * emits loaded() when done, so you can move it to another QThread
 */
class LIBVLYC2PLUGIN_EXPORT Video : public QObject
{
    Q_OBJECT
public:
    // Data
    virtual QString videoId() const = 0;
    virtual SitePlugin *site() const = 0;

    virtual bool useVlcMeta() const;

    virtual QString title() const = 0;
    virtual QString author() const = 0;
    virtual QString description() const = 0;

    virtual int views() const = 0;
    virtual int likes() const = 0;
    virtual int dislikes() const = 0;
    virtual int favorites() const = 0;

    virtual QList<VideoQuality> available() const = 0;
    virtual QStringList availableSubtitles() const;

    virtual Media media(VideoQualityLevel q) = 0;
    virtual VideoSubtitle subtitles(QString language);

    virtual bool isDone() const = 0;
    virtual QString getError() const = 0;

public Q_SLOTS:
    virtual void load() = 0;

Q_SIGNALS:
    void done();
    void error(const QString &message);
};


class LIBVLYC2PLUGIN_EXPORT StandardVideo : public Video
{
    Q_OBJECT
protected:
    QString ms_video_id;
    SitePlugin* mp_site;

    QString ms_title;
    QString ms_author;
    QString ms_description;

    int mi_views;
    int mi_likes;
    int mi_dislikes;
    int mi_favorites;

    QList<VideoQuality> ml_available;

public:
    virtual QString videoId() const;
    virtual SitePlugin *site() const;

    virtual QString title() const;
    virtual QString author() const;
    virtual QString description() const;

    virtual int views() const;
    virtual int likes() const;
    virtual int dislikes() const;
    virtual int favorites() const;

    virtual QList<VideoQuality> available() const;

    virtual bool isDone() const;
    virtual QString getError() const;

// done magic
private:
    bool mb_done;
    QString ms_error;

private Q_SLOTS:
    void _done();
    void _error(const QString &m);

public:
    StandardVideo(SitePlugin *site, const QString &video_id);
};

#endif // VIDEO_H
