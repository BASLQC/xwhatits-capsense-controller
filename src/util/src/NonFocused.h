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
#ifndef NONFOCUSED_H
#define NONFOCUSED_H

#include <QComboBox>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSpinBox>
#include <QWheelEvent>

class NonFocusedSpinBox:
    public QSpinBox
{
    Q_OBJECT

    public:
        NonFocusedSpinBox(const bool &kbdFocusEnabled);

    protected:
        const bool &kbdFocusEnabled;

        virtual void focusInEvent(QFocusEvent *event);
        virtual void focusOutEvent(QFocusEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void wheelEvent(QWheelEvent *event);

};

class NonFocusedComboBox:
    public QComboBox
{
    Q_OBJECT

    public:
        NonFocusedComboBox(const bool &kbdFocusEnabled);

    protected:
        const bool &kbdFocusEnabled;

        virtual void focusInEvent(QFocusEvent *event);
        virtual void focusOutEvent(QFocusEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void wheelEvent(QWheelEvent *event);
};

#endif
