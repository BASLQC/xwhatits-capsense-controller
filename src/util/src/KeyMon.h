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
#ifndef KEYMON_H
#define KEYMON_H

#define KEYMON_CELL_SIZE 8

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <vector>

class KeyMon: public QWidget
{
    Q_OBJECT

    public:
        KeyMon(QWidget *parent = 0);
        virtual QSize sizeHint(void) const;
        void updateStates(const std::vector<std::vector<bool>> &val);

    protected:
        virtual void leaveEvent(QEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void paintEvent(QPaintEvent *event);

    private:
        std::vector<std::vector<bool>> states;
        QString cellText;
};

#endif
