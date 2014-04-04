#include "pythonqtdecorator.h"

#include <PythonQt/PythonQtConversion.h>
#include <QtCore/QEventLoop>
#include <QtCore/QBuffer>
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

inline bool redirect(QNetworkReply *reply, QNetworkRequest &request)
{
    // Check for errors
    if (reply->error() != QNetworkReply::NoError)
        return false;
    // Get redirection URL
    //QUrl redirectTo = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().resolved(request.url());
    QUrl redirectTo = reply->header(QNetworkRequest::LocationHeader).toUrl();
    if (redirectTo.isEmpty() || request.url() == redirectTo)
        return false;
    qDebug("Redirect: %s", qPrintable(redirectTo.toString()));
    // Redirect
    request.setUrl(redirectTo);
    reply->close();
    reply->deleteLater();
    return true;
}

QNetworkReply *PythonQtDecorator::syncGet(QNetworkAccessManager *self, QNetworkRequest request)
{
    QNetworkReply *reply;
    QEventLoop loop;

    do
    {
        // Request
        reply = self->get(request);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }
    while (redirect(reply, request));

    return reply;
}

QNetworkReply *PythonQtDecorator::syncRequest(QNetworkAccessManager *self, QNetworkRequest request,
                                                     QByteArray verb, QByteArray data, bool followRedir)
{
    QNetworkReply *reply;
    QEventLoop loop;
    QBuffer buffer(&data);

    // Request
    reply = self->sendCustomRequest(request, verb, &buffer);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    while (followRedir && redirect(reply, request))
    {
        // Request
        reply = self->sendCustomRequest(request, verb, &buffer);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

    return reply;
}

PyObject *PythonQtDecorator::retrieve(QNetworkAccessManager *self, QString url)
{
    //qDebug("QNAM %p retrieve %s", self, qPrintable(url));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Connection", "keep-alive");

    QNetworkReply *reply = syncGet(self, request);

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning("Network Error: %s", qPrintable(reply->errorString()));

        QByteArray reason = reply->errorString().toUtf8();
        char *creason = (char*)malloc(reason.size() + 1);
        strcpy(creason, reason.constData());
        PyErr_SetString(PyExc_IOError, creason);

        reply->deleteLater();
        return NULL;
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
