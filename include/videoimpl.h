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

#ifndef VLYC2PLUGIN_VIDEOIMPL_H
#define VLYC2PLUGIN_VIDEOIMPL_H

#include <video.h>

/**
 * @brief The StandardVideo class [abstract]
 * Partial reference implementation for Video
 */
class LIBVLYC2_EXPORT StandardVideo : public Video
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

    virtual bool mayBeDownloaded() const;

    // default implementation returns invalid VideoSubtitles. should be overridden
    virtual void getSubtitles(const QString &language);

// getError() magic
    virtual QString getError() const;

private:
    QString ms_error;

private Q_SLOTS:
    void _error(const QString &m);
};

#endif //VLYC2PLUGIN_VIDEOIMPL_H
