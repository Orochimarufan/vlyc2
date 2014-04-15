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

#include "util/cmdparser.h"
#include "util/cmdparser_p.h"

/**
 * @file util/cmdparser.cpp
 */

namespace CmdParser {

// Required
QVariant const Required = qVariantFromValue<void*>(NULL);

// ------------------ commandline splitter ------------------
QStringList splitArgs(QString args)
{
	QStringList argv;
	QString current;
	bool escape = false;
	QChar inquotes;

	for (int i = 0; i < args.length(); i++)
	{
		QChar cchar = args.at(i);

		// \ escaped
		if (escape)
		{
			current += cchar;
			escape = false;
			// in "quotes"
		}
		else if (!inquotes.isNull())
		{
			if (cchar == 0x5C)
				escape = true;
			else if (cchar == inquotes)
				inquotes = 0;
			else
				current += cchar;
			// otherwise
		}
		else
		{
			if (cchar == 0x20)
			{
				if (!current.isEmpty())
				{
					argv << current;
					current.clear();
				}
			}
			else if (cchar == 0x22 || cchar == 0x27)
				inquotes = cchar;
			else
				current += cchar;
		}
	}
	if (!current.isEmpty())
		argv << current;
	return argv;
}

// ------------------ Required ------------------
static inline bool isRequired(ParameterDefinition *param)
{
	// TODO: TEST !!!
	return (QMetaType::Type)param->defaultValue.type() == QMetaType::VoidStar
			&& param->defaultValue.value<void*>() == NULL;
}

// ------------------ ParserPrivate ------------------
ParserPrivate::ParserPrivate(FlagStyle flagStyle, ArgumentStyle argStyle)
{
	m_flagStyle = flagStyle;
	m_argStyle = argStyle;
}

// Definitions
ParameterDefinition *ParserPrivate::addDefinition(DefinitionType type, QString name, QVariant def)
{
	if (m_longLookup.contains(name))
		throw "Parameter name in use.";

	ParameterDefinition *param = new ParameterDefinition;
	param->type = type;
	param->name = name;
	param->meta = QStringLiteral("<%1>").arg(name);
	param->defaultValue = def;

	m_definitions << param;
	m_nameLookup.insert(name, param);

	if (type == DefinitionType::Positional)
		m_positionals << param;
	else if (type == DefinitionType::Switch || type == DefinitionType::Option)
	{
		m_options << param;
		m_longLookup.insert(name, param);
	}

	return param;
}

ParameterDefinition *ParserPrivate::lookup(QString name)
{
	if (!m_nameLookup.contains(name))
		throw "Parameter name not in use.";

	return m_nameLookup[name];
}

void ParserPrivate::clear()
{
	m_nameLookup.clear();
	m_positionals.clear();
	m_options.clear();
	m_longLookup.clear();
	m_flagLookup.clear();

	for (QMutableListIterator<ParameterDefinition *> it(m_definitions); it.hasNext();)
	{
		ParameterDefinition *param = it.next();
		it.remove();
		delete param;
	}
}

// Flag and Long option prefix
void ParserPrivate::getPrefix(QString &opt, QString &flag)
{
	if (m_flagStyle == FlagStyle::Windows)
		opt = flag = "/";
	else if (m_flagStyle == FlagStyle::Unix)
		opt = flag = "-";
	// else if (m_flagStyle == FlagStyle::GNU)
	else
	{
		opt = "--";
		flag = "-";
	}
}

// Parsing
QHash<QString, QVariant> ParserPrivate::parse(QStringList argv)
{
	QHash<QString, QVariant> map;

	QStringListIterator it(argv);
	QString programName = it.next();

	QString optionPrefix;
	QString flagPrefix;
	QListIterator<ParameterDefinition *> positionals(m_positionals);
	QStringList expecting;

	getPrefix(optionPrefix, flagPrefix);

	while (it.hasNext())
	{
		QString arg = it.next();

		if (!expecting.isEmpty())
		// we were expecting an argument
		{
			QString name = expecting.first();

			if (map.contains(name))
				throw ParsingError(
					QString("Option %2%1 was given multiple times").arg(name, optionPrefix));

			map[name] = QVariant(arg);

			expecting.removeFirst();
			continue;
		}

		if (arg.startsWith(optionPrefix))
		// we have an option
		{
			// qDebug("Found option %s", qPrintable(arg));

			QString name = arg.mid(optionPrefix.length());
			QString equals;

			if ((m_argStyle == ArgumentStyle::Equals ||
				 m_argStyle == ArgumentStyle::SpaceAndEquals) &&
				name.contains("="))
			{
				int i = name.indexOf("=");
				equals = name.mid(i + 1);
				name = name.left(i);
			}

			if (m_longLookup.contains(name))
			{
				ParameterDefinition *param = m_longLookup[name];

				if (map.contains(param->name))
					throw ParsingError(QString("Option %2%1 was given multiple times")
										   .arg(param->name, optionPrefix));

				if (param->type == DefinitionType::Switch)
					map[param->name] = !param->defaultValue.toBool();
				else // if (param->type == DefinitionType::Option)
				{
					if (m_argStyle == ArgumentStyle::Space)
						expecting.append(param->name);
					else if (!equals.isNull())
						map[param->name] = equals;
					else if (m_argStyle == ArgumentStyle::SpaceAndEquals)
						expecting.append(param->name);
					else
						throw ParsingError(QString("Option %2%1 reqires an argument.")
											   .arg(name, optionPrefix));
				}

				continue;
			}

			// We need to fall through if the prefixes match
			if (optionPrefix != flagPrefix)
				throw ParsingError(QString("Unknown Option %2%1").arg(name, optionPrefix));
		}

		if (arg.startsWith(flagPrefix))
		// we have (a) flag(s)
		{
			// qDebug("Found flags %s", qPrintable(arg));

			QString flags = arg.mid(flagPrefix.length());
			QString equals;

			if ((m_argStyle == ArgumentStyle::Equals ||
				 m_argStyle == ArgumentStyle::SpaceAndEquals) &&
				flags.contains("="))
			{
				int i = flags.indexOf("=");
				equals = flags.mid(i + 1);
				flags = flags.left(i);
			}

			for (int i = 0; i < flags.length(); i++)
			{
				QChar flag = flags.at(i);

				if (!m_flagLookup.contains(flag))
					throw ParsingError(QString("Unknown flag %2%1").arg(flag, flagPrefix));

				ParameterDefinition *param = m_flagLookup[flag];

				if (map.contains(param->name))
					throw ParsingError(QString("Option %2%1 was given multiple times")
										   .arg(param->name, optionPrefix));

				if (param->type == DefinitionType::Switch)
					map[param->name] = !param->defaultValue.toBool();
				else // if (param->type == DefinitionType::Option)
				{
					if (m_argStyle == ArgumentStyle::Space)
						expecting.append(param->name);
					else if (!equals.isNull())
						if (i == flags.length() - 1)
							map[param->name] = equals;
						else
							throw ParsingError(QString("Flag %4%2 of Argument-requiring Option "
													   "%1 not last flag in %4%3")
												   .arg(param->name, flag, flags, flagPrefix));
					else if (m_argStyle == ArgumentStyle::SpaceAndEquals)
						expecting.append(param->name);
					else
						throw ParsingError(QString("Option %1 reqires an argument. (flag %3%2)")
											   .arg(param->name, flag, flagPrefix));
				}
			}

			continue;
		}

		// must be a positional argument
		if (!positionals.hasNext())
			throw ParsingError(QString("Too many positional arguments: '%1'").arg(arg));

		ParameterDefinition *param = positionals.next();

		map[param->name] = arg;
	}

	// check if we're missing something
	if (!expecting.isEmpty())
		throw ParsingError(QString("Was still expecting arguments for %2%1").arg(
			expecting.join(QString(", ") + optionPrefix), optionPrefix));

	// fill out gaps
	for (QListIterator<ParameterDefinition *> it(m_definitions); it.hasNext();)
	{
		ParameterDefinition *param = it.next();
		if (!map.contains(param->name))
		{
			if (isRequired(param))
				throw ParsingError(
					QStringLiteral("Missing mandatory argument '%1'").arg(param->name));
			else
				map[param->name] = param->defaultValue;
		}
	}

	return map;
}

ParserPrivate::~ParserPrivate()
{
	clear();
}

// ------------------ Parser ------------------
Parser::Parser(FlagStyle flagStyle, ArgumentStyle argStyle)
{
	d_ptr = new ParserPrivate(flagStyle, argStyle);
}

// ---------- Parameter Style ----------
void Parser::setArgumentStyle(ArgumentStyle style)
{
	d_ptr->m_argStyle = style;
}
ArgumentStyle Parser::argumentStyle()
{
	return d_ptr->m_argStyle;
}

void Parser::setFlagStyle(FlagStyle style)
{
	d_ptr->m_flagStyle = style;
}
FlagStyle Parser::flagStyle()
{
	return d_ptr->m_flagStyle;
}

// ---------- Defining parameters ----------
void Parser::newSwitch(QString name, bool direction)
{
	d_ptr->addDefinition(DefinitionType::Switch, name, direction);
}

void Parser::newOption(QString name, QVariant def)
{
	d_ptr->addDefinition(DefinitionType::Option, name, def);
}

void Parser::newArgument(QString name, QVariant def)
{
	d_ptr->addDefinition(DefinitionType::Positional, name, def);
}

// ---------- Modifying Parameters ----------
void Parser::addDocumentation(QString name, QString doc, QString metavar)
{
	ParameterDefinition *param = d_ptr->lookup(name);

	param->desc = doc;

	if (!metavar.isNull())
		param->meta = metavar;
}

void Parser::addFlag(QString name, QChar flag)
{
	if (d_ptr->m_flagLookup.contains(flag))
		throw "Short option already in use.";

	ParameterDefinition *param = d_ptr->lookup(name);

	param->flags << flag;
	d_ptr->m_flagLookup.insert(flag, param);
}

void Parser::addAlias(QString name, QString alias)
{
	if (d_ptr->m_longLookup.contains(alias))
		throw "Long option already in use.";

	ParameterDefinition *param = d_ptr->lookup(name);

	param->aliases << alias;
	d_ptr->m_longLookup.insert(alias, param);
}

// ---------- Generating Help messages ----------
QString Parser::compileHelp(QString progName, int helpIndent, bool useFlags)
{
	QStringList help;
	help << compileUsage(progName, useFlags) << "\r\n";

	// positionals
	if (!d_ptr->m_positionals.isEmpty())
	{
		help << "\r\n";
		help << "Positional arguments:\r\n";
		QListIterator<ParameterDefinition *> it2(d_ptr->m_positionals);
		while (it2.hasNext())
		{
			ParameterDefinition *param = it2.next();
			help << "  " << param->meta;
			help << " " << QString(helpIndent - param->meta.length() - 1, ' ');
			help << param->desc << "\r\n";
		}
	}

	// Options
	if (!d_ptr->m_options.isEmpty())
	{
		help << "\r\n";
		QString optPrefix, flagPrefix;
		d_ptr->getPrefix(optPrefix, flagPrefix);

		help << "Options & Switches:\r\n";
		QListIterator<ParameterDefinition *> it(d_ptr->m_options);
		while (it.hasNext())
		{
			ParameterDefinition *param = it.next();

			help << "  ";

			int nameLength = optPrefix.length() + param->name.length();

			for (QListIterator<QChar> it3(param->flags); it3.hasNext();)
			{
				nameLength += 3 + flagPrefix.length();
				help << flagPrefix << it3.next() << ", ";
			}
			for (QListIterator<QString> it3(param->aliases); it3.hasNext();)
			{
				QString alias = it3.next();
				nameLength += 2 + optPrefix.length() + alias.length();
				help << optPrefix << alias;
			}

			help << optPrefix << param->name;

			if (param->type == DefinitionType::Option)
			{
				QString arg = QString("%1%2").arg(
					((d_ptr->m_argStyle == ArgumentStyle::Equals) ? "=" : " "), param->meta);
				nameLength += arg.length();
				help << arg;
			}

			help << " " << QString(helpIndent - nameLength - 1, ' ');
			help << param->desc << "\r\n";
		}
	}

	return help.join("");
}

QString Parser::compileUsage(QString progName, bool useFlags)
{
	QStringList usage;
	usage << "Usage: " << progName;

	QString optPrefix, flagPrefix;
	d_ptr->getPrefix(optPrefix, flagPrefix);

	// options
	for (QListIterator<ParameterDefinition *> it(d_ptr->m_options); it.hasNext();)
	{
		ParameterDefinition *param = it.next();
		bool required = isRequired(param);
		if (!required) usage << " [";
		if (!param->flags.isEmpty() && useFlags)
			usage << flagPrefix << param->flags[0];
		else
			usage << optPrefix << param->name;
		if (param->type == DefinitionType::Option)
			usage << ((d_ptr->m_argStyle == ArgumentStyle::Equals) ? "=" : " ") << param->meta;
		if (!required) usage << "]";
	}

	// arguments
	for (QListIterator<ParameterDefinition *> it(d_ptr->m_positionals); it.hasNext();)
	{
		ParameterDefinition *param = it.next();
		usage << " ";
		bool required = isRequired(param);
		usage << (required ? "<" : "[") << param->meta << (required ? ">" : "]");
	}

	return usage.join("");
}

// parsing
QHash<QString, QVariant> Parser::parse(QStringList argv)
{
	return d_ptr->parse(argv);
}

// clear defs
void Parser::clearDefinitions()
{
	d_ptr->clear();
}

// Destructor
Parser::~Parser()
{
	delete d_ptr;
}

// ParsingError
ParsingError::ParsingError(const QString &what) : std::runtime_error(what.toStdString())
{
}

}

