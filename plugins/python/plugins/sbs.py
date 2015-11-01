#!/usr/bin/python3

# SBS Live stream.

import vlyc.plugin
import io
import random
import base64
import Crypto.Cipher.DES
try:
	import lxml.etree as etree
except ImportError:
	import vlyc.xml as etree


def rtmp_escape(s):
	return s.replace("\\", "\\5c").replace(" ", "\\20")



# Stuff
class SBSOnAirPlugin(vlyc.plugin.SitePlugin):
	name="SBS OnAir"
	author="Orochimarufan"
	rev=2

	def forUrl(self, url):
		if url.scheme() == "sbs":
			return str(url)[6:]

	def video(self, video_id):
		return Video(self, video_id)


class Video(vlyc.plugin.Video):
		# SBS NeTVOnAir
		version = "1.5.0"
		
		def __init__(self, plugin, video_id):
			super(Video, self).__init__(video_id.upper())
			self.plugin = plugin
		
		def log(self, msg, *a):
			print("SBS [%s] %s" % (self.videoId, msg % a))

		# Auth Key
		# Class.Crypt.decrypt(String) : String
		@staticmethod
		def decrypt(data):
			# Static Key
			key = b"7d1ff4ea8925c225"

			# Ciphertext is base64-encoded
			ciphertext = base64.b64decode(data)

			# The Cipher used is DES-ECB. The key just gets truncated.
			cipher = Crypto.Cipher.DES.new(key[:8], mode=Crypto.Cipher.DES.MODE_ECB)

			# It's padded with PKCS5
			unpad = lambda s : s[0:-s[-1]]

			# Decrypt
			text = unpad(cipher.decrypt(ciphertext))

			# return as string
			return text.decode("utf-8")

		# Class.LivePlayer.Start() : void
		# Class.LivePlayer.authkeyLoadComplete(Event) : void
		def getAuthKey(self):
			self.log("Get AuthKey")
			url = "http://api.sbs.co.kr/vod/_v1/Onair_Media_Auth_Security.jsp?playerType=flash&channelPath=%s&streamName=%s&rnd=%i" \
				% (self.streamPath, self.stream, random.getrandbits(8))
			return self.decrypt(vlyc.network.retrieve(url)).split("?")[-1]

		# Config
		# Controls.LivePlayer.SetSource(url: string, start: bool) : void
		def _setSource(self, url):
			self.source = url
			parts = url.split("/")
			self.host = "rtmp://" + parts[2]
			self.streamPath = parts[3]
			self.stream = parts[4]

		# Class.LiveChannelInitializer.*
		def getConfig(self):
			self.log("Get Config")
			url = "http://sbsplayer.sbs.co.kr/OnAir/env/%s.xml?ControlSetting=YYYYYYY" % self.videoId
			self.config = etree.parse(io.BytesIO(vlyc.network.retrieve(url)))
			self.title = self.config.find(".//ChannelName").text
			self._setSource(self.config.find(".//SourceURL").text)
			#self.captionurl = self.config.find(".//CaptionSourceURL").text
			self.log("Config: %s @ %s", self.title, self.source)

		# Load
		# Controls.LivePlayer.Play() : void
		def _load(self, done, throw):
			self.author = "에스비에스"

			self.getConfig()
			self.authKey = self.getAuthKey()
			self.log("AuthKey: %s", self.authKey)

			self.app = self.streamPath + "?" + self.authKey

			self.availableQualities.append((vlyc.plugin.VideoQualityLevel.QA_LOWEST, "Default"))
			
			# ffmpeg with librtmp
			#self.rtmp_opts = {
			#	"swfUrl": "http://vod.sbs.co.kr/onair/NeTVOnAir_%s.swf?dd=9" % self.version.replace(".", "_"),
			#	"tcUrl": self.host + "/" + self.app,
			#	"playpath": self.stream,
			#	"live": "1",
			#	"flashVer": "LIN 19,0,0,226",
			#	"app": self.app,
			#	"pageUrl": "http://vod.sbs.co.kr/onair/onair_index.jsp?Channel=%s&div=pc_onair" % self.videoId,
			#}
			#self.rtmp_url = " ".join([self.source] + ["%s=%s" % (k, rtmp_escape(v)) for k, v in self.rtmp_opts.items()])
			
			# Should probably work generically (TM)
			self.rtmp_url = "%s/%s/%s" % (self.host, self.app, self.stream)
			
			self.log("URI: %s", self.rtmp_url)

			done()
		
		def load(self, done, throw):
			try:
				return self._load(done, throw)
			except:
				import traceback
				print(traceback.format_exc())
				throw("exc")

		def getMedia(self, quality, media, throw):
			media(quality, "Default", self.rtmp_url)

		def getSubtitles(self, language, subtitles, throw):
			throw("ERROR!")
