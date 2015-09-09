#----------------------------------------------------------------------
#- vlyc2 builtin libs: plugin sugar
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

from .. import _state
import inspect
import abc

class VideoQualityLevel:
	"""
	int constants:
		QA_INVALID	[-1]
		QA_LOWEST	[0]
		QA_360		[100]
		QA_480		[200]
		QA_720		[300]
		QA_1080		[400]
		QA_4K		[600]
		QA_HIGHEST	[1024]
	"""
	QA_INVALID = -1
	QA_LOWEST  = 0
	QA_360     = 100
	QA_480     = 200
	QA_720     = 300
	QA_1080    = 400
	QA_4K      = 600
	QA_HIGHEST = 1024


class _PythonSitePluginMeta(type):
	def __init__(self, name, bases, dict):
		super(_PythonSitePluginMeta, self).__init__(name, bases, dict)
		if bases == ():
			return # the original SitePlugin class
		for name in "name", "author", "rev":
			if name not in self.__dict__:
				raise TypeError("SitePlugin subclasses must define the 'name:str', 'author:str' and 'rev:int' properties")
		obj = self()
		for name in "forUrl", "video":
			inspect.signature(getattr(obj, name)).bind("singleArg")
		_state.register_site(obj)


class SitePlugin(metaclass=_PythonSitePluginMeta):
	"""
	Members:
		str name	:: the Plugin name
		str author	:: the Plugin author
		int rev		:: the Plugin version/revision

        Methods:
		str forUrl(QUrl url) :: get the videoId for url `url`. Return empty string if you cannot handle `url`
		Video video(str video_id) :: return a video object for video_id
	"""


class Video(metaclass=abc.ABCMeta):
	def __init__(self, video_id):
		self.videoId = video_id
		self.useFileMetadata = False
		self.title = "Unknown Title"
		self.author = "Unknown Author"
		self.description = "No description"
		self.views = 0
		self.likes = 0
		self.dislikes = 0
		self.favorites = 0
		self.availableQualities = []
		self.availableSubtitleLanguages = []

	@abc.abstractmethod
	def load(self, done, throw):
		"""
		callable done()
		callable throw(str reason)
		"""
		pass

	@abc.abstractmethod
	def getMedia(self, quality, media, throw):
		"""
		int quality
		callable media(str quality, str description, str url)
		callable throw(str reason)
		"""
		pass

	def getSubtitles(self, language, subtitles, throw):
		"""
		str language
		callable subtitles(str language, str type, QUrl|str|bytes url|data)
			url/data parameter: QUrl url -> load from external resource
								str|bytes data -> load from utf8 blob
		callable throw(reason)
		"""
		pass
