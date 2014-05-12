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

#ifndef VLYC2PLUGIN_VIDEO_H
#define VLYC2PLUGIN_VIDEO_H

#ifdef LIBVLYC2_LIBRARY
#define LIBVLYC2_EXPORT Q_DECL_EXPORT
#else
#define LIBVLYC2_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QAtomicInt>

#include <VlycMem/Object.h>

class SitePlugin;
class Video;

/// Smart pointer to a video instance. for weak refs use QWeakPointer(QObject *)
typedef Vlyc::Memory::Pointer<Video> VideoPtr;

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
struct LIBVLYC2_EXPORT VideoQuality
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
struct LIBVLYC2_EXPORT VideoSubtitles
{
    VideoPtr video;       ///< The Video these subtitles belong to
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
struct LIBVLYC2_EXPORT VideoMedia
{
    VideoPtr video; ///< The Video this URL belongs to
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
class LIBVLYC2_EXPORT Video : public QObject, public Vlyc::Memory::Object
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

    /// Wether users may download this video.
    virtual bool mayBeDownloaded() const = 0;


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

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Video, "me.sodimm.oro.vlyc2.Video/1.0")
QT_END_NAMESPACE

#endif // VLYC2PLUGIN_VIDEO_H
