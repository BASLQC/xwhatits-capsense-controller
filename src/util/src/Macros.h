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
#ifndef MACROS_H
#define MACROS_H

#include <iomanip>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "DiagInterface.h"
#include "Macro.h"
#include "NonFocused.h"

class Macros: public QWidget
{
    Q_OBJECT

    public:
        Macros(DiagInterface &diag, const bool &kbdFocusEnabled,
                QWidget *parent = NULL);
        std::vector<unsigned char> asBytes(void);

    public slots:
        void loadFromKeyboard(void);
        void storeInKeyboard(void);

    private:
        DiagInterface &diag;
        const bool &kbdFocusEnabled;
        NonFocusedComboBox *macroCombo;
        QStackedWidget *macroStack;
        int bytesAvailable;
        QLabel *bytesUsedLabel;

        void removeAllMacros(void);

    private slots:
        void loadButtonClicked(void);
        void storeButtonClicked(void);
        void addButtonClicked(void);
        void removeButtonClicked(void);
        void removeAllButtonClicked(void);
        void macroNameChanged(QString name);
        void macroByteCountChanged(void);
};

#endif
