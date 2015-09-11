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
#include "LayerConditionWatcher.h"

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
void LayerConditionWatcher::changed(int) { changed(); }
void LayerConditionWatcher::changed(void)
{
    diag.setLayerCondition(conditionIndex, toLayerCondition());
}
