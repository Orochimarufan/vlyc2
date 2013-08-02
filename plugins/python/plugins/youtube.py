#----------------------------------------------------------------------
#- vlyc2 YouTube plugin
#----------------------------------------------------------------------
#- Copyright (C) 2011-2013 Orochimarufan
#-                Authors: Orochimarufan <orochimarufan.x3@gmail.com>
#-
#- This program is free software: you can redistribute it and/or modify
#- it under the terms of the GNU General Public License as published by
#- the Free Software Foundation, either version 3 of the License, or
#- (at your option) any later version.
#-
#- This program is distributed in the hope that it will be useful,
#- but WITHOUT ANY WARRANTY; without even the implied warranty of
#- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#- GNU General Public License for more details.
#-
#- You should have received a copy of the GNU General Public License
#- along with this program.  If not, see <http://www.gnu.org/licenses/>.
#----------------------------------------------------------------------

import re
import json
import io

import PythonQt

import vlyc.plugin
import vlyc.util

try:
    from lxml import html
except ImportError:
    from vlyc import html


class YoutubePlugin(vlyc.plugin.SitePlugin):
    name="YouTube"
    author="Orochimarufan"
    rev=1

    regexp = re.compile(r"^(?:https?\:\/\/)?(?:www\.|m\.)?youtu(?:be\..{2,3}\/watch\?.*v\=.+(?:\&.+)*|\.be\/.+)$")
    yt_reg = re.compile(r"^(?:https?\:\/\/)?(?:www\.|m\.)?youtube\..{2,3}\/watch\?(.*)$")
    be_reg = re.compile(r"^(?:https?\:\/\/)?(?:www\.)?youtu.be\/([^\/?#]*)$")

    @staticmethod
    def element_text(elem, id, mod=None):
        xt = elem.get_element_by_id(id)
        if xt is not None:
            if mod:
                xt = mod(xt)
            t = xt.text
            if t is not None:
                return t.strip()

    def forUrl(self, url):
        m = self.yt_reg.match(url)
        if m:
            return vlyc.util.sdict_parser(m.group(1))["v"]
        m = self.be_reg.match(url)
        if m:
            return m.group(1)

    def loadVideo(self, video):
        # TODO: PythonQt QtNetwork fix
        #req = PythonQt.QtNetwork.QNetworkRequest(PythonQt.QtCore.QUrl("http://youtube.com/watch?v=%s&gl=US&hl=en&has_verified=1" % video.videoId()))
        #req.setRawHeader(b"Referer", b"http://www.youtube.com/")
        #reply = vlyc.network.get(req)
        #reply.connect(b"finished()", lambda: self.continueLoadVideo(video, reply))
        #reply.connect(b"error(QNetworkReply::NetworkError)", lambda: self.errorLoadVideo(video, reply))

    #def errorLoadVideo(self, video, reply):
    #    reply.deleteLater()
    #    video.setError(reply.errorString())

    #def continueLoadVideo(self, video, reply):
        #document = html.parse(reply)
        #reply.deleteLater()
        # QtNetwork seems to be a bit broken right now, use lxml/urllib for now.
        document = html.parse("http://youtube.com/watch?v=%s&gl=US&hl=en&has_verified=1" % video.videoId())
        page = document.getroot()

        # ------ Info ------
        video.setTitle(self.element_text(page, "eow-title"))
        video.setAuthor(self.element_text(page, "watch7-user-header", lambda it: it[1]))
        video.setDescription(self.element_text(page, "eow-description"))
        views = page.get_element_by_id("watch7-views-info")
        try:
            video.setViews(int(views[0].text.strip().replace(",", "")))
        except:
            video.setViews(0)
        try:
            video.setLikes(int(views.find_class("likes-count")[0].text.strip().replace(",", "")))
        except:
            video.setLikes(0)
        try:
            video.setDislikes(int(views.find_class("dislikes-count")[0].text.strip().replace(",", "")))
        except:
            video.setDislikes(0)

        # ------ Video URLs ------
        div = page.get_element_by_id("player")

        # check for error message
        try:
            youtube_error = div.get_element_by_id("unavailable-message")
        except:
            youtube_error = None

        def error(message):
            if youtube_error is not None:
                video.setError("%s\n\n%s" % (youtube_error.text.strip(), message))
            else:
                video.setError(message)

        # get player config
        for script in div.iterfind(".//script"):
            if script.text is not None and "ytplayer.config = {" in script.text:
                config = script.text[script.text.index("ytplayer.config = {") + 18:script.text.rindex("}") + 1]
                break
        else:
            return error("Could not find ytplayer.config.")

        try:
            data = json.loads(config, strict=False)
        except:
            return error("Could not parse ytplayer.config.")

        # get url map
        args = data["args"]
        urlmap = {
                int(i["itag"]): "&signature=".join((i["url"], i["sig"]))
                    for i in (vlyc.util.sdict_parser(i, unq=2)
                        for i in args["url_encoded_fmt_stream_map"].split(","))
                }

        # add qualities
        for itag, url in urlmap.items():
            if itag not in self.itags:
                PythonQt.Qt.qWarning(b"Unknown ITag: %i" % itag)
            else:
                q, desc = self.itags[itag]
                if q != vlyc.plugin.VideoQualityLevel.QA_INVALID:
                    video.addQuality(q, desc, url)

        video.setDone()

    # YouTube formats
    itags = {
        #Mobile/3GP   [MPEG-4-Visual|AAC]
        13: (vlyc.plugin.VideoQualityLevel.QA_LOWEST, "Mobile/3GP"),
        17: (vlyc.plugin.VideoQualityLevel.QA_LOWEST + 1, "176x144 Mobile/3GP"),
        36: (vlyc.plugin.VideoQualityLevel.QA_LOWEST + 2, "240p Mobile/3GP"),
        #Flash/FLV    [Sorenson Spark|MP3]
        5: (vlyc.plugin.VideoQualityLevel.QA_LOWEST + 3, "240p Flash/FLV"),
        6: (vlyc.plugin.VideoQualityLevel.QA_LOWEST + 4, "270p Flash/FLV"),
        #Flash/FLV    [Sorenson Spark|AAC]
        34: (vlyc.plugin.VideoQualityLevel.QA_360, "360p Flash/FLV"),
        35: (vlyc.plugin.VideoQualityLevel.QA_480, "480p Flash/FLV"),
        #WebM/VP8     [VP8|Vorbis]
        43: (vlyc.plugin.VideoQualityLevel.QA_360 + 1, "360p WebM/VP8"),
        44: (vlyc.plugin.VideoQualityLevel.QA_480 + 1, "480p WebM/VP8"),
        45: (vlyc.plugin.VideoQualityLevel.QA_720, "720p WebM/VP8"),
        46: (vlyc.plugin.VideoQualityLevel.QA_1080, "1080p WebM/VP8"),
        #MPEG4/AVC    [H.264|AAC]
        18: (vlyc.plugin.VideoQualityLevel.QA_360 + 2, "360p MPEG4/AVC"),
        22: (vlyc.plugin.VideoQualityLevel.QA_720 + 1, "720p MPEG4/AVC"),
        37: (vlyc.plugin.VideoQualityLevel.QA_1080 + 1, "1080p MPEG4/AVC"),
        38: (vlyc.plugin.VideoQualityLevel.QA_HIGHEST, "3072p MPEG4/AVC (Original)"),
        #WebM/VP8 3D  [VP8|Vorbis]
        # no 3D support (yet)
        100: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None),#(vlyc.plugin.VideoQualityLevel.QA_360 + 3, "360p WebM/VP8 3D"),
        101: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None),#(vlyc.plugin.VideoQualityLevel.QA_480 + 2, "480p WebM/VP8 3D"),
        102: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None),#(vlyc.plugin.VideoQualityLevel.QA_720 + 2, "720p WebM/VP8 3D"),
        #MPEG4/AVC 3D [H.264|AAC]
        83: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None), #(vlyc.plugin.VideoQualityLevel.QA_LOWEST + 5, "240p MPEG4/AVC 3D"),
        82: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None), #(vlyc.plugin.VideoQualityLevel.QA_360 + 4, "360p MPEG4/AVC 3D"),
        85: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None), #(vlyc.plugin.VideoQualityLevel.QA_480 + 3, "520p MPEG4/AVC 3D"),
        84: (vlyc.plugin.VideoQualityLevel.QA_INVALID, None), #(vlyc.plugin.VideoQualityLevel.QA_720 + 3, "720p MPEG4/AVC 3D"),
        # Stream
        120: (vlyc.plugin.VideoQualityLevel.QA_HIGHEST + 1, "720p FLV Stream")
    }