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
#include "KeyMon.h"

/*
 *
 */
KeyMon::KeyMon(QWidget *parent):
    QWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/*
 *
 */
QSize KeyMon::sizeHint(void) const
{
    if (states.empty())
        return QSize(0, 0);

    QFontMetrics fm(font());
    return QSize(states.size() * KEYMON_CELL_SIZE,
            states[0].size() * KEYMON_CELL_SIZE + fm.height());
}

/*
 *
 */
void KeyMon::updateStates(const std::vector<std::vector<bool>> &val)
{
    if (states.empty())
        updateGeometry();

    states = val;
    update();
}

/*
 *
 */
void KeyMon::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    cellText = "";
}

/*
 *
 */
void KeyMon::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (states.empty())
        return;

    size_t col = event->pos().x() / KEYMON_CELL_SIZE + 1;
    size_t row = event->pos().y() / KEYMON_CELL_SIZE + 1;

    if (col > states.size() || row > states[0].size())
        cellText = "";
    else
        cellText = QString::number(col) + ", " + QString::number(row);
}

/*
 *
 */
void KeyMon::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (states.empty() || states[0].empty())
        return;

    QPainter painter(this);

    int cols = states.size();
    int rows = states[0].size();
    for (int col = 0; col <  cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            painter.fillRect(col * KEYMON_CELL_SIZE + 1,
                    row * KEYMON_CELL_SIZE + 1,
                    KEYMON_CELL_SIZE - 2,
                    KEYMON_CELL_SIZE - 2,
                    states[col][row] ? Qt::darkGray : Qt::white);
        }
    }

    QFontMetrics fm(font());
    painter.drawText(rect().width() - fm.width(cellText),
            rect().height() - fm.descent(), cellText);
}
