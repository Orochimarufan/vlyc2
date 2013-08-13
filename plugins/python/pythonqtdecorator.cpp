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
    QNetworkReply *reply = self->get(QNetworkRequest(QUrl(url)));
    QEventLoop loop;
    connect(reply, (void (QNetworkReply::*)(QNetworkReply::NetworkError))&QNetworkReply::error, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray barr = reply->readAll();
    return PyBytes_FromStringAndSize(barr.constData(), barr.size());
}
