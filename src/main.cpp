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

#include <QApplication>
#include <QNetworkProxy>

#include <iostream>

#include "mainwindow.h"
#include "vlyc.h"
#include "cmdparser.h"
#include "vlycbrowser.h"
#include "browser/networkaccessmanager.h"

#include <VlcInstance.h>


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    app.setApplicationName("vlyc2");
    app.setApplicationDisplayName("VLYC²");
    app.setApplicationVersion("1.9.0");
    app.setOrganizationName("Orochimarufan");
    app.setOrganizationDomain("oro.sodimm.me");
    app.setWindowIcon(QIcon(":/vlyc/res/vlyc2.png"));

    Commandline::Parser parser(Commandline::FlagStyle::GNU, Commandline::ArgumentStyle::SpaceAndEquals);

    parser.addArgument("mrl", false, QString());
    parser.addDocumentation("mrl", "The MRL to play after startup");

    parser.addOption("proxy", QString());
    parser.addDocumentation("proxy", "Set the HTTP proxy", "PROXY");

    parser.addSwitch("help", false);
    parser.addShortOpt("help", 'h');
    parser.addDocumentation("help", "Show this help");

    QHash<QString,QVariant> args = parser.parse(app.arguments());

    if (args["help"].toBool())
    {
        std::cout << qPrintable(parser.compileHelp(app.arguments()[0]));
        return 0;
    }

    if (!args["proxy"].toString().isNull())
    {
        QUrl proxy(args["proxy"].toString());
        if (proxy.scheme().toLower() != "http")
        {
            std::cerr << "Only HTTP proxies supported as of right now.";
            args["proxy"] = QString();
        }
        else
        {
            QStringList vargs;
            vargs << QStringLiteral("--http-proxy=%1:%2").arg(proxy.host()).arg(proxy.port(8080));
            VlcInstance::initGlobalInstance(vargs);
        }
    }

    Vlyc vlyc;

    if (!args["proxy"].toString().isNull())
    {
        QUrl proxyUrl(args["proxy"].toString());
        QNetworkProxy::ProxyType type;
        if (proxyUrl.scheme().toLower() == "http")
            type = QNetworkProxy::HttpProxy;
        else if (proxyUrl.scheme().toLower() == "socks")
            type = QNetworkProxy::Socks5Proxy;
        else if (proxyUrl.scheme().toLower() == "https")
            type = QNetworkProxy::HttpProxy;
        else
        {
            std::cerr << "Unknown Proxy scheme: " << proxyUrl.scheme().toLocal8Bit().constData() << std::endl;
            type = QNetworkProxy::NoProxy;
        }
        QNetworkProxy proxy(type, proxyUrl.host(), proxyUrl.port(8080), proxyUrl.userName(), proxyUrl.password());
        QNetworkProxy::setApplicationProxy(proxy);
        std::cerr << "Proxy: " << qPrintable(proxy.hostName()) << ":" << qPrintable(QString::number(proxy.port())) << std::endl;
    }

    vlyc.window()->show();

    if (!args["mrl"].toString().isNull())
        emit vlyc.window()->playMrlSignal(args["mrl"].toString());

    return app.exec();
}
