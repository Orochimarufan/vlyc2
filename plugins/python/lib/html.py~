#----------------------------------------------------------------------
#- libyo.html
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
HTML Parser on top of etree
"""

from __future__ import absolute_import, unicode_literals, division

# Imports
import logging
logger = logging.getLogger(__name__)

from collections import deque

from .compat.uni import unichr
from .compat.html import entities, parser
from .compat import PY3

from .etree import *  # @UnusedWildImport

# Exports
__all__ = ["ElementTree", "Element", "Comment", "ProcessingInstruction",
           "SubElement", "Document",
           "TreeBuildError", "EmptyTree", "AmbiguousRoot", "TagMismatch",
           "TreeBuilder", "HTMLParser", "parse", "fragment_fromstring"]


class Document(ElementTree):
    """
    A DOM Document
    """
    def get_element_by_id(self, id_, default=None):
        return self._root.get_element_by_id(id_)

    def find_class(self, name):
        return self._root.find_class(name)

    def parse(self, source, parser=None):
        if parser is None:
            parser = HTMLParser(TreeBuilder())
        return super(Document, self).parse(source, parser)


ElementTree = Document


class Mixin(object):
    __slots__ = ()

    # lxml.html api
    def drop_tree(self):
        """
        Drops the element and all its children.
        Unlike el.getparent().remove(el) this does not remove the tail text;
        with drop_tree the tail text is merged with the previous element.
        """
        prev = self.getprevious()
        if prev.tail is not None:
            prev.tail += self.tail
        else:
            prev.tail = self.tail
        self.getparent().remove(self)

    def drop_tag(self):
        """
        Remove the tag, but not its children or text. The children and text are merged into the parent.

        Example:

        >>> h = fragment_fromstring('<div>Hello <b>World!</b></div>')
        >>> h.find('.//b').drop_tag()
        >>> print(tostring(h, encoding=unicode))
        <div>Hello World!</div>
        """
        ix = self._index
        parent = self.getparent()
        if ix == 0:
            if parent.text is not None:
                parent.text += self.text + self.tail
            else:
                parent.text = self.text + self.tail
        else:
            elem = self.getprevious()
            if elem.tail is not None:
                elem.tail += self.text + self.tail
            else:
                elem.tail = self.text + self.tail
        for child in reversed(self._children):
            parent.insert(ix, child)
        parent.remove(self)

    def find_class(self, name):
        """
        Returns a list of all the elements with the given CSS class name.
        Note that class names are space separated in HTML, so doc.find_class_name('highlight')
        will find an element like <div class="sidebar highlight">. Class names are case sensitive.
        """
        return [elem for elem in self.iter() if name in elem.get("class", "").split(" ")]

    """TODO
    def find_rel_links(self, rel):
    """

    def get_element_by_id(self, id_, default=None):
        """
        Return the element with the given id_, or the default if none is found.
        If there are multiple elements with the same id_ (which there shouldn't be, but there often is),
        this returns only the first.
        """
        for elem in self.iter():
            if elem.get("id", None) == id_:
                return elem

    def _iter_text_content(self):
        if self.text is not None:
            yield self.text
        stack = deque()
        node = self
        index = 0
        while True:
            if len(node) > index:
                index += 1
                node = node[index]
                stack.append(index)
                if node.text is not None:
                    yield node.text
                index = 0
            else:
                if not stack:
                    return
                if node.tail is not None:
                    yield node.tail
                index = stack.pop()
                node = node.getparent()

    def text_content(self):
        """
        Returns the text content of the element, including the text content of its children, with no markup.
        """
        return "".join(self._iter_text_content())

    def getroottree(self):
        """
        Return a Document instance for the root node of the document that contains this element.

        This is the same as following element.getparent() up the tree until it returns None (for the root element) and then build a Document for the last parent that was returned.
        """
        return Document(self.getroot())


class Element(Element, Mixin):
    __slots__ = ()

    def __repr__(self):
        """
        A readable representation
        """
        return "".join((
            "<%s Element" % self.tag,
            " id=%s" % self._attrib["id"] if "id" in self._attrib else "",
            " class=%s" % [i for i in self._attrib["class"].split(" ") if i] if "class" in self._attrib else "",
            " at 0x%x>" % id(self)))


class Comment(Comment, Mixin):
    __slots__ = ()


class ProcessingInstruction(ProcessingInstruction, Mixin):
    __slots__ = ()


class TreeBuilder(TreeBuilder):
    """
    The Tree Builder, HTML edition
    this does much magic to clean up (invalid) HTML trees
    """
    self_closing = ("area", "base", "br", "col", "command", "embed", "hr",
                    "img", "input", "keygen", "link", "meta", "param",
                    "source", "track", "wbr")

    def end(self, tag):
        """
        Close a tag.
        and try to clean up messy html.
        """
        self.flushBuffer()
        # Handle self-closing (void-) tags
        # TODO: improve self-closing handling
        while self[-1].tag != tag and len(self) > 2 and self[-1].tag in self.self_closing:
            # move all children to the upper element
            self[-2].extend(self[-1])
            self.pop()

        # try to fix broken html trees
        if self[-1].tag != tag:
            # maybe the last tag was closed twice?
            for elem in self:
                if elem.tag == tag:
                    # we found a matching open tag, so we'll assume that it was not
                    break
            else:
                # there are no matching open tags, so we'll assume it was
                logger.warn("Malformed HTML: %s closed twice (near: %s, stack: %s)" % (tag, repr(self[-1]), self._debugtags()))
                return

            # FIXME: close all inner tags?
            logger.warn("Malformed HTML: %s closed but last element on stack is %s (stack: %s)" % (tag, repr(self[-1]), self._debugtags()))
            while len(self) > 1 and self[-1].tag != tag:
                self.pop()

        # pop the tag
        return self.pop()

    #def close(self):
    #    if self.root is None:
    #        raise EmptyTree("Missing root element: tree empty")
    #    # make missing end tags non-critical
    #    if len(self):
    #        logger.warn("Malformed HTML: Missing %i end tags: %s, buffer: %s" % (len(self), list(reversed(self._debugtags())), self.buffer))
    #    return self.root

    # We have our own Factories
    default_factory = Element
    comment_factory = Comment
    pi_factory = ProcessingInstruction


class HTMLParser(parser.HTMLParser):
    """
    The stdlib-HTMLParser interface
    """
    def __init__(self, target=None, strict=False):
        # in python 2.x, HTMLParser is a old-style class and doesn't support strict
        if PY3:
            super(HTMLParser, self).__init__(strict)
        else:
            parser.HTMLParser.__init__(self)

        if target is None:
            target = TreeBuilder()
        self.target = target

    def handle_starttag(self, name, attrs):
        self.target.start(name, attrs)

    def handle_endtag(self, name):
        self.target.end(name)

    def handle_data(self, data):
        self.target.data(data)

    def handle_charref(self, name):
        if name.startswith("x"):
            i = int(name.lstrip("x"), 16)
        else:
            i = int(name)
        try:
            c = unichr(i)
        except (ValueError, OverflowError):
            c = ""
        self.handle_data(c)

    def handle_entityref(self, name):
        i = entities.name2codepoint.get(name)
        if i is not None:
            c = unichr(i)
        else:
            c = "&{0};".format(name)
        self.handle_data(c)

    def handle_comment(self, data):
        self.target.comment(data)

    def handle_pi(self, data):
        if data.endswith("?") and data.lower().startswith("xml"):
            # "An XHTML processing instruction using the trailing '?'
            # will cause the '?' to be included in data." - HTMLParser
            # docs.
            #
            # Strip the question mark so we don't end up with two
            # question marks.
            data = data[:-1]
        self.target.pi(data)

    def close(self):
        if PY3:
            super(HTMLParser, self).close()
        else:
            parser.HTMLParser.close(self)
        if hasattr(self.target, "close"):
            return self.target.close()

    def feed(self, data):
        # convert to str
        if isinstance(data, bytes):
            try:
                data = data.decode("UTF-8")
            except UnicodeDecodeError:
                data = data.decode("Latin-1")
        return super(HTMLParser, self).feed(data)


def parse(source, parser=None):
    """
    Parses the named file or url, or if the object has a .read() method, parses from that.
    """
    document = Document()
    document.parse(source, parser)
    return document


def fragment_fromstring(text, parser=None, create_parent=None):
    """
    Returns an HTML fragment from a string. The fragment must contain just a single element, unless create_parent is given; e.g,. fragment_fromstring(string, create_parent='div') will wrap the element in a <div>.
    """
    if parser is None:
        parser = HTMLParser(TreeBuilder())
    if create_parent:
        parser.feed("<%s>" % create_parent)
    parser.feed(text)
    if create_parent:
        parser.feed("</%s>" % create_parent)
    return parser.close()
