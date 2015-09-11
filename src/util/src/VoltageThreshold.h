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
#ifndef VOLTAGETHRESHOLD_H
#define VOLTAGETHRESHOLD_H

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include "DiagInterface.h"
#include "NonFocused.h"

class VoltageThreshold: public QWidget
{
    Q_OBJECT

    public:
        VoltageThreshold(DiagInterface &diag, const bool &kbdFocusEnabled,
                QWidget *parent = NULL);

    private:
        DiagInterface &diag;
        QTimer maskTimer;
        NonFocusedSpinBox *thresholdSpinBox;
        QPushButton *autoCalButton;
        QPushButton *storeButton;
        unsigned short cachedThreshold;

    private slots:
        void updateThreshold(void);
        void thresholdValueChanged(int);
        void maskTimerExpired(void);
        void autoCalButtonClicked(void);
        void autoCalComplete(void);
        void autoCalEnableComplete(void);
        void storeButtonClicked(void);
        void storeComplete(void);
};

#endif
