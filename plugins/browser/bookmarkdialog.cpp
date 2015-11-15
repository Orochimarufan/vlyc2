/*****************************************************************************
 * vlyc2 - A Desktop YouTube client
 * Copyright (C) 2013-2015 Taeyeon Mori <orochimarufan.x3@gmail.com>
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

#include "bookmarkdialog.h"
#include "ui_bookmarkdialog.h"

#include <QtCore/QAbstractTableModel>

BookmarksDialog::BookmarksDialog(BookmarkModel *bookmarks, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookmarksDialog),
    mp_bookmarks(bookmarks)
{
    ui->setupUi(this);

    ui->tableView->setModel(mp_bookmarks);
}

void BookmarksDialog::on_btn_close_clicked()
{
    close();
}

void BookmarksDialog::on_btn_add_clicked()
{
    static Bookmark templ {QIcon(), "New Bookmark", QUrl("https://")};
    mp_bookmarks->add(templ);
}

void BookmarksDialog::on_btn_rem_clicked()
{
    auto sel = ui->tableView->selectionModel()->selectedRows();

    if (sel.empty())
        return;

    qSort(sel);

    int off = 0;
    size_t count;
    size_t first = sel[0].row();
    size_t last = first;
    size_t next = 1;

    while (next <= sel.size())
    {
        size_t cur;
        while (next < sel.size() && (cur = sel[next].row()) == last + 1)
        {
            last = cur;
            ++next;
        }

        count = last - first + 1;
        mp_bookmarks->removeRows(first + off, count);
        off -= count;
        first = last = cur;
        ++next;
    }
}

BookmarksDialog::~BookmarksDialog()
{
    delete ui;
}
