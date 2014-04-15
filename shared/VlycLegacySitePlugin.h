#ifndef VLYC_VLYCLEGACYSITEPLUGIN_H
#define VLYC_VLYCLEGACYSITEPLUGIN_H

#include "VlycPlugin.h"
#include "../include/video.h"
#include "../include/siteplugin.h"

namespace Vlyc {

class LegacySitePlugin : public virtual Vlyc::Plugin, public virtual SitePlugin
{
public:
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

    // needed for legacy compat
    virtual int rev() const;
};

} // namespace Vlyc

#endif // VLYC_VLYCLEGACYSITEPLUGIN_H
