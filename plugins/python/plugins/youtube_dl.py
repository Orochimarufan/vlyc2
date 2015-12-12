# (c) 2014 Taeyeon Mori
# Public Domain.

import os
import sys
import urllib.parse
import urllib.request
import http.cookiejar

sys.path.append(os.path.expanduser("~/Development/youtube-dl"))

from youtube_dl.extractor import gen_extractors, get_info_extractor
from youtube_dl.utils import ExtractorError
from youtube_dl.cache import Cache

import vlyc.plugin
import vlyc.network

import youtube_dl
youtube_dl.plugin = sys.modules[__name__]

last_exc = None
instance = None

# We don't want playlists (yet)
import re
from youtube_dl.extractor import youtube
youtube.YoutubeIE._VALID_URL =     _VALID_URL = r"""(?x)^
                     (
                         (?:https?://|//)                                    # http(s):// or protocol-independent URL
                         (?:(?:(?:(?:\w+\.)?[yY][oO][uU][tT][uU][bB][eE](?:-nocookie)?\.com/|
                            (?:www\.)?deturl\.com/www\.youtube\.com/|
                            (?:www\.)?pwnyoutube\.com/|
                            (?:www\.)?yourepeat\.com/|
                            tube\.majestyc\.net/|
                            youtube\.googleapis\.com/)                        # the various hostnames, with wildcard subdomains
                         (?:.*?\#/)?                                          # handle anchor (#/) redirect urls
                         (?:                                                  # the various things that can precede the ID:
                             (?:(?:v|embed|e)/(?!videoseries))                # v/ or embed/ or e/
                             |(?:                                             # or the v= param in all its forms
                                 (?:(?:watch|movie)(?:_popup)?(?:\.php)?/?)?  # preceding watch(_popup|.php) or nothing (like /?v=xxxx)
                                 (?:\?|\#!?)                                  # the params delimiter ? or # or #!
                                 (?:.*?&)?                                    # any other preceding param (like /?s=tuff&v=xxxx)
                                 v=
                             )
                         ))
                         |youtu\.be/                                          # just youtu.be/xxxx
                         |(?:www\.)?cleanvideosearch\.com/media/action/yt/watch\?videoId=
                         )
                     )?                                                       # all until now is optional -> you can pass the naked ID
                     ([0-9A-Za-z_-]{11})                                      # here is it! the YouTube video ID
                     #(?!.*?&list=)                                            # combined list/video URLs are handled by the playlist IE
                     (?(1).+)?                                                # if we found the ID, everything can follow
                     $"""

# Blacklist Extractors
key_whitelist = ["YoutubePlaylist"]
key_blacklist = ["Generic", "Crunchyroll"]

def default_extractors():
    for ie in gen_extractors():
        key = ie.ie_key()
        if key in key_whitelist or (key not in key_blacklist and
                not(key.endswith("Channel") or key.endswith("Playlist") or key.endswith("User"))):
            yield ie


class YoutubeDLPlugin(vlyc.plugin.SitePlugin):
    name = "Youtube-DL"
    author = "Orochimarufan"
    rev = 12

    def __init__(self):
        self._ies = dict()
        self.add_default_info_extractors()
        # DASH is broken in Vlc HEAD
        self.params = {"writesubtitles": True, "writeautomaticsub": True, "verbose": True, "youtube_include_dash_manifest": False}
        self._current = None
        self.cookiejar = http.cookiejar.CookieJar() # stub
        self.cache = Cache(self)
        
        global instance
        instance = self

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
        for ie in default_extractors():
            self.add_info_extractor(ie)

    # Output
    def to_screen(self, text, skip_eol=False):
        print(text)

    def to_stdout(self, text, skip_eol=False, respect_quiet=False):
        print(text)

    def to_stderr(self, text):
        print(text)

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
            try:
                self._load(done, throw)
            except:
                import traceback
                traceback.print_exc()
                print()
                raise

        def _load(self, done, throw):
            #throw = self._plugin.begin(self, throw)
            if not throw: return

            # Ask YoutubeDL
            try:
                result = self._ie.extract(self._url)
            #except ExtractorError as e:
            #    return throw(str(e))
            except Exception:
                global last_exc
                last_exc = sys.exc_info()
                import traceback
                return throw(traceback.format_exc())

            if result is None:
                return throw("Video not found.")

            if isinstance(result, list):
                result = {
                        "_type": "playlist",
                        "title": "Legacy Youtube-DL Playlist",
                        "entries": result,
                        }

            result.setdefault("extractor", self._ie.IE_NAME)
            result.setdefault("extractor_key", self._ie.ie_key())
            result.setdefault("webpage_url", self._url)
