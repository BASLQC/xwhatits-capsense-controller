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
#ifndef FRONTEND_H
#define FRONTEND_H

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <stdexcept>
#include "ColSkips.h"
#include "DiagInterface.h"
#include "ExpansionHeader.h"
#include "ImportExport.h"
#include "Key.h"
#include "KeyMon.h"
#include "LayerConditions.h"
#include "Macros.h"
#include "NonFocused.h"
#include "PaddedBox.h"
#include "StatusBar.h"
#include "VoltageThreshold.h"
#include "exp_types.h"

class Frontend: public QMainWindow
{
    Q_OBJECT

    public:
        Frontend(DiagInterface &diag, QWidget *parent = NULL);
        virtual ~Frontend(void);

    private:
        DiagInterface &diag;
        bool kbdFocusEnabled;
        QPushButton *bootloaderButton;
        QTabWidget *mainTabWidget;
        KeyMon *keyMon;
        ExpansionHeader *expansionHeader;
        LayerConditions *layerConditions;
        ColSkips *colSkips;
        Macros *macros;
        QTimer *keyStatesTimer;
        std::vector<std::vector<std::vector<Key *>>> keyWidgets;

        void buildMenuBar(void);

    private slots:
        void enterBootloader(void);

        void guiKbdLockToggled(bool checked);

        void haltScanButtonToggled(bool checked);
        void haltScanHelpButtonClicked(void);

        void buildMatrix(void);

        void updateKeyStates(void);

        void setKeyColsEnabled(void);

        void importConfig(void);
        void exportConfig(void);
};

#endif
