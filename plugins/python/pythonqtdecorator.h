#ifndef PYTHONQTDECORATOR_H
#define PYTHONQTDECORATOR_H

#include <QObject>
#include <PythonQt.h>

#include <QtCore/QUrl>

class QNetworkAccessManager;

class PythonQtDecorator : public QObject
{
    Q_OBJECT
public slots:
    int __len__(QByteArray *self);
    PyObject *toBytes(QByteArray *self);
    PyObject *retrieve(QNetworkAccessManager *self, QString url);
};

#endif // PYTHONQTDECORATOR_H
