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
#ifndef COLSKIPS_H
#define COLSKIPS_H

#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include "DiagInterface.h"

class ColSkips: public QWidget
{
    Q_OBJECT

    public:
        ColSkips(DiagInterface &diag, QWidget *parent = NULL);
        void updateState(void);
        std::vector<bool> state(void);

    signals:
        void skipsChanged(void);

    private:
        DiagInterface &diag;
        std::vector<QCheckBox *> skipCBs;

    private slots:
        void storeButtonClicked(void);
        void storeComplete(void);
        void helpButtonClicked(void);
        void skipCBChanged(int);

};

#endif
