#!/usr/bin/python3
#----------------------------------------------------------------------
#- vlyc2 python UrlHandler plugin
#----------------------------------------------------------------------
#- Copyright (C) 2014      Taeyeon Mori
#-                Authors: Taeyeon Mori <orochimarufan.x3@gmail.com>
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

from . import plugin


class UrlHandlerPlugin(plugin.Plugin):
	@classmethod
	def _vp_types(cls):
		return super()._vp_types() + ["url-handler"]
	
	@plugin.abc.abstractmethod
	def handleUrl(self, url):
		"""
		@brief Try to handle an URL
		@param url a string containing the URL in question
		@return a vlyc.RObject or None
		"""
		pass
