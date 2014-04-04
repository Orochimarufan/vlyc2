# (c) 2014 Taeyeon Mori
# Public Domain.

import os
import sys
import urllib.parse
import urllib.request

sys.path.append(os.path.expanduser("~/devel/youtube-dl"))

from youtube_dl.extractor import gen_extractors, get_info_extractor
from youtube_dl.utils import ExtractorError

import vlyc.plugin
import vlyc.network


class YoutubeDLPlugin(vlyc.plugin.SitePlugin):
    name = "Youtube-DL"
    author = "Orochimarufan"
    rev = 1

    def __init__(self):
        self._ies = dict()
        self.add_default_info_extractors()
        self.params = {}
        self._current = None

#### YoutubeDL interface
    # IE Management
    def add_info_extractor(self, ie):
        #print("Adding IE %s" % ie.ie_key())
        self._ies[ie.ie_key()] = ie
        ie.set_downloader(self)

    def get_info_extractor(self, ie_key):
        ie = self._ies.get(ie_key)
        if ie is None:
            ie = get_info_extractor(ie_key)()
            self.add_info_extractor(ie)
        return ie

    def add_default_info_extractors(self):
        for ie in gen_extractors():
            if ie.ie_key() == "Generic":
                continue
            self.add_info_extractor(ie)

    # Output
    def to_screen(self, text, skip_eol=False):
        print(text, end="")

    def to_stdout(self, text, skip_eol=False, respect_quiet=False):
        print(text, end="")

    def to_stderr(self, text):
        print(text, end="")

    def to_console_title(self, title):
        pass

    # Error handling
    def trouble(self, message=None, tb=None):
        raise Exception(message)

    def report_warning(self, message):
        self.to_stderr("WARNING: %s" % message)

    def report_error(self, message):
        self.to_stderr("ERROR: %s" % message)
        if self._current:
            self._current[1](message)

    # Urllib
    def urlopen(self, url):
        #return urllib.request.urlopen(url)
        return vlyc.network.urlopen(url)

#### Vlyc2 interface
    def forUrl(self, url):
        url = str(url)
        for ie in self._ies.values():
            if ie.suitable(url):
                return "%s#%s" % (ie.ie_key(), urllib.parse.quote(url))

    def video(self, id):
        return self.Video(self, id)

    def begin(self, video, throw):
        if self._current is not None:
            return throw("Please wait for your previous request to finish.")
        else:
            self._current = (video, throw)
            def new_throw(msg):
                self._current = None
                return throw(msg)
            return new_throw

    def end(self, video):
        if self._current is not None:
            video_, throw = self._current
            if video != video_:
                throw("FATAL: end() called by different video!")
            else:
                self._current = None

    class Video(vlyc.plugin.Video):
        def __init__(self, plugin, xurl):
            super().__init__(xurl)

            ie_key, url = xurl.split("#", 1)

            self._plugin = plugin
            self._ie = plugin.get_info_extractor(ie_key)
            self._url = urllib.parse.unquote(url)

        def load(self, done, throw):
            #throw = self._plugin.begin(self, throw)
            if not throw: return

            # Ask YoutubeDL
            try:
                result = self._ie.extract(self._url)
            except ExtractorError as e:
                return throw(str(e))
            except Exception:
                import traceback
                return throw(traceback.format_exc())

            if result is None:
                return throw("Video not found.")

            if isinstance(result, list):
                result = {
                        "_type": "compat_list",
                        "entries": result,
                        }

            result.setdefault("extractor", self._ie.IE_NAME)
            result.setdefault("extractor_key", self._ie.ie_key())
            result.setdefault("webpage_url", self._url)
#            result.setdefault("webpage_url_basename", )

            self._data = result

            if result.get("_type", "video") != "video":
                return throw("Cannot handle '%s' result yet. Only video results supported" % result["_type"])

            # Populate attributes
            self.title = result["title"]
            self.author = result.get("uploader", "Unknown Uploader")
            self.description = result.get("description", "")
            self.views = result.get("view_count", 0)
            self.likes = result.get("like_count", 0)
            self.dislikes = result.get("dislike_count", 0)

            # Do quality stuff
            if result["extractor_key"] == "Youtube":
                self._formats, self.availableQualities = YoutubeFormatDatabase.process(result["formats"])
            else:
                self._formats, self.availableQualities = GenericFormatMapper.process(result["formats"])

            #self._plugin.end(self)
            done()

        def getMedia(self, quality, media, throw):
            if quality not in self._formats:
                throw("Quality not available!")
            else:
                media(*self._formats[quality])

        def getSubtitles(self, language, subtitles, throw):
            throw("Not implemented")


class GenericFormatMapper:
    map = (
        (360, vlyc.plugin.VideoQualityLevel.QA_360),
        (480, vlyc.plugin.VideoQualityLevel.QA_480),
        (720, vlyc.plugin.VideoQualityLevel.QA_720),
        (1080, vlyc.plugin.VideoQualityLevel.QA_1080),
        (2000, vlyc.plugin.VideoQualityLevel.QA_HIGHEST),
    )

    @staticmethod
    def this_or_that(iter_a, iter_b):
        try:
            a = next(iter_a)
        except StopIteration:
            yield from iter_b
        else:
            yield a
            yield from iter_a

    @classmethod
    def process(cls, formats):
        """
        Just try to map the youtube-dl formats to vlyc2 ones as cleanly as possible
        """
        # Filter and sort formats
        formats = sorted(
            cls.this_or_that(
                filter(
                    formats,
                    lambda f: f["vcodec"] != "none" and f["acodec"] != "none"
                ),
                formats
            ),
            key=lambda f: (f["height"], f["preference"])
        )
        # Map formats
        avail = list()
        lookup = dict()
        level = vlyc.plugin.VideoQualityLevel.QA_LOWEST
        levels = list(cls.map)
        for format in formats:
            while format["height"] > levels[0][0]:
                level = levels.pop(0)[1]
            avail.append((level, format["format_note"]))
            lookup[level] = (level, format["format_note"], format["url"])
            level += 1
        return lookup, avail


class YoutubeFormatDatabase:
    @classmethod
    def process(cls, formats):
        """
        We know more about youtube formats
        """
        avail = list()
        lookup = dict()
        for format in formats:
            itag = int(format["format_id"])
            if itag in cls.itags:
                stream = cls.itags[itag]
                if stream[0] != vlyc.plugin.VideoQualityLevel.QA_INVALID:
                    avail.append(stream)
                    lookup[stream[0]] = (stream[0], stream[1], format["url"])
        return lookup, avail

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

