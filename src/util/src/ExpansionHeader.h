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
#ifndef EXPANSION_HEADER_H
#define EXPANSION_HEADER_H

#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "DiagInterface.h"
#include "NonFocused.h"
#include "exp_types.h"

class ExpansionHeader: public QWidget
{
    Q_OBJECT

    public:
        ExpansionHeader(DiagInterface &diag, const bool &kbdFocusEnabled,
                QWidget *parent = NULL);
        void updateMode(void);
        ExpMode mode(void);
        int val1(void) { return val1SpinBox->value(); }
        int val2(void) { return val2SpinBox->value(); }

    private:
        DiagInterface &diag;
        const bool &kbdFocusEnabled;
        NonFocusedComboBox *modeCombo;
        QLabel *val1Label;
        NonFocusedSpinBox *val1SpinBox;
        QLabel *val2Label;
        NonFocusedSpinBox *val2SpinBox;

    private slots:
        void populateModeCombo(void);
        void adjustVals(ExpMode mode);
        void setMode(int);
        void storeModeButtonClicked(void);
        void storeModeComplete(void);
};

#endif
