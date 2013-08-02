#ifndef PYTHONQTDECORATOR_H
#define PYTHONQTDECORATOR_H

#include <QObject>
#include <PythonQt/PythonQt.h>

class PythonQtDecorator : public QObject
{
    Q_OBJECT
public slots:
    int __len__(QByteArray *self);
    PyObject *toBytes(QByteArray *self);
};

#endif // PYTHONQTDECORATOR_H
