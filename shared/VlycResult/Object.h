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

#pragma once

#include <functional>
#include <exception>

#include <QtCore/QVariant>
#include <QtCore/QHash>

#include "VlycConfig.h"
#include "Result.h"


namespace Vlyc {
namespace Result {

/**
 * @brief The Result Object
 * A result object has properties and methods.
 * It can also inherit them from a parent object.
 * Note that when calling methods, the this argument is always
 *  the object the method was called on, not necessarily the one it was defined on.
 * This only makes a difference for .setProperty and .setMethod calls though.
 */
class VLYC_EXPORT Object : public Result
{
public:
    using Ptr = Pointer<Object>;

    Object() = default;
    Object(std::initializer_list<std::pair<QString, QVariant>> il):
        properties(il) {}

    explicit Object(const Ptr &parent):
        parent(parent) {}
    explicit Object(const Ptr &parent, std::initializer_list<std::pair<QString, QVariant>> il):
        parent(parent), properties(il) {}

    // Property Interface
    /**
     * @brief Set a property
     * @param name The property name
     * @param value The new value
     */
    inline void setProperty(const QString &name, const QVariant &value)
    {
        properties.insert(name, value);
    }

    /**
     * @brief Check if a property exists
     * @param name The property name
     * @return
     */
    bool hasProperty(const QString &name) const;

    /**
     * @brief Retrieve a property
     * @param name The property name
     * @return The property's value
     */
    QVariant getProperty(const QString &name) const;
    QVariant getProperty(const QString &name, const QVariant &default_) const;

    /**
     * @brief Retrieve and cast a property
     */
    template <typename T = QVariant>
    inline T property(const QString &name) const
    {
        return qvariant_cast<T>(getProperty(name));
    }

    template <typename T = QVariant>
    inline T property(const QString &name, const T &default_) const
    {
        return qvariant_cast<T>(getProperty(name, default_));
    }

    /**
     * @brief Remove a property
     * @param name The property name
     * @return true if a property named \c name was removed
     */
    inline bool delProperty(const QString &name)
    {
        return properties.remove(name) > 0;
    }

    // Callback interface
    /**
     * @brief The type of a callback function
     * Callbacks are VariantArgs functions with the additional instance argument.
     */
    typedef std::function<QVariant(Ptr, const QVariantList&)> Callback;

    /**
     * @brief Set the callback for a Result method
     * @param name The method name
     * @param cb The callback function
     */
    inline void setMethod(const QString &name, const Callback &cb)
    {
        methods.insert(name, cb);
    }

    /**
     * @brief Check if a method is provided with this Media
     * @param name The method name
     * @return
     */
    bool hasMethod(const QString &name) const;

    /**
     * @brief Call a method on this Result
     * @param method The method name
     * @param arguments The arguments
     * @return The call's return value
     */
    QVariant call(const QString &method, const QVariantList &arguments);

    /**
     * @brief Call a method on this Result
     * @param method The method name
     * @param args The arguments
     * @return The call's return value
     * This is an overloaded version of \c call.
     * It abstracts away the VariantArgs interface.
     */
    template <typename Ret = QVariant, typename... Args>
    inline Ret call(const QString &method, Args... args)
    {
        return qvariant_cast<Ret>(call(method, QVariantList{args...}));
    }

    // Utility Methods
    QString objectName() const;
    QByteArray stringyfy() const;

    // Property getter shortcuts
    inline QString type()
    {
        return getProperty("type").toString();
    }

protected:
    Ptr parent;
    QHash<QString, QVariant> properties;
    QHash<QString, Callback> methods;

    QVariant call_(const Ptr &inst, const QString &method, const QVariantList &arguments);

    explicit Object(const Ptr &parent, const QHash<QString, QVariant> &p, const QHash<QString, Callback> &m):
        parent(parent), properties(p), methods(m) {}
};

using ObjectPtr = Pointer<Object>;

class VLYC_EXPORT MethodNotFoundError : std::exception
{
    QString m_name;
    ObjectPtr m_object;
    QByteArray message;

public:
    MethodNotFoundError(ObjectPtr o, QString method_name):
        m_name(method_name), m_object(o)
    {
        message = QStringLiteral("%1 has no method %2").arg(o->objectName(), method_name).toUtf8();
    }

    const char *what() const noexcept
    {
        return message.constData();
    }

    ObjectPtr object() const
    {
        return m_object;
    }

    QString method_name() const
    {
        return m_name;
    }
};

}
}
