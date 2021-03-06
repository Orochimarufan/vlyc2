/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
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

#include "VlycPlugin.h"

#include <QJsonDocument>

namespace Vlyc {

Plugin::Plugin()
{}

Plugin::~Plugin()
{}

void Plugin::_init(InitEvent &init)
{
    m_initializer = init;
    this->init(init);
}

void Plugin::init(InitEvent &init)
{
    Q_UNUSED(init);
}

QString Plugin::name() const
{
    if (m_initializer.metadata->contains("Name"))
        return m_initializer.metadata->value("Name").toString();
    else
        return id();
}

QString Plugin::author() const
{
    if (m_initializer.metadata->contains("Author"))
        return m_initializer.metadata->value("Author").toString();
    else
        return QStringLiteral("Anon");
}

QString Plugin::version() const
{
    if (m_initializer.metadata->contains("Version"))
        return m_initializer.metadata->value("Version").toString();
    else
        return QStringLiteral("v0");
}

PluginManagerInterface *Plugin::plugins()
{
    return m_initializer.plugin_manager;
}

PluginInterface *Plugin::interface()
{
    return m_initializer.interface;
}

QJsonObject *Plugin::metadata() const
{
    return m_initializer.metadata;
}

}
