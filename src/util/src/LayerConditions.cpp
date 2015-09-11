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
#include "LayerConditions.h"

using namespace std;

/*
 *
 */
LayerConditionWatcher::LayerConditionWatcher(DiagInterface &diag,
        int conditionIndex, QCheckBox *fn1Check, QCheckBox *fn2Check,
        QCheckBox *fn3Check, QComboBox *layerCombo, QObject *parent):
    QObject(parent),
    diag(diag),
    conditionIndex(conditionIndex),
    fn1Check(fn1Check),
    fn2Check(fn2Check),
    fn3Check(fn3Check),
    layerCombo(layerCombo)
{
    connect(fn1Check, SIGNAL(toggled(bool)), SLOT(changed(bool)));
    connect(fn2Check, SIGNAL(toggled(bool)), SLOT(changed(bool)));
    connect(fn3Check, SIGNAL(toggled(bool)), SLOT(changed(bool)));
    connect(layerCombo, SIGNAL(currentIndexChanged(int)), SLOT(changed(int)));
}

/*
 *
 */
LayerCondition LayerConditionWatcher::toLayerCondition(void)
{
    return LayerCondition(fn1Check->isChecked(), fn2Check->isChecked(),
            fn3Check->isChecked(), layerCombo->currentIndex());
}

/*
 *
 */
void LayerConditionWatcher::changed(bool) { changed(); }
void LayerConditionWatcher::changed(int)  { changed(); }
void LayerConditionWatcher::changed(void)
{
    diag.setLayerCondition(conditionIndex, toLayerCondition());
}

/*
 *
 */
LayerConditions::LayerConditions(DiagInterface &diag,
        const bool &kbdFocusEnabled, QWidget *parent):
    QWidget(parent),
    diag(diag),
    kbdFocusEnabled(kbdFocusEnabled)
{
    grid = new QGridLayout;
    setLayout(grid);

    rebuild();
}

/*
 *
 */
void LayerConditions::rebuild(void)
{
    vector<LayerCondition> cnds = diag.layerConditions();

    /* clear existing widgets */
    QLayoutItem *item;
    while ((item = grid->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    for (size_t i = 0; i < layerConditionWatchers.size(); i++)
        delete layerConditionWatchers[i];
    layerConditionWatchers.clear();

    int count = cnds.size();
    if (count == 0)
        throw runtime_error("error: keyboard reports zero layer conditions");

    for (int i = 0; i < 3; i++)
    {
        QLabel *fnLabel = new QLabel("<b>Fn" + QString::number(i + 1) + "</b>");
        grid->addWidget(fnLabel, 0, i + 2);
    }
    for (int i = 0; i < count; i++)
    {
        QLabel *rowLabel = new QLabel("<b>" + QString::number(i + 1) + "</b>");
        grid->addWidget(rowLabel, i + 1, 1);

        QCheckBox *fn1Check = new QCheckBox;
        fn1Check->setChecked(cnds[i].fn1Set());

        QCheckBox *fn2Check = new QCheckBox;
        fn2Check->setChecked(cnds[i].fn2Set());

        QCheckBox *fn3Check = new QCheckBox;
        fn3Check->setChecked(cnds[i].fn3Set());

        QLabel *arrowLabel = new QLabel(QChar(0x2192));

        NonFocusedComboBox *layerCombo =
            new NonFocusedComboBox(kbdFocusEnabled);
        for (int i = 0; i < 4; i++)
            layerCombo->addItem(i == 0 ? "Base Layer" : "Layer " +
                    QString::number(i), i);
        layerCombo->setCurrentIndex(cnds[i].layer());

        grid->addWidget(fn1Check, i + 1, 2);
        grid->addWidget(fn2Check, i + 1, 3);
        grid->addWidget(fn3Check, i + 1, 4);
        grid->addWidget(arrowLabel, i + 1, 5);
        grid->addWidget(layerCombo, i + 1, 6);

        layerConditionWatchers.push_back(new LayerConditionWatcher(diag,
                    i, fn1Check, fn2Check, fn3Check, layerCombo, this));
    }

    grid->setRowStretch(grid->rowCount(), 1);
    grid->setColumnStretch(0, 4);
    for (int i = 1; i < grid->columnCount(); i++)
        grid->setColumnStretch(i, 1);
    grid->setColumnStretch(grid->columnCount(), 4);
}
