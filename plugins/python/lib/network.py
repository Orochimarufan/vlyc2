# (c) 2014 Taeyeon Mori
# Vlyc2 Network stuff

import urllib.error

from PythonQt import QtCore
from PythonQt import QtNetwork

from . import _state

# --------------------------------------------------------------------------------------
# Public
def getNAM():
    """
    Get the QNetworkAccessManager instance
    """
    return _state.network_access_manager


def retrieve(url):
    """
    Get the contents of \c url
    """
    return _state.network_access_manager.retrieve(url)


def get(url):
    """
    Execute a GET request on \c url
    """
    return QNetworkReplyToReply(_state.network_access_manager.syncGet(QtNetwork.QNetworkRequest(QtCore.QUrl(url))))


def urlopen(request):
    """
    urllib.request.urlopen replacement
    """
    req, verb, data = RequestToQNetworkRequest(request)
    #print("%s %s" % (verb.decode("ascii"), req.url()))
    return QNetworkReplyToReply(_state.network_access_manager.syncRequest(req, verb, data, True))


# --------------------------------------------------------------------------------------
# Utilities
def RequestToQNetworkRequest(request):
    """
    Convert a Urllib Request to a QtNetwork one.
    """
    if isinstance(request, str):
        # From string
        return QtNetwork.QNetworkRequest(QtCore.QUrl(request)), b"GET", QtCore.QByteArray()
    else:
        # From Urllib Request
        qnrequest = QtNetwork.QNetworkRequest(QtCore.QUrl(request.full_url))
    
        # Headers
        for name, value in request.header_items():
            if not isinstance(name, bytes):
                name = bytes(name, "ascii")
            if not isinstance(value, bytes):
                value = bytes(value, "utf-8")
            qnrequest.setRawHeader(name, value)

        # Method, Data
        data = request.data
        if data is None:
            data = QtCore.QByteArray()
        elif isinstance(data, str):
            data = bytes(data, "utf-8")
        return qnrequest, bytes(request.get_method(), "ascii"), data


def QNetworkReplyToReply(qnreply):
    if (qnreply.error() != QtNetwork.QNetworkReply.NoError):
        if qnreply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute) is not None:
            raise HTTPError(qnreply)
        else:
            qnreply.deleteLater()
            raise URLError(qnreply.errorString(), str(qnreply.request().url()))
    else:
        return Reply(qnreply)


# --------------------------------------------------------------------------------------
# Replies & Errors
class URLError(urllib.error.URLError):
    pass

class ReplyHeadersProxy:
    def __init__(self, qnreply):
        print("headers for %s: %s" % (qnreply, self))
        self._qnreply = qnreply
    
    def __contains__(self, hdr):
        return self._qnreply.hasRawHeader(hdr.encode("utf-8"))
    
    def keys(self):
        return tuple(ba.toBytes().decode("utf-8") for ba in self._qnreply.rawHeaderList())
    
    def __getitem__(self, key):
        if key in self:
            return self._qnreply.rawHeader(key).toBytes().decode("utf-8")
        else:
            raise KeyError(key)
    
    def get(self, key, fallback='__THISISADEFAULTPLACEHOLDER_'):
        print("header %s for %s" % (key, self))
        if key in self:
            print("    found: %r" % self._qnreply.rawHeader(key.encode("utf-8")))
            return self._qnreply.rawHeader(key.encode("utf-8")).toBytes().decode("utf-8")
        elif fallback == '__THISISADEFAULTPLACEHOLDER_':
            raise KeyError(key)
        else:
            return fallback


class ReplyCommon(urllib.response.addinfourl):
    """
    Common Reply Base-Class
    Should be compatible with HTTPError and HTTPResponse
    """
    def __init__(self, qnreply):
        self._qnreply = qnreply

    def __del__(self):
        if not self.isclosed():
            self._qnreply.deleteLater()

    def __iter__(self):
        return self.readlines()

    def __repr__(self):
        if self._qnreply is None:
            return '<%s (closed) at %s>' % (self.__class__.__name__, id(self))
        else:
            return '<%s for "%s" at 0x%X>' % (self.__class__.__name__, self.geturl(), id(self))

    # Properties
    @property
    def fp(self):
        """ the standard addinfourl is a light wrapper. we're a heavy one ;) """
        if self.isclosed():
            return None
        else:
            return self

    @property
    def code(self):
        """ The HTTP status code """
        self._check_closed()
        return self._qnreply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute).toInt()

    status = code

    @property
    def headers(self):
        """ The HTTP Headers """
        self._check_closed()
        return ReplyHeadersProxy(self._qnreply)
        #return {x: self._qnreply.rawHeader(x) for x in self._qnreply.rawHeaderList()}

    @property
    def url(self):
        """ The original URL """
        self._check_closed()
        return str(self._qnreply.request().url())

    @property
    def reason(self):
        """ The HTTP status """
        return ""

    msg = reason

    # Getters
    def getcode(self):
        return self.code

    def getheaders(self):
        self._check_closed()
        return [(x, self._qnreply.rawHeader(x)) for x in self._qnreply.rawHeaderList()]

    def getheader(self, hdr):
        self._check_closed()
        return self._qnreply.rawHeader(hdr)

    def geturl(self):
        return self.url

    def readable(self):
        self._check_closed()
        return self._qnreply.isReadable()

    def writable(self):
        self._check_closed()
        return self._qnreply.isWritable()

    def seekable(self):
        self._check_closed()
        return self._qnreply.isSeekable()

    # Stuff
    def fileno(self):
        """ Don't have no fileno """
        return None

    def close(self):
        """ Close the reply """
        if not self.isclosed():
            self._qnreply.close()
            self._qnreply.deleteLater()
            self._qnreply = None

    def isclosed(self):
        return self._qnreply is None

    def _check_closed(self):
        if self.isclosed():
            raise io.UnsupportedOperation("Trying to operate on closed file")
    
    # Read from reply
    def read(self, size=None):
        """ Read """
        self._check_closed()
        if size is None:
            return self._qnreply.readAll().toBytes()
        else:
            return self._qnreply.read(size).toBytes()

    def readline(self, size=0):
        """ Read one line """
        self._check_closed()
        return self._qnreply.readLine(size).toBytes()

    def readlines(self):
        """ Iterate over all lines """
        self._check_closed()
        while True:
            line = self.readline()
            if line:
                yield line
            else:
                break

    def readinto(self, *a, **b):
        #TODO: Do it!
        raise io.UnsupportedOperation("Not Implemented")


class HTTPError(urllib.error.HTTPError, URLError, ReplyCommon):
    __init__ = ReplyCommon.__init__

    hdrs = property(ReplyCommon.info)
    filename = property(ReplyCommon.geturl)


class Reply(ReplyCommon):
    pass
