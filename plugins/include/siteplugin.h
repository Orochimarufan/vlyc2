#ifndef VLYC_SITE_PLUGIN_H
#define VLYC_SITE_PLUGIN_H

#include "plugin.h"
#include "video.h"

class SitePlugin : public VlycBasePlugin
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
    virtual Video* video(QString video_id) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SitePlugin, "me.sodimm.oro.vlyc.SitePlugin/1.0")
QT_END_NAMESPACE

#endif // VLYC_SITE_PLUGIN_H
