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

#include <QtCore/QCoreApplication>

#include "Promise.h"
#include "Promise_p.h"

namespace Vlyc {
namespace Result {

#define LOCK() std::lock_guard<std::mutex> lock(m_mutex)

// Default constructor
Promise::Promise() :
    m_state(State::NoState),
    m_hasResult(false),
    mp_listener(nullptr)
{
}

Promise::~Promise()
{
    if (m_state == State::Running)
        qWarning("Vlyc::Result::Promise: Destroying running promise!");
}

void Promise::setListener(QObject *listener)
{
    if (mp_listener)
        qWarning("Vlyc::Result::Promise: Re-Setting Listener!");
    mp_listener = listener;
}

// Public interface
ResultPtr Promise::result() const
{
    LOCK();
    return mp_result;
}

Promise::State Promise::state() const
{
    LOCK();
    return m_state;
}

bool Promise::isFinished() const
{
    LOCK();
    return m_state == State::Finished || m_state == State::Canceled;
}

void Promise::run()
{
    {
        LOCK();
        if (m_state != State::NoState)
            return;

        m_state = State::Running;

        if (!mp_listener)
            qWarning("Vlyc::Result::Promise: Running promise without listener!");
    }

    try {
        work();
    } catch(const std::exception &e) {
        error(e.what());
    } catch(const std::string &e) {
        error(QString::fromStdString(e));
    } catch(const char *e) {
        error(e);
    } catch(const QString &e) {
        error(e);
    } catch(...) {
        error("Unknown error occured!");
    }
}

// Promise Interface

#define POST_EVENT1(what) if (mp_listener) QCoreApplication::instance()->postEvent(mp_listener, PromiseEvent:: what (this))
#define POST_EVENT2(what, ...) if (mp_listener) QCoreApplication::instance()->postEvent(mp_listener, PromiseEvent:: what (this, __VA_ARGS__))

void Promise::progress(int current, int total, const QString &message)
{
    POST_EVENT2(progress, current, total, message);
}

void Promise::fulfill(ResultPtr result)
{
    LOCK();
    if (!m_hasResult)
    {
        mp_result = result;
        m_hasResult = true;
        m_state = State::Finished;
        POST_EVENT1(finished);
    }
    else
        qWarning("Vlyc::Result::Promise: tried to fulfill a fulfilled promise!");
}

void Promise::error(QString message)
{
    LOCK();
    if (!m_hasResult)
    {
        m_error = message;
        m_hasResult = true;
        m_state = State::Canceled;
        POST_EVENT2(error, message);
    }
    else
        qWarning("Vlyc::Result::Promise: tried to error() a fulfilled promise!");
}

// Lambda Promise
LambdaPromise::LambdaPromise(std::function<void (Proxy *)> f) :
    Promise(), mf_work(f), m_proxy(this)
{
}

void LambdaPromise::work()
{
    mf_work(&m_proxy);
}

} // namespace Result
} // namespace Vlyc
