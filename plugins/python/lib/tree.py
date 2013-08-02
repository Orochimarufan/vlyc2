#----------------------------------------------------------------------
#- libyo.tree
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
Simple Tree Implementation
"""

from __future__ import absolute_import, unicode_literals

# Imports
from collections import deque

_reversed = reversed

# Exports
__all__ = ["Element"]


class Element(object):
    """
    A Tree Element
    """
    __slots__ = ("_parent", "_children", "__weakref__")

    def __init__(self):
        self._parent = None
        self._children = list()

    @property
    def _index(self):
        return self._parent.index(self)

    # sequence api
    def __contains__(self, elem):
        return elem in self._children

    def __delitem__(self, index):
        del self._children[index]

    def __getitem__(self, index):
        return self._children[index]

    def __iter__(self):
        return iter(self._children)

    def __len__(self):
        return len(self._children)

    def __reversed__(self):
        return reversed(self._children)

    def __setitem__(self, index, elem):
        elem._assign(self)
        self._children[index] = elem

    def _assign(self, parent):
        """
        Move self to a new parent
        """
        if self._parent is not None:
            self._parent.remove(self)
        self._parent = parent

    # etree api
    def addnext(self, elem):
        """
        Adds the element as a following sibling directly after this element.

        This is normally used to set a processing instruction or comment after the root node of a document. Note that tail text is automatically discarded when adding at the root level.
        """
        self._parent.insert(self._index + 1, elem)

    def addprevious(self, elem):
        """
        Adds the element as a preceding sibling directly before this element.

        This is normally used to set a processing instruction or comment before the root node of a document. Note that tail text is automatically discarded when adding at the root level.
        """
        self._parent.insert(self._index, elem)

    def append(self, elem):
        """
        Adds a subelement to the end of this element.
        """
        elem._assign(self)
        self._children.append(elem)

    def extend(self, elements):
        """
        Extends the current children by the elements in the iterable.
        """
        for elem in elements:
            self.append(elem)

    def getnext(self):
        """
        Returns the following sibling of this element or None.
        """
        ix = self._index
        if ix == len(self._parent) - 1:
            return None
        return self._parent[ix + 1]

    def getparent(self):
        """
        Returns the parent of this element or None for the root element.
        """
        return self._parent

    def getprevious(self):
        """
        Returns the preceding sibling of this element or None.
        """
        ix = self._index
        if ix == 0:
            return None
        return self._parent[ix - 1]

    def getroot(self):
        """
        Returns the tree root Element
        """
        for ancestor in self.iterancestors():
            pass
        return ancestor

    def index(self, elem, start=None, stop=None):
        """
        Find the position of the child within the parent.
        """
        if stop == None and start in (None, 0):
            return self._children.index(elem)

        if start == None:
            start = 0
        if stop == None:
            stop = len(self._children) - 1

        if elem not in self._children:
            raise ValueError("list.index(x): x not in list")
        elif elem not in self._children[start:stop]:
            raise ValueError("list.index(x): x not in slice")
        else:
            return self._children.index(elem)

    def insert(self, index, element):
        """
        Inserts a subelement at the given position in this element
        """
        element._assign(self)
        self._children.insert(index, element)

    def iter(self, *tags):
        """
        Iterate over all elements in the subtree in document order (depth first pre-order), starting with this element.

        Can be restricted to find only elements with a specific tag.

        Passing a sequence of tags will let the iterator return all elements matching any of these tags, in document order.
        """
        stack = deque([self])
        if tags:
            while stack:
                node = stack.pop()
                if node.tag in tags:
                    yield node
                stack.extend(reversed(node))
        else:
            while stack:
                node = stack.popleft()
                yield node
                stack.extend(reversed(node))

    def iterancestors(self, *tags):
        """
        Iterate over the ancestors of this element (from parent to parent).
        """
        x = self._parent
        while x is not None:
            if not tags or x.tag in tags:
                yield x
            x = x.getparent()

    def iterchildren(self, tag=None, reversed=False, *tags):  # @ReservedAssignment
        """
        Iterate over the children of this element.

        As opposed to using normal iteration on this element, the returned elements can be reversed with the 'reversed' keyword and restricted to find only elements with a specific tag
        """
        if tags is None:
            tags = list()
        if tag is not None:
            tags.insert(0, tag)
        x = _reversed(self._children) if reversed else iter(self._children)
        if tags:
            return (child for child in x if child.tag in tags)
        else:
            return x

    def iterdescendants(self, *tags):
        """
        Iterate over the descendants of this element in document order.

        As opposed to el.iter(), this iterator does not yield the element itself. The returned elements can be restricted to find only elements with a specific tag, see iter.
        """
        stack = deque(reversed(self._children))
        if tags:
            while stack:
                node = stack.pop()
                if node.tag in tags:
                    yield node
                stack.extend(reversed(node))
        else:
            while stack:
                node = stack.pop()
                yield node
                stack.extend(reversed(node))

    def itersiblings(self, tag=None, preceding=False, *tags):
        """
        Iterate over the following or preceding siblings of this element.

        The direction is determined by the 'preceding' keyword which defaults to False, i.e. forward iteration over the following siblings. When True, the iterator yields the preceding siblings in reverse document order, i.e. starting right before the current element and going backwards.

        Can be restricted to find only elements with a specific tag
        """
        if tags is None:
            tags = list()
        if tag is not None:
            tags.insert(0, tag)
        x = reversed(self._parent[:self._index]) if preceding else iter(self._parent[self._index + 1:])
        if tags:
            return (sib for sib in x if sib.tag in tags)
        else:
            return x

    def remove(self, elem):
        """
        Removes a matching subelement. Unlike the find methods, this method compares elements based on identity, not on tag value or contents.
        """
        self._children.remove(elem)

    def replace(self, elem, new_elem):
        """
        Replaces a subelement with the element passed as second argument.
        """
        new_elem._assign(self)
        self._children[self._children.index(elem)] = new_elem
