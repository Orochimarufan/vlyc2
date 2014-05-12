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

#include <QtCore/QEvent>
#include <QtCore/QVariant>

#include <VlycResult/Promise.h>

namespace Vlyc {
namespace Async {

    typedef Vlyc::Result::Promise Promise;
    typedef Vlyc::Result::PromisePtr PromisePtr;

/**
 * @brief Events sent to the promise listener
 */
class PromiseEvent : public QEvent
{
public:
    static QEvent::Type const TYPE;

    enum class Type {
        Started,
        Finished,
        Error,
        Progress
    };

    // Create it
    static PromiseEvent *started(Promise *);
    static PromiseEvent *finished(Promise *);
    static PromiseEvent *error(Promise *, const QString &message);
    static PromiseEvent *progress(Promise *, const int &current, const int &total, const QString &message = QString::null);

    // Use it
    PromisePtr promise() const;
    Type type() const;
    QString message() const;
    int currentProgress() const;
    int totalProgress() const;

private:
    PromiseEvent(Promise *, Type t, const QString &message, int current, int total);

    PromisePtr mp_promise;
    Type m_type;
    QString m_message;
    int m_current;
    int m_total;
};

}
}
