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

#include "SleepPlugin.h"

#include <time.h>
#include <math.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QAction>

#include <QtVlc/VlcMediaPlayer.h>

#include "../../src/vlyc.h"
#include "../../src/logic/VlycPlayer.h"


SleepPlugin::SleepPlugin()
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, [this](){ ((VlycApp*)qApp->property("vlyc").value<QObject*>())->player()->player().pause(); });
}

void SleepPlugin::set(unsigned int mins)
{
    m_started = time(nullptr);
    m_timer.start(mins * 60000);
}

unsigned int SleepPlugin::getRemainingMins() const
{
    int v = (int)ceil(m_timer.interval() / 1000 - (time(nullptr) - m_started)) / 60;
    return v > 0 ? v : 0;
}

void SleepPlugin::clear()
{
    m_timer.stop();
}

void SleepPlugin::onActionTriggered()
{
    bool ok;
    int iv = QInputDialog::getInt((QWidget*)qApp->property("mainwindow").value<QObject*>(), "Sleep Timer", "Sleep after after this many minutes:\nUse 0 to disable.", m_timer.isActive() ? getRemainingMins() : 0, 0, 1440, 10, &ok);
    if (ok)
    {
        if (iv == 0)
            clear();
        else
            set(iv);
    }
}

QAction *SleepPlugin::toolMenuAction()
{
    QAction *a = new QAction(tr("Set Sleep Timer"), this);
    connect(a, &QAction::triggered, this, &SleepPlugin::onActionTriggered);
    return a;
}
