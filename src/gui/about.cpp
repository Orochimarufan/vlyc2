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

#include "about.h"
#include "ui_about.h"

#include <QLabel>

#include <VlcInstance.h>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

#define PAGE_LINK(name) connect(ui-> name ## Link, &QLabel::linkActivated, this, &About::onLinkClicked)
    PAGE_LINK(license);
    PAGE_LINK(authors);
    PAGE_LINK(versions);
#undef PAGE_LINK

    // Generate versions page
    static QString versions;
    if (versions.isNull())
    {
        QStringList ver;
        ver << "<!DOCTYPE html ><html><body><table><thead><th colspan=\"2\">Library Versions</th></thead><tbody>";
        QString vlc_changeset(VlcInstance::libvlc_changeset());
        vlc_changeset = vlc_changeset.mid(vlc_changeset.lastIndexOf("-")+1);
        ver << "<tr><th>libvlc</th><td>" << VlcInstance::libvlc_version() << "(" << vlc_changeset << ")</td></tr>";
        ver << "<tr><th>QtVlc</th><td>" << VlcInstance::QtVlc_version() << "(" << VlcInstance::QtVlc_version_git() << ")</td></tr>";
        ver << "<tr><th>Qt</th><td>" << qVersion() << "</td></tr>";
        ver << "</tbody></table></body></html>";
        versions = ver.join("");
    }
    ui->versionsPage->setHtml(versions);

    // Version
    ui->appver->setText(qApp->applicationVersion());
}

void About::onLinkClicked(const QString &url)
{
    if (url.startsWith("page:"))
    {
        int page = url.mid(5).toInt();
        ui->stackedWidget->setCurrentIndex(page);
    }
}

About::~About()
{
    delete ui;
}
