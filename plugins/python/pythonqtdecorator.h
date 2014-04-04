#ifndef PYTHONQTDECORATOR_H
#define PYTHONQTDECORATOR_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <PythonQt/PythonQt.h>

class PythonQtDecorator : public QObject
{
    Q_OBJECT
public slots:
    int __len__(QByteArray *self);
    PyObject *toBytes(QByteArray *self);
    PyObject *retrieve(QNetworkAccessManager *self, QString url);
    QNetworkReply *syncGet(QNetworkAccessManager *self, QNetworkRequest req);
    QNetworkReply *syncRequest(QNetworkAccessManager *self, QNetworkRequest req,
                               QByteArray verb="GET", QByteArray data=QByteArray(),
                               bool followRedir=false);
};

#endif // PYTHONQTDECORATOR_H
