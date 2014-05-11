#!/usr/bin/env python3
# tanks to http://www.darkztar.com/forum/showthread.php?219034-Ripping-videos-amp-subtitles-from-Crunchyroll-(noob-friendly)
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

# Prerequisites:
# * pythonaes
# * A VLC compiled against a version of ffmpeg/libav compiled with librtmp support

# SETTINGS
video_format = "106"
resolution = "60"

import re
import io
import hashlib
import binascii
import math
import zlib
import base64

from Crypto.Cipher import AES

import vlyc.plugin

from html import entities

try:
    from lxml import html, etree as xml
except ImportError:
    from vlyc import html, xml

import urllib.request
import urllib.parse


def rtmp_escape(s):
    return s.replace("\\", "\\5c").replace(" ", "\\20")


class CrunchyrollPlugin(vlyc.plugin.SitePlugin):
    name = "Crunchyroll"
    author = "Orochimarufan"
    rev = 1

    regexp = re.compile(r"(?:http://)?(?:www\.)?crunchyroll.com/.*-([0-9]+)", re.DOTALL)

    def forUrl(self, url):
        m = self.regexp.match(str(url))
        if m:
            return m.group(1)

    def video(self, video_id):
        return Video(self, video_id)


class Video(vlyc.plugin.Video):
    playerrev_regexp = re.compile(br'flash\\/(.+)\\/StandardVideoPlayer.swf')

    def __init__(self, plugin, video_id):
        super(Video, self).__init__(video_id)
        self.plugin = plugin
        self.url = "http://www.crunchyroll.com/media-%s" % video_id
        self.flv = None

        self._htmlsource = None
        self._html = None
        self._playerrev = None

        self.sub_tracks = list()
        self.have_subs = dict()

    def playerRev(self):
        if self._playerrev is None:
            self._playerrev = self.playerrev_regexp.findall(self.htmlsource()).pop().decode("utf-8")
        return self._playerrev

    def _httprequest(self, url, headers={}, data=None):
        if isinstance(data, str):
            data = data.encode("ascii")
        request = urllib.request.Request(url, data)
        request.add_header("Referer", "http://crunchyroll.com/")
        request.add_header("Host", "www.crunchyroll.com")
        request.add_header("Content-Type", "application/x-www-form-urlencoded")
        request.add_header("User-Agent", "Mozilla/5.0 (Windows NT 6.1; rv:26.0) Gecko/20100101 Firefox/26.0)")
        for k, v in headers.items():
            request.add_header(k, v)
        return urllib.request.urlopen(request)

    def htmlsource(self):
        if self._htmlsource is None:
            print("[CR] Getting Page")
            self._htmlsource = self._httprequest(self.url).read()
            with open("CR.html", "wb") as f:
                f.write(self._htmlsource)
        return self._htmlsource

    def html(self):
        if self._html is None:
            self._html = html.parse(io.BytesIO(self.htmlsource()))
        return self._html

    def xml(self, req, script_id=None):
        url = 'http://www.crunchyroll.com/xml/'
        print("[CR] Getting XML: %s" % req)

        if req == 'RpcApiSubtitle_GetXml':
            data = {'req': 'RpcApiSubtitle_GetXml', 'subtitle_script_id': script_id}
        elif req == 'RpcApiVideoPlayer_GetStandardConfig':
            data = {'req': 'RpcApiVideoPlayer_GetStandardConfig', 'media_id': self.videoId,
                    'video_format': video_format, 'video_quality': resolution, 'auto_play': '1',
                    'show_pop_out_controls': '1', 'current_page': 'http://www.crunchyroll.com/'}
        else:
            data = {'req': req, 'media_id': self.videoId, 'video_format': video_format,
                    'video_encode_quality': resolution}

        res = self._httprequest(url, {
        "Referer": "http://static.ak.crunchyroll.com/flash/" + self.playerRev() + "/StandardVideoPlayer.swf"},
                                urllib.parse.urlencode(data))
        data = res.read()
        with open("CR-%s.xml" % req, "wb") as f:
            f.write(data)

        return xml.parse(io.BytesIO(data))

    # Regular expressions
    re_grr = re.compile(r"fplive\.net")
    re_host_grr = re.compile(r".+/c[0-9]+")
    re_app_grr = re.compile(r"c[0-9]+\?.+")
    re_host = re.compile(r".+/ondemand")
    re_app = re.compile(r"ondemand/.+")

    def load(self, done, throw):
        self.config = self.xml("RpcApiVideoPlayer_GetStandardConfig")
        code = self.config.find(".//code")
        if code is not None:
            code = int(code.text)
            if code == 4:
                return throw("Video not available in your country")

        # Title
        self.title = self.html().find(".//title").text.replace('Crunchyroll - Watch ', '')
        self.title = self.title.replace(":", "-")

        # Stream
        try:
            host = self.config.find(".//host").text
        except:
            self.gsi_config = self.xml("RpcApiVideoEncode_GetStreamInfo")
            host = self.gsi_config.find(".//host").text
        else:
            self.gsi_config = self.config

        # URL handling, from crunchy-xml-decoder
        #why host_grr? well, there was a time when fplive videos couldn't be downloaded, so...
        if self.re_grr.search(host):
            self.host = self.re_host_grr.findall(host).pop()
            self.app = self.re_app_grr.findall(host).pop()
        else:
            self.host = self.re_host.findall(host).pop()
            self.app = self.re_app.findall(host).pop()
        self.playpath = self.gsi_config.find('.//file').text

        self.availableQualities.append((vlyc.plugin.VideoQualityLevel.QA_LOWEST, "Default"))

        # Subtitles
        self.sub_config = self.xml("RpcApiSubtitle_GetListing")
        for sub in self.sub_config.iterfind(".//subtitle"):
            self.sub_tracks.append(sub)
            self.availableSubtitleLanguages.append(sub.get("title"))

        done()

    def getMedia(self, quality, media, throw):
        RTMP_OPTS = {
        "swfVfy": "1",
        "swfUrl": 'http://static.ak.crunchyroll.com/flash/%s/ChromelessPlayerApp.swf' % self.playerRev(),
        "app": self.app,
        "playpath": self.playpath,
        "pageUrl": self.url,
        "flashVer": "WIN 11,8,800,50"
        }
        rtmp_url = " ".join([self.host] + ["%s=%s" % (k, rtmp_escape(v)) for k, v in RTMP_OPTS.items()])
        print(rtmp_url)
        media(quality, "Default", rtmp_url)

    def getSubtitles(self, language, subtitles, throw):
        sub = self.sub_config.find(".//subtitle[@title='%s']" % language)
        if sub is None:
            throw("Unknown error!")
        else:
            try:
                subtitles(language, "ass", Subtitles(self.xml("RpcApiSubtitle_GetXml", sub.get("id"))).ass())
                #subtitles(language, "srt", Subtitles(self.xml("RpcApiSubtitle_GetXml", sub.get("id"))).srt())
            except:
                import traceback
                throw("".join(traceback.format_exc()))


