/*
    Copyright 2016 - 2017 Benjamin Vedder	benjamin@vedder.se

    This file is part of ExiTool.

    ExiTool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ExiTool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#include "pagelistitem.h"
#include <QHBoxLayout>
#include <QStyle>

PageListItem::PageListItem(QString name,
                           QString icon,
                           QString groupIcon,
                           QWidget *parent) : QWidget(parent)
{
    mIconLabel = new QLabel;
    mNameLabel = new QLabel;
    mGroupLabel = new QLabel;
    mSpaceStart = new QSpacerItem(6, 0);

    mIconLabel->setScaledContents(true);
    mGroupLabel->setObjectName("groupChip");

    setName(name);
    setIcon(icon);
    setGroupChip(groupIcon);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(10, 6, 10, 6);
    layout->setSpacing(8);

    layout->addSpacerItem(mSpaceStart);
    layout->addWidget(mIconLabel);
    layout->addWidget(mNameLabel);
    layout->addStretch();
    layout->addWidget(mGroupLabel);

    this->setLayout(layout);
}

void PageListItem::setName(const QString &name)
{
    mNameLabel->setText(name);
}

void PageListItem::setIcon(const QString &path)
{
    if (!path.isEmpty()) {
        mIconLabel->setPixmap(QPixmap(path));

        QFontMetrics fm(this->font());
        int height = fm.height() * 1.3;

        mIconLabel->setFixedSize(height, height);
    } else {
        mIconLabel->setPixmap(QPixmap());
    }
}

void PageListItem::setGroupIcon(const QString &path)
{
    // Legacy raster API kept for source compat; route to chip if input
    // is a plain token (no slash), otherwise hide the badge.
    if (!path.isEmpty() && !path.contains('/') && !path.contains('\\')) {
        setGroupChip(path);
    } else {
        setGroupChip(QString());
    }
}

void PageListItem::setGroupChip(const QString &text)
{
    mGroupLabel->setText(text);
    mGroupLabel->setProperty("chip", text.toLower());
    mGroupLabel->style()->unpolish(mGroupLabel);
    mGroupLabel->style()->polish(mGroupLabel);
    mGroupLabel->setVisible(!text.isEmpty());
}

QString PageListItem::name()
{
    return mNameLabel->text();
}

void PageListItem::setBold(bool bold)
{
    QFont f = mNameLabel->font();
    f.setBold(bold);
    mNameLabel->setFont(f);
}

void PageListItem::setIndented(bool indented)
{
    mSpaceStart->changeSize(indented ? 18 : 2, 0);
}
