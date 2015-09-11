/******************************************************************************
  Copyright 2014 Tom Cornall

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 ******************************************************************************/
#include "PaddedBox.h"

/*
 *
 */
PaddedBox::PaddedBox(QWidget *child, QWidget *parent):
    QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    hbox->addWidget(child);
    hbox->addStretch();

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addStretch();
    vbox->addLayout(hbox);
    vbox->addStretch();

    setLayout(vbox);
}
