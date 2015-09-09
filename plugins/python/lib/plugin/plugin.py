#!/usr/bin/python3
#----------------------------------------------------------------------
#- vlyc2 python plugin base
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

from .. import _state

import abc


class PluginType(abc.ABCMeta):	
	def register(cls):
		if not _state.is_loader_active:
			raise RuntimeError("Cannot register plugin after the file is initialized")
		
		_state.register_plugin(cls._vp_types(), cls.metadata, cls)


class Plugin(metaclass=PluginType):
	@classmethod
	def _vp_types(cls):
		return ["plugin"]

	def __init__(self):
		super().__init__()

		if not hasattr(self, id):
			self.id = self.metadata["id"]
	
	def init(self, e):
		pass
