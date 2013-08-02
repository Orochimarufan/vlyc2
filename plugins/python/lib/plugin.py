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

from . import registrar
import inspect

class _PythonSitePluginMeta(type):
	def __init__(self, name, bases, dict):
		super(_PythonSitePluginMeta, self).__init__(name, bases, dict)
		if bases == ():
			return # the original SitePlugin class
		for name in "name", "author", "rev":
			if name not in self.__dict__:
				raise TypeError("SitePlugin subclasses must define the 'name:str', 'author:str' and 'rev:int' properties")
		if "simpleLoadVideo" in self.__dict__ and "loadVideo" not in self.__dict__:
			def loadVideo(self, video):
				self.simpleLoadVideo(video)
				if not video.isDone():
					video.setDone()
			self.__dict__["loadVideo"] = loadVideo
		for name in "forUrl", "loadVideo":
			if name not in self.__dict__:
				raise TypeError("SitePlugin subclasses must define the 'forUrl(url:str)' and 'loadVideo(video:Video)' methods")
		obj = self()
		for name in "forUrl", "loadVideo":
			inspect.signature(getattr(obj, name)).bind("singleArg")
		registrar.registerSite(obj.name, obj.author, obj.rev, obj.forUrl, obj.loadVideo)


class SitePlugin(metaclass=_PythonSitePluginMeta):
	"""
	Members:
		str name	:: the Plugin name
		str author	:: the Plugin author
		int rev		:: the Plugin version/revision

		str forUrl(str url) :: get the videoId for url `url`. Return empty string if you cannot handle `url`
		void loadVideo(Video video) :: populate the Video object `video`. Must call either video.setDone() or video.setError(str message)
	"""

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
