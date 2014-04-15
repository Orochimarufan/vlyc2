/* Copyright 2013 MultiMC Contributors
 *
 * Authors: Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ------------------------------------------------------------------------
 * 2013-12-13 Commandline Parser Rewrite (Orochimarufan)
 */

#pragma once

#include "util/cmdparser.h"

namespace CmdParser {

enum class DefinitionType : int
{
    None,
    Switch,
    Option,
    Positional
};

struct ParameterDefinition
{
    DefinitionType type;
    QString name;

    QStringList aliases;
    QList<QChar> flags;

    QString meta;
    QString desc;

    QVariant defaultValue;
};

class ParserPrivate
{
    friend class Parser;

    FlagStyle m_flagStyle;
    ArgumentStyle m_argStyle;

    QList<ParameterDefinition *> m_definitions;
    QHash<QString, ParameterDefinition *> m_nameLookup;

    QList<ParameterDefinition *> m_positionals;
    QList<ParameterDefinition *> m_options;

    QHash<QString, ParameterDefinition *> m_longLookup;
    QHash<QChar, ParameterDefinition *> m_flagLookup;

    // Methods
    ParserPrivate(FlagStyle flagStyle, ArgumentStyle argStyle);

    ParameterDefinition *addDefinition(DefinitionType type, QString name, QVariant def);
    ParameterDefinition *lookup(QString name);

    void getPrefix(QString &opt, QString &flag);

    QHash<QString, QVariant> parse(QStringList argv);

    void clear();
    ~ParserPrivate();
};

}

