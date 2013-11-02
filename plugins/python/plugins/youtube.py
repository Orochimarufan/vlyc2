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
import math

import PythonQt

import vlyc.plugin
import vlyc.util

try:
    from lxml import html, etree as xml
except ImportError:
    from vlyc import html, xml

from html import entities


def element_text(elem, id, mod=None):
            xt = elem.get_element_by_id(id)
            if xt is not None:
                if mod:
                    xt = mod(xt)
                t = xt.text
                if t is not None:
                    return t.strip()


def html_reflacer(match):
    if match.group(1):
        return chr(int(match.group(2)))
    else:
        return chr(entities.name2codepoint[match.group(3)])
html_refex = re.compile(r'&(?:(#)(\d+)|([^;]+));')
html_decode = lambda s: html_refex.sub(html_reflacer, s)


class YoutubePlugin(vlyc.plugin.SitePlugin):
    name="YouTube"
    author="Orochimarufan"
    rev=1

    regexp = re.compile(r"^(?:https?\:\/\/)?(?:www\.|m\.)?youtu(?:be\..{2,3}\/watch\?.*v\=.+(?:\&.+)*|\.be\/.+)$")
    yt_reg = re.compile(r"^(?:https?\:\/\/)?(?:www\.|m\.)?youtube\..{2,3}\/watch\?(.*)$")
    be_reg = re.compile(r"^(?:https?\:\/\/)?(?:www\.)?youtu.be\/([^\/?#]*)$")

    def forUrl(self, url):
        url = str(url)
        m = self.yt_reg.match(url)
        if m:
            return vlyc.util.sdict_parser(m.group(1))["v"]
        m = self.be_reg.match(url)
        if m:
            return m.group(1)

    def video(self, video_id):
        return self.Video(video_id);

    class Video(vlyc.plugin.Video):
        def __init__(self, video_id):
            super(YoutubePlugin.Video, self).__init__(video_id)
            self.urlmap = dict()
            self.sub_tracks = list()
            self.sub_cache = dict()

        def load(self, done, throw):
            url = "http://youtube.com/watch?v=%s&gl=US&hl=en&has_verified=1" % self.videoId
            data = vlyc.network.retrieve(url)
            document = html.parse(io.BytesIO(data))
            page = document.getroot()

            # ------ Info ------
            self.title = element_text(page, "eow-title")
            self.author = element_text(page, "watch7-user-header", lambda it: it[1])
            self.description = element_text(page, "eow-description")
            views = page.get_element_by_id("watch7-views-info")
            try:
                self.views = int(views[0].text.strip().replace(",", ""))
            except:
                self.views = 0
            try:
                self.likes = int(views.find_class("likes-count")[0].text.strip().replace(",", ""))
            except:
                self.likes = 0
            try:
                self.dislikes = int(views.find_class("dislikes-count")[0].text.strip().replace(",", ""))
            except:
                self.dislikes = 0

            # ------ Video URLs ------
            div = page.get_element_by_id("page")

            # check for error message
            try:
                youtube_error = div.get_element_by_id("unavailable-message")
            except:
                youtube_error = None

            def error(message):
                if youtube_error is not None:
                    throw("%s\n\n%s" % (youtube_error.text.strip(), message))
                else:
                    throw(message)

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
            self.urlmap = {
                    int(i["itag"]): "&signature=".join((i["url"], i["sig"]))
                        for i in (vlyc.util.sdict_parser(i, unq=2)
                            for i in args["url_encoded_fmt_stream_map"].split(","))
                    }

            # add qualities
            for itag in self.urlmap.keys():
                if itag not in self.itags:
                    PythonQt.Qt.qWarning(b"Unknown ITag: %i" % itag)
                else:
                    q, desc = self.itags[itag]
                    if q != vlyc.plugin.VideoQualityLevel.QA_INVALID:
                        self.availableQualities.append((q, desc))

            # ------ subtitles ------
            # same problem as above. use urllib for now.
            document = xml.parse(io.BytesIO(vlyc.network.retrieve("http://video.google.com/timedtext?type=list&v=%s" % self.videoId)))
            transcript_list = document.getroot()
            self.sub_tracks = list()
            for track in transcript_list:
                track_id = int(track.get("id"))
                track_name = track.get("name")
                track_ln = track.get("lang_code")
                track_lo = track.get("lang_original")
                track_lt = track.get("lang_translated")
                track_default = bool(track.get("lang_default"))
                self.sub_tracks.append((track_id, track_name, track_ln, track_lo, track_lt, track_default))
                self.availableSubtitleLanguages.append(track_lo)

            done()

        @staticmethod
        def _srt_entry(n, start, duration, text):
            nms, nsec = math.modf(start)
            nmin = math.floor(nsec / 60)
            nsec %= 60
            nhr = math.floor(nmin / 60)
            nmin %= 60
            ems, esec = math.modf(start + duration)
            emin = math.floor(esec / 60)
            esec %= 60
            ehr = math.floor(emin / 60)
            emin %= 60
            caption = html_decode(text)
            return r"""{n}
            {nhr:02.0f}:{nmin:02.0f}:{nsec:02.0f},{nms:03.0f} --> {ehr:02.0f}:{emin:02.0f}:{esec:02.0f},{ems:03.0f}
            {caption}

            """.format(**locals())

        def getSubtitles(self, language, subtitles, throw):
            if language in self.sub_cache:
                return subtitles(language, "SRT", self.sub_cache[language])
            x = [i for i in self.sub_tracks if i[3] == language]
            if not x:
                throw("no subtitles found for language %s" % language)
            id, name, lang_code, lang_original, lang_translated, isDefault = x[0]
            url = "http://video.google.com/timedtext?type=track&v=%s&name=%s&lang=%s" % (self.videoId, name, lang_code)
            document = xml.parse(url)
            i = 0
            srt = list()
            for caption in document.getroot():
                i += 1
                srt.append(self._srt_entry(i, float(caption.get("start")), float(caption.get("dur")), caption.text))
            srt = "".join(srt)
            self.sub_cache[language] = srt
            subtitles(language, "SRT", srt)


        def getMedia(self, quality, media, throw):
            if quality not in self.itags_reverse:
                throw("Unknown Quality %i" % quality)
            itag, desc = self.itags_reverse[quality]
            if itag not in self.urlmap:
                throw("Quality '%s' not available for this video." % desc)
            media(quality, desc, self.urlmap[itag])

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

        itags_reverse = { qa: (itag, desc) for itag, (qa, desc) in itags.items() }
