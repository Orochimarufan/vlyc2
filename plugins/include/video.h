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
    inline VideoQuality() : q(VideoQualityLevel::QA_INVALID) {}
    inline VideoQuality(VideoQualityLevel q, QString description) : q(q), description(description) {}
    inline VideoQuality(const VideoQuality &q) : q(q.q), description(q.description) {}
    inline VideoQuality &operator =(const VideoQuality &q) { this->q = q.q; description = q.description; }
    inline bool operator <(const VideoQuality &o) const { return q<o.q; }
    inline bool operator >(const VideoQuality &o) const { return q>o.q; }
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
 * @brief The Media struct
 * Describes a phaysical video
 */
struct Media
{
    Video *video;
    VideoQuality q;
    QUrl url;
    inline Media() : video(NULL) {}
    inline Media(Video *vid, VideoQuality q, const QUrl &url) : video(vid), q(q), url(url) {}
    inline Media(const Media &m) : video(m.video), q(m.q), url(m.url) {}
    inline Media &operator =(const Media &m) { video = m.video; q = m.q; url = m.url; }
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

    virtual bool useVlcMeta() const { return false; }

    virtual QString title() const = 0;
    virtual QString author() const = 0;
    virtual QString description() const = 0;

    virtual int likes() const = 0;
    virtual int dislikes() const = 0;
    virtual int favorites() const = 0;

    virtual QList<VideoQuality> available() const = 0;
    virtual QStringList availableSubtitles() const { return QStringList(); }

    virtual Media media(VideoQualityLevel q) = 0;
    virtual VideoSubtitle subtitles(QString language) { return VideoSubtitle(); }

    virtual bool isDone() const = 0;
    virtual QString getError() const = 0;

    virtual ~Video() {}

public Q_SLOTS:
    virtual void load() = 0;

Q_SIGNALS:
    void done();
    void error(const QString &message);
};


class StandardVideo : public Video
{
    Q_OBJECT
protected:
    QString ms_video_id;
    SitePlugin* mp_site;

    QString ms_title;
    QString ms_author;
    QString ms_description;

    int mi_likes;
    int mi_dislikes;
    int mi_favorites;

    QList<VideoQuality> ml_available;

public:
    virtual QString videoId() const { return ms_video_id; }
    virtual SitePlugin *site() const { return mp_site; }

    virtual QString title() const { return ms_title; }
    virtual QString author() const { return ms_author; }
    virtual QString description() const { return ms_description; }

    virtual int likes() const { return mi_likes; }
    virtual int dislikes() const { return mi_dislikes; }
    virtual int favorites() const { return mi_favorites; }

    virtual QList<VideoQuality> available() const { return ml_available; }

    virtual bool isDone() const { return mb_done; }
    virtual QString getError() const { return ms_error; }

    virtual ~StandardVideo() {}

// done magic
private:
    bool mb_done;
    QString ms_error;

private Q_SLOTS:
    void _done() { mb_done = true; }
    void _error(const QString &m) { mb_done = true; ms_error = m; }

public:
    StandardVideo() : Video(), mb_done(false) {
        connect(this, SIGNAL(done()), SLOT(_done()));
        connect(this, SIGNAL(error(QString)), SLOT(_error(QString)));
    }
};

#endif // VIDEO_H
