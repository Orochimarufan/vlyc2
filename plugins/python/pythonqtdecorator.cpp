#include "pythonqtdecorator.h"

#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

int PythonQtDecorator::__len__(QByteArray *self)
{
    return self->size();
}

PyObject *PythonQtDecorator::toBytes(QByteArray *self)
{
    return PyBytes_FromStringAndSize(self->constData(), self->size());
}

PyObject *PythonQtDecorator::retrieve(QNetworkAccessManager *self, QString url)
{
    //qDebug("QNAM %p retrieve %s", self, qPrintable(url));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply *reply;
    QEventLoop loop;
    while (true)
    {
        // Request
        reply = self->get(request);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        //connect(reply, (void (QNetworkReply::*)(QNetworkReply::NetworkError))&QNetworkReply::error, &loop, &QEventLoop::quit);
        loop.exec();
        if (reply->error() != QNetworkReply::NoError)
        {
            //TODO: PyErr_*, PythonQt seems to PyErr_Clear() it right away and return None?
            PyErr_SetString(PyExc_IOError, qPrintable(reply->errorString()));
            reply->deleteLater();
            return NULL;
        }
        // Redirects
        // thanks peterix :D
        QVariant redirLocation = reply->header(QNetworkRequest::LocationHeader);
        if (!redirLocation.isValid())
            break;
        //qDebug("QNAM %p redirect %s", self, qPrintable(redirLocation.toString()));
        request.setUrl(redirLocation.toUrl());
        reply->deleteLater();
    }
    // Reply
#if 0
    qDebug("Reply: %s %i", qPrintable(reply->url().toString()), reply->isFinished());
    foreach(QNetworkReply::RawHeaderPair header, reply->rawHeaderPairs())
        qDebug("HDR: %s: %s", qPrintable(header.first), qPrintable(header.second));
#endif
    QByteArray barr = reply->readAll();
    reply->deleteLater();
    return toBytes(&barr);
}
