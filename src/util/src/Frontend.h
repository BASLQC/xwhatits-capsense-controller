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
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <stdexcept>
#include "DiagInterface.h"
#include "ImportExport.h"
#include "Key.h"
#include "KeyMon.h"
#include "LayerConditionWatcher.h"
#include "NonFocused.h"
#include "exp_types.h"

class Frontend: public QWidget
{
    Q_OBJECT

    public:
        Frontend(DiagInterface &diag, QWidget *parent = NULL);
        virtual ~Frontend(void);

    private:
        DiagInterface &diag;
        NonFocusedSpinBox *vrefSpinBox;
        QTimer *vrefMaskTimer;
        QPushButton *autoCalButton;
        QPushButton *storeVrefButton;
        QPushButton *bootloaderButton;
        QTabWidget *matrixTabWidget;
        QLabel *kbdVersionLabel;
        QLabel *kbdTypeLabel;
        QLabel *kbdMatrixSizeLabel;
        QLabel *kbdLayerCountLabel;
        NonFocusedComboBox *expModeCombo;
        NonFocusedSpinBox *expVal1SpinBox;
        NonFocusedSpinBox *expVal2SpinBox;
        KeyMon *keyMon;
        QGridLayout *layerConditionsGrid;
        QGridLayout *colSkipsGrid;
        std::vector<QCheckBox *> colSkipsCBs;
        QTimer *keyStatesTimer;
        std::vector<std::vector<std::vector<Key *>>> keyWidgets;
        std::vector<LayerConditionWatcher *> layerConditionWatchers;
        QPushButton *storeMatrixButton;
        QPushButton *loadMatrixButton;

        void buildColSkips(void);
        std::vector<bool> colSkipsFromCBs(void);

    private slots:
        void updateVref(void);
        void vrefValueChanged(int);
        void setVrefFromBox(void);
        void autoCalButtonClicked(void);
        void autoCalComplete(void);
        void storeVrefButtonClicked(void);
        void vrefHelpButtonClicked(void);

        void setExpValsEnabled(ExpMode mode);
        void updateExpMode(void);
        void setExpMode(int);
        void storeExpModeButtonClicked(void);
        void storeExpModeComplete(void);

        void bootloaderButtonClicked(void);
        void bootloaderHelpButtonClicked(void);

        void buildMatrix(void);
        void buildLayerConditions(void);
        void queryKbdVersion(void);

        void updateKeyStates(void);

        void updateColSkips(void);
        void colSkipCBChanged(int);
        void storeColSkipsButtonClicked(void);
        void storeColSkipsComplete(void);
        void colSkipsHelpButtonClicked(void);
        void setKeyColsEnabled(void);

        void storeMatrixButtonClicked(void);
        void storeMatrixComplete(void);

        void loadMatrixButtonClicked(void);
        void loadMatrixComplete(void);

        void importMatrixButtonClicked(void);
        void exportMatrixButtonClicked(void);
};

#endif
