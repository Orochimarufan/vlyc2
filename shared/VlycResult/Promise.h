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

#include "Result.h"

#include <QtCore/QRunnable>

#include <functional>
#include <mutex>

namespace Vlyc {
namespace Result {

/**
 * @brief The Promise class
 * Promises a result that has to be calculated first.
 * Promises may or may not be executed in a separate thread!
 */
class Promise : public Result, public QRunnable
{
public:
    enum class State {
        NoState,
        Running,
        Finished,
        Canceled
    };

    // Execute the promise
    void run() override;

    // Set the listener.
    void setListener(QObject *listener);

    // ----------------------------------------------------------------
    // Information about the promise
    /**
     * @brief result Query the result after the promise has been fullfilled
     * @return the result
     */
    ResultPtr result() const;

    /**
     * @brief state Query the state of the Promise's execution
     * @return the current state
     */
    State state() const;

    /**
     * @brief isFinished
     * @return
     */
    bool isFinished() const;

protected:
    // Default constructor
    Promise();

    /**
     * @brief progress Notify a progress change
     * @param current the current progress
     * @param total the total progress
     * @param progress message
     */
    void progress(int current, int total, const QString &message = QString::null);

    /**
     * @brief fulfill the promise
     * @param result the operation result
     * The worker is expected to exit after calling this!
     */
    void fulfill(ResultPtr result);

    /**
     * @brief error Abort execution due to an error.
     * @param message An informational error message
     */
    void error(QString message);

    /**
     * @brief work do the work.
     * Override this in subclasses.
     * Default implementation calls the function object.
     */
    virtual void work() = 0;

private:
    mutable std::mutex m_mutex;
    State m_state;
    ResultPtr mp_result;
    QString m_error;
    bool m_hasResult;
    QObject *mp_listener;

public: ~Promise();
};


class LambdaPromise : public Promise
{
public:
    class Proxy
    {
        friend class LambdaPromise;
        LambdaPromise *mp_promise;
        inline Proxy(LambdaPromise *promise)
            : mp_promise(promise) {}
    public:
        inline void progress(int current, int total, const QString &message = QString::null)
            { mp_promise->progress(current, total, message); }
        inline void fulfill(ResultPtr result)
            { mp_promise->fulfill(result); }
        inline void error(QString message)
            { mp_promise->error(message); }
    };

    LambdaPromise(std::function<void(Proxy*)> f);

private:
    void work() override;
    std::function<void(Proxy*)> mf_work;
    Proxy m_proxy;
};

typedef Pointer<Promise> PromisePtr;

} // namespace Result
} // namespace Vlyc
