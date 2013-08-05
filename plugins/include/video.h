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
#include <QtCore/QVariant>

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
    bool operator ==(const VideoQuality &o) const;
    bool operator <=(const VideoQuality &o) const;
    bool operator >=(const VideoQuality &o) const;
};

/**
 * @brief The VideoSubtitle struct
 * Describes subtitles for a single language
 * a (NULL) language makes it invalid.
 */
struct LIBVLYC2PLUGIN_EXPORT VideoSubtitles
{
    Video *video;       ///< The Video these subtitles belong to
    QString language;   ///< The subtitle language
    QString type;       ///< The subtitle type/format. e.g. SRT, ASS; may be empty for external resources
    QVariant data;      /**< The subtitles.
                             May be
                               QUrl: load it from an external resouce
                               QByteArray: load it from a blob
                               QString: load it from string data.
                        */
};

/**
 * @brief The Media struct
 * Describes a phaysical video
 */
struct LIBVLYC2PLUGIN_EXPORT VideoMedia
{
    Video *video; ///< The Video this URL belongs to
    VideoQuality q; ///< The quality
    QUrl url; ///< The URL
};

/**
 * @brief The Video class [pure virtual]
 * Describes a Video.
 *
 * call the load() slot before accessing data
 * emit done() when load() is finished.
 * emit error(QString) if an error occurred
 * DO NOT DEPEND ON IT BEING DONE AFTER load() RETURNS!
 * therefore, any implementation MUST emit either done() or error().
 *
 * may be moved to a QThread.
 *
 * getMedia() and getSubtitles() must be called after load() and work in the same fashion.
 * getMedia() returns by emitting media(VideoMedia), or error() on error
 * getSubtitles() return by emitting subtitles(VideoSubtitles), or error() on error
 */
class LIBVLYC2PLUGIN_EXPORT Video : public QObject
{
    Q_OBJECT
public:
    /// The site-specific VideoID.
    virtual QString videoId() const = 0;

    /// The Site Plugin the video belongs to.
    virtual SitePlugin *site() const = 0;


    /// Wether to use the file metadata for title & co.
    virtual bool useFileMetadata() const = 0;


    /// The video title.
    virtual QString title() const = 0;

    /// The video author.
    virtual QString author() const = 0;

    /// The video description
    virtual QString description() const = 0;


    /// The number of video views.
    virtual int views() const = 0;

    /// The number of likes.
    virtual int likes() const = 0;

    /// The number of dislikes.
    virtual int dislikes() const = 0;

    // The number of users that added this video to their favorites.
    virtual int favorites() const = 0;


    /// A list of available quality levels.
    virtual QList<VideoQuality> availableQualities() const = 0;

    /// A list of available subtitle languages.
    virtual QStringList availableSubtitleLanguages() const = 0;


    /// The last error message.
    virtual QString getError() const = 0;

public Q_SLOTS:
    /// Initialize this video.
    virtual void load() = 0;

    /// Get the URL for a specific quality level.
    virtual void getMedia(const VideoQualityLevel &q) = 0;

    /// Get the subtitles for a specific language.
    virtual void getSubtitles(const QString &language) = 0;

Q_SIGNALS:
    /// emitted when load() finishes.
    void done();

    /// emitted when getMedia() returns.
    void media(const VideoMedia &media);

    /// emitted when getSubtitles() returns.
    void subtitles(const VideoSubtitles &subs);

    /// emitted when either of the above slots error.
    void error(const QString &message);
};

/**
 * @brief The StandardVideo class [abstract]
 * Partial reference implementation for Video
 */
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

    QList<VideoQuality> ml_availableQualities;
    QStringList ml_availableSubtitleLanguages;

public:
    StandardVideo(SitePlugin *site, const QString &video_id);

    virtual QString videoId() const;
    virtual SitePlugin *site() const;

    // default implementation returns false
    virtual bool useFileMetadata() const;

    // default implementations return corresponding member
    virtual QString title() const;
    virtual QString author() const;
    virtual QString description() const;

    virtual int views() const;
    virtual int likes() const;
    virtual int dislikes() const;
    virtual int favorites() const;

    virtual QList<VideoQuality> availableQualities() const;
    virtual QStringList availableSubtitleLanguages() const;

    // default implementation returns invalid VideoSubtitles. should be overridden
    virtual void getSubtitles(const QString &language);

// getError() magic
    virtual QString getError() const;

private:
    QString ms_error;

private Q_SLOTS:
    void _error(const QString &m);
};

#endif // VIDEO_H
