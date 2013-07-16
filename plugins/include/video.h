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

class SitePlugin;
class VlcMedia;

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
    QA_HIGHEST = 1000
};

/**
 * @brief The VideoQuality struct
 * Describes a single quality level for a video
 */
struct VideoQuality
{
    VideoQualityLevel q;
    QString description;
};

/**
 * @brief The VideoSubtitle struct
 * Describes subtitles for a single language
 * a format of (NULL) means 'data' is an url pointing to a subtitle file
 * that VLC can understand.
 * a (NULL) language makes it invalid.
 */
struct VideoSubtitle
{
    QString language;
    QString format;
    QString data;
};

/**
 * @brief The Video class
 * Describes a Video.
 * call load() to populate
 * emits loaded() when done, so you can move it to another QThread
 */
class Video : public QObject
{
    Q_OBJECT
public:
    // Data
    virtual QString videoId() const = 0;
    virtual SitePlugin *site() const = 0;

    virtual QString title() const = 0;
    virtual QString author() const = 0;
    virtual QString description() const = 0;

    virtual int likes() const = 0;
    virtual int dislikes() const = 0;
    virtual int favorites() const = 0;

    virtual QList<VideoQuality> available() const = 0;
    virtual QStringList availableSubtitles() const { return QStringList(); }

    virtual VlcMedia media(VideoQuality q) = 0;
    virtual VideoSubtitle subtitles(QString language) { return VideoSubtitle(); }

    virtual bool isDone() const = 0;

    virtual ~Video() {}

public slots:
    virtual void load() = 0;

signals:
    void loaded(Video *);
};


class StandardVideo : public Video
{
    Q_OBJECT
protected:
    QString m_video_id;
    SitePlugin* mp_site;

    QString m_title;
    QString m_author;
    QString m_description;

    int m_likes;
    int m_dislikes;
    int m_favorites;

    QList<VideoQuality> ml_available;

public:
    virtual QString videoId() const { return m_video_id; }
    virtual SitePlugin *site() const { return mp_site; }

    virtual QString title() const { return m_title; }
    virtual QString author() const { return m_author; }
    virtual QString description() const { return m_description; }

    virtual int likes() const { return m_likes; }
    virtual int dislikes() const { return m_dislikes; }
    virtual int favorites() const { return m_favorites; }

    virtual QList<VideoQuality> available() const { return ml_available; }

    virtual bool isDone() const { return mb_done; }

    virtual ~StandardVideo() {}

// done magic
private:
    bool mb_done;

private slots:
    void _loaded() { mb_done = true; }

public:
    StandardVideo() : Video(), mb_done(false) { connect(this, SIGNAL(loaded(Video*)), SLOT(_loaded())); }
};

#endif // VIDEO_H
