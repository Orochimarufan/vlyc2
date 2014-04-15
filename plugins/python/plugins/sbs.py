#!/usr/bin/python3

# SBS Live stream.
# NOTE: your libvlc/ffmpeg needs to be linked against librtmp

import vlyc.plugin
import re
import io
import hashlib
import datetime
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
	rev=1

	def forUrl(self, url):
		if url.scheme() == "sbs":
			return str(url)[6:]

	def video(self, video_id):
		return Video(self, video_id)


class Video(vlyc.plugin.Video):
		playerrev_regexp = re.compile(br'flash\\/(.+)\\/StandardVideoPlayer.swf')

		def __init__(self, plugin, video_id):
			super(Video, self).__init__(video_id.upper())
			self.plugin = plugin

		# Auth Key
		# Class.Crypt.decrypt(param1: String) : String
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

		def getAuthKey(self):
			url = "http://api.sbs.co.kr/vod/_v1/Onair_Media_Auth_Security.jsp?playerType=flash&channelPath=%s&streamName=%s&rnd=%i" \
				% (self.streamPath, self.stream, random.getrandbits(8))
			return self.decrypt(vlyc.network.retrieve(url)).split("?")[-1]

		# Config
		def setSource(self, url):
			parts = url.split("/")
			self.host = "rtmp://" + parts[2]
			self.streamPath = parts[3]
			self.stream = parts[4]

		def getConfig(self):
			url = "http://sbsplayer.sbs.co.kr/OnAir/env/%s.xml?ControlSetting=YYYYYYY" % self.videoId
			self.config = etree.parse(io.BytesIO(vlyc.network.retrieve(url)))
			self.title = self.config.find(".//ChannelName").text
			self.setSource(self.config.find(".//SourceURL").text)

		# Load
		def load(self, done, throw):
			self.author = "에스비에스"

			self.getConfig()
			self.auth_key = self.getAuthKey()

			self.app = self.streamPath + "?" + self.auth_key

			self.availableQualities.append((vlyc.plugin.VideoQualityLevel.QA_LOWEST, "Default"))

			done()

		def getMedia(self, quality, media, throw):
			RTMP_OPTS = {
				"swfUrl": "http://vod.sbs.co.kr/onair/NeTVOnAir_1_1_0_0.swf",
				"tcUrl": self.host + "/" + self.app,
				"playpath": self.stream,
				"live": "1",
				#"flashVer": "LNX 11,9,900,170",
				"app": self.app,
				"pageUrl": "http://vod.sbs.co.kr/onair/onair_index.jsp?Channel=%s" % self.videoId,
				# FIXME: do it properly!
				"socks": "localhost:12345",
			}
			rtmp_url = " ".join([self.host + "/" + self.app] + ["%s=%s" % (k, rtmp_escape(v)) for k, v in RTMP_OPTS.items()])
			print("RTMP: %s"% rtmp_url)
			media(quality, "Default", rtmp_url)

		def getSubtitles(self, language, subtitles, throw):
			throw("ERROR!")