#            result.setdefault("webpage_url_basename", )

            self._data = result

            type = result.get("_type", "video")
            if type == "playlist": # Shoehorned! (TM)
                self.title = result["title"]
                self.childrenUrls = list(map(self._plugin.figure_out_url_result, result["entries"]))
                return done()

            if type != "video":
                return throw("Cannot handle '%s' result yet. Only video results supported" % result["_type"])

            # Populate attributes
            self.title = result["title"]
            self.author = result.get("uploader", "Unknown Uploader")
            self.description = result.get("description", "")
            self.views = result.get("view_count", 0)
            self.likes = result.get("like_count", 0)
            self.dislikes = result.get("dislike_count", 0)
            
            #import pprint
            #pprint.pprint(result)

            # Do quality stuff
            if result["extractor_key"] == "Youtube":
                self._formats, self.availableQualities = YoutubeFormatDatabase.process(result["formats"])
            elif "formats" in result:
                self._formats, self.availableQualities = GenericFormatMapper.process(result["formats"])
            elif "url" in result:
                lx = vlyc.plugin.VideoQualityLevel.QA_LOWEST
                self._formats = {lx: (lx, "Default", result["url"])}
                self.availableQualities = [(lx, "Default")]
            else:
                import pprint
                pprint.pprint(result)
                return throw("could not figure out video url")

            # Subtitles
            self._subs = result.get("subtitles", {})
            self.availableSubtitleLanguages = list(self._subs.keys())

            #self._plugin.end(self)
            return done()

        def getMedia(self, quality, media, throw):
            if quality not in self._formats:
                throw("Quality not available!")
            else:
                media(*self._formats[quality])

        def getSubtitles(self, language, subtitles, throw):
            try:
                subtitles(language, "srt", self._subs[language])
            except Exception as e:
                throw(str(e))

    def figure_out_url_result(self, url):
        if isinstance(url, str):
            return url
        if "webpage_url" in url:
            return url["webpage_url"]
        if "url" in url:
            if url["url"].startswith("http"):
                return url["url"]
            elif "ie_key" in url:
                ie = url["ie_key"]
                if ie == "Youtube":
                    return "http://youtube.com/watch?v=" + url["url"]
            return url["url"]
        raise ValueError("Cannot convert %s to URL" % url)
        

class GenericFormatMapper:
    map = (
        (360, vlyc.plugin.VideoQualityLevel.QA_360),
        (480, vlyc.plugin.VideoQualityLevel.QA_480),
        (720, vlyc.plugin.VideoQualityLevel.QA_LOWEST + 15),
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
    def process(c, f):
        try:
            return c._process(f)
        except:
            import traceback
            traceback.print_exc()
            raise

    @classmethod
    def _process(cls, formats):
        """
        Just try to map the youtube-dl formats to vlyc2 ones as cleanly as possible
        """
        import pprint
        pprint.pprint(formats)
        # Filter and sort formats
        formats = sorted(
            cls.this_or_that(
                filter(
                    lambda f: ("vcodec" not in f or f["vcodec"] != "none") and ("acodec" not in f or f["acodec"] != "none"),
                    formats
                ),
                formats
            ),
            key=lambda f: (f["height"] if "height" in f and f["height"] is not None else 0, f["preference"] if "preference" in f and f["preference"] is not None else 0)
        )
        # Map formats
        avail = list()
        lookup = dict()
        level = vlyc.plugin.VideoQualityLevel.QA_LOWEST
        levels = list(cls.map)
        for format in formats:
            while "height" in format and format["height"] is not None and format["height"] > levels[0][0]:
                level = levels.pop(0)[1]
            if "format_note" in format:
                name = format["format_note"]
            elif "format" in format:
                name = format["format"]
            elif "height" in format:
                name = "[%ip]" % format["height"]
            else:
                name = "Unknown format"
            avail.append((level, name))
            lookup[level] = (level, name, format["url"])
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
            if (format["format_id"] == "dash"):
                avail.append(cls.dash)
                lookup[cls.dash[0]] = (cls.dash[0], cls.dash[1], format["url"])
            else:
                itag = int(format["format_id"].strip("nondash-"))
                if itag in cls.itags:
                    stream = cls.itags[itag]
                    if stream[0] != vlyc.plugin.VideoQualityLevel.QA_INVALID:
                        avail.append(stream)
                        lookup[stream[0]] = (stream[0], stream[1], format["url"])
        cls.dash_formats(formats, avail, lookup)
        return lookup, avail

    dash =  (vlyc.plugin.VideoQualityLevel.QA_HIGHEST * 2, "Auto (DASH)")

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
        22: (vlyc.plugin.VideoQualityLevel.QA_720 + 10, "720p MPEG4/AVC"),
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

    @staticmethod
    def dash_formats(formats, avail, lookup):
        return
        # HAAAAX!
        format_map = {a["format_id"]:a for a in formats}
        def add_merged_level(level, desc, main_format_id, *format_ids):
            avail.append((level, desc))
            mrl = format_map[main_format_id]["url"]
            if format_ids:
                mrl += " :input-slave=" + "#".join(format_map[id]["url"] for id in format_ids)
            lookup[level] = (level, desc, mrl)
        def try_combine(level, desc, video_format_id, *audio_format_ids):
            if video_format_id not in format_map:
                return
            for audio_format in audio_format_ids:
                if audio_format in format_map:
                    add_merged_level(level, desc, video_format_id, audio_format)
                    break
        # GO!
        try_combine(vlyc.plugin.VideoQualityLevel.QA_480 + 3, "480p DASH/MP4", "135", "141", "140", "139")
        try_combine(vlyc.plugin.VideoQualityLevel.QA_480 + 2, "480p DASH/VP8", "244", "172", "171", "139")
        try_combine(vlyc.plugin.VideoQualityLevel.QA_1080 + 3, "1080p DASH/MP4", "137", "141", "140", "139")
        try_combine(vlyc.plugin.VideoQualityLevel.QA_1080 + 4, "1440p DASH/MP4", "264", "141", "140", "139")
        try_combine(vlyc.plugin.VideoQualityLevel.QA_HIGHEST + 3, "2160p DASH/MP4", "138", "141", "140", "139")

    itags_reverse = { qa: (itag, desc) for itag, (qa, desc) in itags.items() }

