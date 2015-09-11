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
#include "NonFocused.h"

/*
 *
 */
NonFocusedSpinBox::NonFocusedSpinBox(void)
{
    setFocusPolicy(Qt::StrongFocus);
}

/*
 *
 */
void NonFocusedSpinBox::focusInEvent(QFocusEvent *event)
{
    QSpinBox::focusInEvent(event);
    setFocusPolicy(Qt::WheelFocus);
}

/*
 *
 */
void NonFocusedSpinBox::focusOutEvent(QFocusEvent *event)
{
    QSpinBox::focusOutEvent(event);
    setFocusPolicy(Qt::StrongFocus);
}

/*
 *
 */
void NonFocusedSpinBox::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

/*
 *
 */
void NonFocusedSpinBox::wheelEvent(QWheelEvent *event)
{
    if (focusPolicy() == Qt::WheelFocus)
        QSpinBox::wheelEvent(event);
    else
        event->ignore();

}

/*
 *
 */
NonFocusedComboBox::NonFocusedComboBox(void)
{
    setFocusPolicy(Qt::StrongFocus);
}

/*
 *
 */
void NonFocusedComboBox::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    setFocusPolicy(Qt::WheelFocus);
}

/*
 *
 */
void NonFocusedComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    setFocusPolicy(Qt::StrongFocus);
}

/*
 *
 */
void NonFocusedComboBox::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

/*
 *
 */
void NonFocusedComboBox::wheelEvent(QWheelEvent *event)
{
    if (focusPolicy() == Qt::WheelFocus)
        QComboBox::wheelEvent(event);
    else
        event->ignore();

}
