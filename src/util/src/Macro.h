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
#ifndef MACRO_H
#define MACRO_H

#include <iostream>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "NonFocused.h"
#include "Scancodes.h"
#include "macro_types.h"

class MacroTriggerModsWidget;
class MacroModsWidget;

class Macro: public QWidget
{
    Q_OBJECT

    public:
        Macro(const bool &kbdFocusEnabled, QWidget *parent = NULL);
        QString name(void);
        std::vector<unsigned char> asBytes(void);
        std::vector<unsigned char> fromBytes(std::vector<unsigned char> bytes);

    signals:
        void nameChanged(QString name);
        void byteCountChanged(void);

    private:
        enum RowAsBytesFlags
        {
            rowWaitBreak = (1 << 0),
            rowPushMods  = (1 << 1)
        };

        const bool &kbdFocusEnabled;
        NonFocusedComboBox *triggerScancodeCombo;
        MacroTriggerModsWidget *triggerSetMods;
        QCheckBox *retainPressedModsCB;
        MacroModsWidget *triggerExcludedMods;
        QTableWidget *table;
        QLabel *bytesUsedLabel;
        int contextMenuRow;

        void addContextMenu(QWidget *w);
        void insertRowAt(int row, int cmd = 0);
        void populateCommandCombo(QComboBox *cb);
        void populateScancodeCombo(QComboBox *cb);
        int findWidgetRow(QWidget *w);
        bool createValueCell(int row, int commandType, int value = 0);
        void createNullValueCell(int row);
        void createScancodeValueCell(int row, int value = 0);
        void createModsValueCell(int row, int value = 0);
        void createDelayValueCell(int row, int value = 0);
        void resizeCells(void);
        void calculateTriggerModBytes(unsigned char *states,
                unsigned char *mask);
        std::vector<unsigned char> rowAsBytes(int row, int *flags);
        unsigned char valueByteScancode(int row);
        unsigned char valueByteMods(int row);
        unsigned char valueByteDelay(int row);
        void updateByteCountLabel(void);
        void removeAllRows(void);
        void setTriggerModsFromBytes(unsigned char state, unsigned char mask);
        std::vector<unsigned char>::iterator appendRowFromBytes(
                std::vector<unsigned char>::iterator i,
                std::vector<unsigned char>::iterator end);

    private slots:
        void triggerNameChanged(int i = 0);
        void addButtonClicked(void);
        void showTableContextMenu(QPoint pos);
        void commandComboIndexChanged(int);
        void contextMenuInsertTriggered(void);
        void contextMenuDeleteTriggered(void);
};

class MacroTriggerModsWidget: public QWidget
{
    Q_OBJECT

    public:
        MacroTriggerModsWidget(QWidget *parent = NULL);
        QString stateAsText(void);
        int stateOfEithers(void);
        int stateOfMods(void);
        void setStateFromBytes(unsigned char state, unsigned char mask);

    signals:
        void changed(void);

    private:
        QCheckBox *eitherCtrlCB;
        QCheckBox *eitherShiftCB;
        QCheckBox *eitherAltCB;
        QCheckBox *eitherGUICB;
        QCheckBox *modCBs[8];

        void blockCBSignals(bool value);
        void eitherCBToggled(bool state, int lhsMod);

    private slots:
        void eitherCtrlCBToggled(bool state);
        void eitherShiftCBToggled(bool state);
        void eitherAltCBToggled(bool state);
        void eitherGUICBToggled(bool state);
        void modCBToggled(bool);
};

class MacroModsWidget: public QWidget
{
    Q_OBJECT

    public:
        MacroModsWidget(QWidget *parent = NULL);
        QString stateAsText(void);
        int stateOfMods(void);
        void setStateOfMods(int state);

    signals:
        void changed(void);

    private:
        QCheckBox *modCBs[8];

    private slots:
        void allButtonClicked(void);
        void modCBToggled(bool);
};

QString macroBytesToString(std::vector<unsigned char> bytes);

#endif
