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

#ifndef VLYC_SITE_PLUGIN_H
#define VLYC_SITE_PLUGIN_H

#include "plugin.h"
#include "video.h"

class SitePlugin : public virtual VlycBasePlugin
{
public:
    virtual ~SitePlugin() {}

    /**
     * @brief get an id for a URL, QString::null if this site cannot handle it.
     * @param url QString the URL
     * @return QString video_id | QString::null
     */
    virtual QString forUrl(QUrl url) = 0;

    /**
     * @brief get a Video instance for video_id
     * @param video_id
     * @return the Video instance
     */
    virtual VideoPtr video(QString video_id) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SitePlugin, "me.sodimm.oro.vlyc.SitePlugin/1.0")
QT_END_NAMESPACE

#endif // VLYC_SITE_PLUGIN_H
