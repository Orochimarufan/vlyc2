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

#include "Promise_p.h"

namespace Vlyc {
namespace Result {

QEvent::Type const PromiseEvent::TYPE = static_cast<QEvent::Type>(QEvent::registerEventType());

PromiseEvent::PromiseEvent(Promise *p, Type t, const QString &message, int current, int total) :
    QEvent(TYPE),
    mp_promise(p),
    m_type(t),
    m_message(message),
    m_current(current),
    m_total(total)
{
}

// Create
PromiseEvent *PromiseEvent::started(Promise *p)
{
    return new PromiseEvent(p, Type::Started, QString::null, 0, 1);
}

PromiseEvent *PromiseEvent::finished(Promise *p)
{
    return new PromiseEvent(p, Type::Finished, QString::null, 1, 1);
}

PromiseEvent *PromiseEvent::error(Promise *p, const QString &message)
{
    return new PromiseEvent(p, Type::Error, message, 0, 0);
}

PromiseEvent *PromiseEvent::progress(Promise *p, const int &current, const int &total, const QString &message)
{
    return new PromiseEvent(p, Type::Progress, message, current, total);
}


// Access
PromisePtr PromiseEvent::promise() const
{
    return mp_promise;
}

PromiseEvent::Type PromiseEvent::type() const
{
    return m_type;
}

QString PromiseEvent::message() const
{
    return m_message;
}

int PromiseEvent::currentProgress() const
{
    return m_current;
}

int PromiseEvent::totalProgress() const
{
    return m_total;
}

}
}
