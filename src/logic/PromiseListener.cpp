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

#include <VlycAsync/PromiseEvent.h>

#include "PromiseListener.h"
#include "PlaylistNode.h"

using namespace Vlyc::Result;
using namespace Vlyc::Async;

PromiseListener::PromiseListener()
{
}

void PromiseListener::schedule(PlaylistNode *node)
{
    PromisePtr promise = node->result().cast<Promise>();
    if (mm_promises.find(promise) != mm_promises.end())
        return;
    mm_promises.insert(std::make_pair(promise, node));
    promise->setListener(this);
    promise->run();
}

void PromiseListener::del(PlaylistNode *node)
{
    auto it = mm_promises.find(node->result().cast<Promise>());
    if (it != mm_promises.end())
        mm_promises.insert(it, std::make_pair(it->first, nullptr));
}

bool PromiseListener::event(QEvent *e)
{
    if (e->type() == PromiseEvent::TYPE)
    {
        PromiseEvent *event = static_cast<PromiseEvent*>(e);
        auto it = mm_promises.find(event->promise());
        if (it == mm_promises.end())
        {
            qWarning("Got PromiseEvent for unknown promise at %p", event->promise().get());
            return false;
        }
        else if (!it->second)
            // Deleted
            return true;

        if (event->type() == PromiseEvent::Type::Finished)
        {
            PlaylistNode *node = it->second;
            node->replaceWith(it->first->result());
            mm_promises.erase(it);
            emit finished(node);
        }
        else if (event->type() == PromiseEvent::Type::Error)
        {
            qWarning("PromiseListener: Error! %s", qPrintable(event->message()));
            PlaylistNode *node = it->second;
            node->markFailed(event->message());
            mm_promises.erase(it);
            emit finished(node);
        }
        else
            qWarning("Unhandled PromiseEvent");

        return true;
    }
    else
        return QObject::event(e);
}
