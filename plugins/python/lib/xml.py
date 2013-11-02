#----------------------------------------------------------------------
#- libyo.xml
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

"""
A XML Parser using etree
"""

from __future__ import absolute_import, unicode_literals, division

# Imports
import logging
logger = logging.getLogger(__name__)

import io

HAVE_EXPAT = True
try:
    import xml.parsers.expat as expat
except ImportError:
    try:
        import pyexpat as expat
    except ImportError:
        HAVE_EXPAT = False

from .etree import *  # @UnusedWildImport

# Exports
__all__ = ["ElementTree", "Element", "Comment", "ProcessingInstruction",
           "SubElement",
           "TreeBuildError", "EmptyTree", "AmbiguousRoot", "TagMismatch",
           "TreeBuilder", "XMLSerializer",
           "ParseError", "XMLParser", "parse", "fragment_fromstring"]


class ElementTree(ElementTree):
    def parse(self, source, parser=None):
        if parser is None:
            parser = XMLParser(TreeBuilder())
        return super(ElementTree, self).parse(source, parser)


class ParseError(SyntaxError):
    pass


class XMLParser(object):
    """ The XML Parser (uses expat) """

    if HAVE_EXPAT:
        def __init__(self, target=None, encoding=None):
            self.parser = expat.ParserCreate(encoding, "}")
            if target is None:
                target = TreeBuilder()
            self.target = target

            # Create Parser
            self.parser.DefaultHandlerExpand = self._default
            if hasattr(self.target, "start"):
                self.parser.StartElementHandler = self._start
            if hasattr(self.target, "end"):
                self.parser.EndElementHandler = self._end
            if hasattr(self.target, "data"):
                self.parser.CharacterDataHandler = self.target.data
            if hasattr(self.target, "comment"):
                self.parser.CommentHandler = self.target.comment
            if hasattr(self.target, "pi"):
                self.parser.ProcessingInstructionHandler = self.target.pi

            try:
                self.parser.buffer_text = 1
            except AttributeError:
                pass

            try:
                self.parser.ordered_attributes = 1
                self.parser.specified_attributes = 1
                if hasattr(self.target, "start"):
                    self.parser.StartElementHandler = self._start2
            except AttributeError:
                pass

            self._doctype = None
    else:
        def __init__(self, target=None, encoding=None):
            raise ImportError("no module named xml.parsers.expat")

    def _error(self, value, string=None):
        line = string.split("\n")[value.lineno - 1]
        err = ParseError(value, ("XML", value.lineno, value.offset, line))
        err.code = value.code
        err.position = value.lineno, value.offset
        return err

    def _fixname(self, name):
        if "}" in name:
            return "".join(("{", name))
        return name

    def _start(self, tag, attrib_in):
        return self.target.start(self._fixname(tag), {self._fixname(name): value for name, value in attrib_in.items()})

    def _start2(self, tag, attrib_in):
        return self.target.start(self._fixname(tag), {self._fixname(attrib_in[i]): attrib_in[i + 1] for i in range(0, len(attrib_in), 2)})

    def _end(self, tag):
        return self.target.end(self._fixname(tag))

    def _default(self, text):
        """ Default handler (same as the ElementTree version)"""
        prefix = text[:1]
        if prefix == "&":
            err = expat.error(
                "undefined entity %s: line %d, column %d" %
                (text, self.parser.ErrorLineNumber,
                self.parser.ErrorColumnNumber)
                )
            err.code = 11 # XML_ERROR_UNDEFINED_ENTITY
            err.lineno = self.parser.ErrorLineNumber
            err.offset = self.parser.ErrorColumnNumber
            raise err
        elif prefix == "<" and text[:9] == "<!DOCTYPE":
            self._doctype = [] # inside a doctype declaration
        elif self._doctype is not None:
            # parse doctype contents
            if prefix == ">":
                self._doctype = None
                return
            text = text.strip()
            if not text:
                return
            self._doctype.append(text)
            n = len(self._doctype)
            if n > 2:
                type_ = self._doctype[1]
                if type_ == "PUBLIC" and n == 4:
                    name, type_, pubid, system = self._doctype
                    if pubid:
                        pubid = pubid[1:-1]
                elif type_ == "SYSTEM" and n == 3:
                    name, type_, system = self._doctype
                    pubid = None
                else:
                    return
                if hasattr(self.target, "doctype"):
                    self.target.doctype(name, pubid, system[1:-1])
                self._doctype = None

    def feed(self, data):
        try:
            self.parser.Parse(data, 0)
        except expat.error as e:
            raise self._error(e, data)

    def close(self):
        try:
            self.parser.Parse("", 1)
        except expat.error as e:
            raise self._error(e)
        try:
            return self.target.close()
        except AttributeError:
            pass
        finally:
            del self.parser
            del self.target


def parse(source, parser=None):
    """
    Parses the named file or url, or if the object has a .read() method, parses from that.
    """
    tree = ElementTree()
    tree.parse(source, parser)
    return tree


def fragment_fromstring(text, parser=None, create_parent=None):
    """
    Returns an XML fragment from a string. The fragment must contain just a single element, unless create_parent is given; e.g,. fragment_fromstring(string, create_parent='div') will wrap the element in a <div>.
    """
    if parser is None:
        parser = XMLParser(TreeBuilder())
    if create_parent:
        parser.feed("<%s>" % create_parent)
    parser.feed(text)
    if create_parent:
        parser.feed("</%s>" % create_parent)
    return parser.close()

XML = fragment_fromstring


# Serialization
class XMLSerializer(TreeSerializer):
    @staticmethod
    def _escape_data(text):
        try:
            if "&" in text:
                text = text.replace("&", "&amp;")
            if "<" in text:
                text = text.replace("<", "&lt;")
            if ">" in text:
                text = text.replace(">", "&gt;")
            return text
        except (AttributeError, TypeError):
            raise TypeError("Data is not str: %s" % text)

    @staticmethod
    def _escape_attrib(text):
        try:
            if "&" in text:
                text = text.replace("&", "&amp;")
            if "<" in text:
                text = text.replace("<", "&lt;")
            if ">" in text:
                text = text.replace(">", "&gt;")
            if "\"" in text:
                text = text.replace("\"", "&quot;")
            if "\n" in text:
                text = text.replace("\n", "&#10;")
            return text
        except (AttributeError, TypeError):
            raise TypeError("Attribute value is not str: %s" % text)

    def __init__(self, file):
        super(XMLSerializer, self).__init__()
        self.file = file

    def walk_starttag(self, tag, attrib):
        if attrib:
            self.file.write("<%s %s>" % (tag, " ".join(("%s=\"%s\"" % (k, self._escape_attrib(v)) for k, v in sorted(attrib.items())))))
        else:
            self.file.write("<%s>" % tag)

    def walk_endtag(self, tag):
        self.file.write("</%s>" % tag)

    def walk_emptytag(self, tag, attrib):
        if attrib:
            self.file.write("<%s %s />" % (tag, " ".join(("%s=\"%s\"" % (k, self._escape_attrib(v)) for k, v in sorted(attrib.items())))))
        else:
            self.file.write("<%s/>" % tag)

    def walk_data(self, data):
        self.file.write(self._escape_data(data))


def tostring(root):
    fp = io.StringIO()
    s = XMLSerializer(fp)
    s.serialize(root)
    return fp.getvalue()
