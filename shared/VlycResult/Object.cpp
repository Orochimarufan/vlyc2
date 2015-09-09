/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2014 Taeyeon Mori <orochimarufan.x3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "Object.h"

#include <QtCore/QJsonDocument>


namespace Vlyc {
namespace Result {

// Properties
bool Object::hasProperty(const QString &name) const
{
    return properties.contains(name) || (parent.isValid() && parent->hasProperty(name));
}

QVariant Object::getProperty(const QString &name) const
{
    if (properties.contains(name))
        return properties.value(name);
    else if (parent.isValid())
        return parent->getProperty(name);
    else
        return QVariant();
}

QVariant Object::getProperty(const QString &name, const QVariant &default_) const
{
    if (properties.contains(name))
        return properties.value(name);
    else if (parent.isValid())
        return parent->getProperty(name, default_);
    else
        return default_;
}

// Methods
bool Object::hasMethod(const QString &name) const
{
    return methods.contains(name) || (parent.isValid() && parent->hasMethod(name));
}

QVariant Object::call(const QString &method, const QVariantList &arguments)
{
    if (methods.contains(method))
        return methods.value(method)(this, arguments);
    else if (parent.isValid())
        return parent->call_(this, method, arguments);
    else
        throw MethodNotFoundError(this, method);
}

QVariant Object::call_(const ObjectPtr &inst, const QString &method, const QVariantList &arguments)
{
    if (methods.contains(method))
        return methods.value(method)(inst, arguments);
    else if (parent.isValid())
        return parent->call_(inst, method, arguments);
    else
        throw MethodNotFoundError(inst, method);
}

// Utility
QString Object::objectName() const
{
    if (properties.contains("__name__"))
        return QStringLiteral("<Object [%1]>").arg(properties.value("__name__").toString());
    else
    {
        QStringList build;
        build << "<Object at 0x" << QString::number((qint64)this, 16);
        if (parent.isValid())
            build << " of " << parent->objectName();
        build << ">";
        return build.join("");
    }
}

QByteArray Object::stringyfy() const
{
    return QJsonDocument::fromVariant(properties).toJson();
}

}
}
