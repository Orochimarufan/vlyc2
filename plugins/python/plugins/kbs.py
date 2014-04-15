#!/usr/bin/env python3
# (c) 2013 Orochimarufan
# This script may not be used to harm KBS or anyone else in any way.

# KBS uses cross-flash-javascript callbacks to support both Flash and HTML5 (latter for Mobile platforms)
# The PC/Flash streaming seems to be somehow implemented with P2P.
# Will use Mobile for now.
# http://211.233.93.75/assets/js/live.js

import vlyc.plugin
import http.client
import urllib.parse
import json
import io

try:
	from lxml import html
except InputError:
	from vlyc import html


class KBSLivePlugin(vlyc.plugin.SitePlugin):
	def forUrl(self, url):
		if url.scheme() == "kbs":
			return str(url)[6:]

	def video(self, channel):
		return KBS_LiveStream(channel)


class KBS_LiveStream(vlyc.plugin.Video):
	def __init__(self, channel):
		super(KBS_LiveStream, self).__init__(channel)
		self.pk_token = None

	def get_token(self):
		url = "http://211.233.93.75/live_popup"
		data = vlyc.network.retrieve(url)
		dom = html.parse(io.BytesIO(data))
		form = dom.get_element_by_id("token_form")
		return form[0][0].get("value")

	def ajax_json_load(self, url, data, method):
		if self.pk_token is None:
			self.pk_token = self.get_token()
		data["pk_token"] = self.pk_token

		resource = urllib.parse.urlparse(url)
		scheme = resource.scheme.lower()

		if resource.hostname == "":
			resource.hostname = "211.233.93.75"

		if scheme in ("http", "https"):
			if scheme == "https":
				conn = http.client.HTTPSConnection(resource.hostname, resource.port)
			else:
				conn = http.client.HTTPConnection(resource.hostname, resource.port)

			conn.connect()
			
			conn.request(method, url.split("/", 3)[3], json.dumps(data))

			response = conn.getresponse()

			if response.status != 200:
				return None

			result = json.loads(response)

		return result

	def channel_master_items_ajax(self, channel_type):
		data = {
			"src_channel_type": channel_type,
			"os_type": "android",
		}

		rtn = self.ajax_json_load('/broadcast_live/channel_master_items_json', data, 'POST')
		 function(my_rtn) {
        if (my_rtn.result == true && my_rtn.result_data != null) {
            channel_master_items_result(channel_type, my_rtn.result_data, episode_reload);
        }
    });


	def load(self, done, throw):
		self.author = "KBS"

