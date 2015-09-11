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
#include "Key.h"

using namespace std;

/*
 *
 */
Key::Key(DiagInterface &diag, int layer, int col, int row,
        unsigned char scancode, QWidget *parent):
    QWidget(parent),
    diag(diag),
    layer(layer),
    col(col),
    row(row)
{
    QFont f = font();
    f.setStyleHint(QFont::Monospace);
    f.setFamily("");
    f.setPointSize(f.pointSize() - 2);
    setFont(f);

    int cmbIdx = -1;
    scancodeCombo = new QComboBox;
    for (unsigned char i = 0x00; i < 0xff; i++)
    {
        string scS = scancodeName(i);
        if (scS.empty())
            continue;

        scancodeCombo->addItem(QString::fromStdString(scS), i);
        if (i == scancode)
            cmbIdx = scancodeCombo->count() - 1;
    }
    if (cmbIdx >= 0)
        scancodeCombo->setCurrentIndex(cmbIdx);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addSpacing(10);
    vbox->addWidget(scancodeCombo);
    vbox->addSpacing(10);
    vbox->setContentsMargins(0, 0, 0, 0);

    setLayout(vbox);

    connect(scancodeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(scancodeComboIndexChanged(int)));
}

/*
 *
 */
unsigned char Key::scancode(void)
{
    return scancodeCombo->itemData(scancodeCombo->currentIndex()).toInt();
}

/*
 *
 */
void Key::setPressed(bool val)
{
    if (pressed == val)
        return;

    pressed = val;
    update();
}

/*
 *
 */
void Key::paintEvent(QPaintEvent *event)
{
    if (pressed)
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::darkGray);
    }
    else
        QWidget::paintEvent(event);
}

/*
 *
 */
void Key::scancodeComboIndexChanged(int)
{
    diag.setScancode(layer, col, row, scancode());
}
