"""
----------------------------------------------------------------------
- utils.utils: random utilities
----------------------------------------------------------------------
- Copyright (C) 2011-2012  Orochimarufan
-                 Authors: Orochimarufan <orochimarufan.x3@gmail.com>
-
- This program is free software: you can redistribute it and/or modify
- it under the terms of the GNU General Public License as published by
- the Free Software Foundation, either version 3 of the License, or
- (at your option) any later version.
-
- This program is distributed in the hope that it will be useful,
- but WITHOUT ANY WARRANTY; without even the implied warranty of
- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
- GNU General Public License for more details.
-
- You should have received a copy of the GNU General Public License
- along with this program.  If not, see <http://www.gnu.org/licenses/>.
----------------------------------------------------------------------
"""
from __future__ import absolute_import, unicode_literals, division

from ..urllib import parse as urlparse


def listreplace(lis, old, new):
    return [(i if i != old else new) for i in lis]


def listreplace_s(lis, old, new):
    return [i.replace(old, new) for i in lis]


def getkv(string, delim="=", unq=0):
    """turns string "key{delim}value" into tuple ("key","value").
    if UNQ AND UNQP are TRUE it will UrlUnQuote_Plus the value
    if UNQ BUT NOT UNQP is TRUE it will UrlUnQuote the value."""
    x = string.split(delim)
    if unq == 1:
        return x[0], urlparse.unquote_plus(x[1])
    elif unq == 2:
        return x[0], urlparse.unquote(x[1])
    else:
        return x


def sdict_parser(string, kvdelim="=", delim="&", unq=1):
    """Parse Strings:
    "a=b&b=c&k=v&key=value"
    
    Parameters:
        string: the string to parse
        kvdelim: the key/value delimiter (default="=")
        delim: the pair delimiter (default="&")
        unq: UrlUnquote values: 0=No 2=Yes 1=Plus"""
    return dict([getkv(i, kvdelim, unq) for i in string.split(delim)])


def mega(iIn, bStrOut=False, bBase10=False, sStrStr=" "):
    if bBase10:
        base = 1000
    else:
        base = 1024
    table = {1024: ["B", "kiB", "MiB", "GiB", "TiB"],
             1000: ["B", "kB", "MB", "GB", "TB"]}
    x = 0
    i = int(iIn)
    while i > base:
        x += 1
        i = i / base
        if x == len(table[base]):
            break
    return ("{0:.2f}{1}{2}".format(round(i, 2), sStrStr, table[base][x]) if bStrOut else (i, table[base][x]))