def firstchild(el, tag):
    return next(el.iterchildren(tag))

def html_entity(entity):
    ent = entity[1:-1]
    if ent[0] == "#":
        # decoding by number
        if ent[1] != 'x':
            # number is in decimal
            return chr(int(ent[1:]))
        else:
            # number is in hex
            return chr(int('0x' + ent[2:], 16))
    else:
        # they were using a name
        if ent in entities:
            return entities[ent]
        else:
            return entity

def html_decode(text):
    def f(match):
        return html_entity(match.group(0))
    return re.sub("(\&.*\;)", f, text)

def clean_html(html):
    """Clean an HTML snippet into a readable string"""
    # Newline vs <br />
    html = html.replace('\n', ' ')
    html = re.sub(r'\s*<\s*br\s*/?\s*>\s*', '\n', html)
    html = re.sub(r'<\s*/\s*p\s*>\s*<\s*p[^>]*>', '\n', html)
    # Strip html tags
    html = re.sub('<.*?>', '', html)
    # Replace html entities
    html = html_decode(html)
    return html.strip()


class Subtitles:
    def __init__(self, xml):
        self.c_xml = xml

        subtitle = self.c_xml.getroot()

        self.script_id = int(subtitle.get("id"))
        self.c_iv = firstchild(subtitle, "iv").text
        self.c_data = firstchild(subtitle, "data").text

        self.xmlsource = self.decrypt(self.script_id, self.c_iv, self.c_data)

        with open("CR-subs.xml", "wb") as f:
            f.write(self.xmlsource)

        self.xml = xml.parse(io.BytesIO(self.xmlsource))
        # Should return ElementTree but returns Element?
        print(type(self.xml))

    # ========== ASS ==========
    def iterass(self):
        # Header
        script = self.xml  #.getroot()
        yield "[Script Info]"
        yield "Title: %s" % script.get("title")
        yield "ScriptType: v4.00+"
        yield "WrapStyle: %s" % script.get("wrap_style")
        yield "PlayResX: %s" % script.get("play_res_x")
        yield "PlayResY: %s" % script.get("play_res_y")

        yield ""

        # Styles
        styles = firstchild(script, "styles")
        yield "[V4+ Styles]"
        yield "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding"
        format = "Style: {name},{font_name},{font_size},{primary_colour},{secondary_colour},{outline_colour},{back_colour},{bold},{italic},{underline},{strikeout},{scale_x},{scale_y},{spacing},{angle},{border_style},{outline},{shadow},{alignment},{margin_l},{margin_r},{margin_v},{encoding}"
        for style in styles.iterchildren("style"):
            yield format.format(**style.attrib)

        yield ""

        # Events
        events = firstchild(script, "events")
        yield "[Events]"
        yield "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text"
        format = "Dialogue: 0,{start},{end},{style},{name},{margin_l},{margin_r},{margin_v},{effect},{text}"
        for event in events.iterchildren("event"):
            yield format.format(**event.attrib)

        yield ""

    def ass(self):
        data = "\n".join(self.iterass())#.encode("utf-8-sig")  # ASS needs the BOM?
        with open("CR-subs.ass", "w") as f:
            f.write(data)
        return data

    # ========== Crypto ==========
    # Salt
    # @staticmethod
    # def createSalt(iters, mod, *args):
    # 	string = list(args)
    # 	for i in range(iters):
    # 		string.append(string[-1] + string[-2])
    # 	return bytes([c % mod + 33 for c in string[2:]])
    # key_salt = createSalt(20, 97, 1, 2)
    # sid_salt = int(math.floor(math.sqrt(6.9) * math.pow(2, 25)))
    key_salt = b'$&).6CXzPHw=2N_+isZK'
    sid_salt = 88140282

    @classmethod
    def generateKey(cls, script_id, size=32):
        # Create key (Black Magic)
        eq3 = (script_id ^ cls.sid_salt) ^ (script_id ^ cls.sid_salt) >> 3 ^ int(cls.sid_salt ^ script_id) * 32
        key = cls.key_salt + str(eq3).encode("ascii")
        # Hash it (SHA1-160)
        digest = hashlib.sha1(key).digest()
        # Pad if necessary
        if size > len(digest):
            digest += b'\0' * (size - len(digest))
        return digest[:size]

    @classmethod
    def decrypt(self, script_id, iv, data, compressed=True):
        key = self.generateKey(script_id)
        iv = base64.b64decode(iv)
        data = base64.b64decode(data)

        cipher = AES.new(key=key, mode=AES.MODE_CBC, IV=iv)
        data = cipher.decrypt(data)

        if compressed:
            return zlib.decompress(data)
        else:
            return data

    def itersrt(self):
        for i, event in enumerate(self.xml.xpath("//event")):
            start = event.get("start").replace(".", ",")
            end = event.get("end").replace(".", ",")
            text = clean_html(event.get("text", "")).replace("\\N", '\n')
            if text:
                yield "%d\n%s --> %s\n%s\n\n" % (i, start, end, text)

    def srt(self):
        return "".join(self.itersrt()).encode("utf-8")
