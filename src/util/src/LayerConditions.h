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
#ifndef LAYERCONDITIONS_H
#define LAYERCONDITIONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include "DiagInterface.h"
#include "NonFocused.h"

class LayerConditionWatcher: public QObject
{
    Q_OBJECT

    public:
        LayerConditionWatcher(DiagInterface &diag, int conditionIndex,
                QCheckBox *fn1Check, QCheckBox *fn2Check, QCheckBox *fn3Check,
                QComboBox *layerCombo, QObject *parent = NULL);
        LayerCondition toLayerCondition(void);

    private:
        DiagInterface &diag;
        int conditionIndex;
        QCheckBox *fn1Check;
        QCheckBox *fn2Check;
        QCheckBox *fn3Check;
        QComboBox *layerCombo;

        void changed(void);

    private slots:
        void changed(bool);
        void changed(int);
};

class LayerConditions: public QWidget
{
    Q_OBJECT

    public:
        std::vector<LayerConditionWatcher *> layerConditionWatchers;

        LayerConditions(DiagInterface &diag, const bool &kbdFocusEnabled,
                QWidget *parent = NULL);
        void rebuild(void);

    private:
        DiagInterface &diag;
        const bool &kbdFocusEnabled;
        QGridLayout *grid;
};

#endif
