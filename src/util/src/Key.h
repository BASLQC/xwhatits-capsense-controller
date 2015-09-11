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
#ifndef KEY_H
#define KEY_H

#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>
#include "DiagInterface.h"
#include "Scancodes.h"

class Key: public QWidget
{
    Q_OBJECT

    public:
        Key(DiagInterface &diag, int layer, int col, int row,
                unsigned char scancode, QWidget *parent = 0);
        unsigned char scancode(void);
        void setPressed(bool val);

    protected:
        virtual void paintEvent(QPaintEvent *event);

    private:
        DiagInterface &diag;
        int layer;
        int col;
        int row;
        QComboBox *scancodeCombo;
        bool pressed;

    private slots:
        void scancodeComboIndexChanged(int index);
};

#endif
