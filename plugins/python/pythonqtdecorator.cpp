#include "pythonqtdecorator.h"

int PythonQtDecorator::__len__(QByteArray *self)
{
    return self->size();
}

PyObject *PythonQtDecorator::toBytes(QByteArray *self)
{
    return PyBytes_FromStringAndSize(self->constData(), self->size());
}
