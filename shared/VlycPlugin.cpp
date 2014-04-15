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

Plugin::Plugin() :
    metaData(nullptr)
{}

Plugin::~Plugin()
{}

void Plugin::init(InitEvent &init)
{
    metaData = init.pluginMetaData;
}

QString Plugin::name() const
{
    if (metaData && metaData->contains("Name"))
        return metaData->value("Name").toString();
    else
        return id();
}

QString Plugin::author() const
{
    if (metaData && metaData->contains("Author"))
        return metaData->value("Author").toString();
    else
        return QStringLiteral("Anonymous");
}

QString Plugin::version() const
{
    if (metaData && metaData->contains("Version"))
        return metaData->value("Version").toString();
    else
        return QStringLiteral("r0");
}

}
