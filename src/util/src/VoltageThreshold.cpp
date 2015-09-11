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
#include "VoltageThreshold.h"

/*
 *
 */
VoltageThreshold::VoltageThreshold(DiagInterface &diag,
        const bool &kbdFocusEnabled, QWidget *parent):
    QWidget(parent),
    diag(diag),
    maskTimer(this)
{
    maskTimer.setSingleShot(true);

    QLabel *thresholdLabel = new QLabel("Current threshold:");
    thresholdSpinBox = new NonFocusedSpinBox(kbdFocusEnabled);
    thresholdSpinBox->setMaximum(65535);

    autoCalButton = new QPushButton("Auto-calibrate");
    storeButton = new QPushButton("Store override in EEPROM");

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(thresholdLabel, 0, 0);
    grid->addWidget(thresholdSpinBox, 0, 1);
    grid->addWidget(autoCalButton, 1, 0, 1, 2);
    grid->addWidget(storeButton, 2, 0, 1, 2);

    setLayout(grid);

    connect(&maskTimer, SIGNAL(timeout()), SLOT(maskTimerExpired()));
    connect(autoCalButton, SIGNAL(clicked()), SLOT(autoCalButtonClicked()));
    connect(storeButton, SIGNAL(clicked()), SLOT(storeButtonClicked()));

    updateThreshold();
}

/*
 *
 */
void VoltageThreshold::updateThreshold(void)
{
    disconnect(thresholdSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(thresholdValueChanged(int)));

    thresholdSpinBox->setValue(diag.vref());

    connect(thresholdSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(thresholdValueChanged(int)));

    setEnabled(true);
}

/*
 *
 */
void VoltageThreshold::thresholdValueChanged(int)
{
    /* mask for a bit before actually setting value in case just mashing one
     * of the arrows, or typing etc.
     */
    maskTimer.start(500);
    autoCalButton->setEnabled(false);
    storeButton->setEnabled(false);
}

/*
 *
 */
void VoltageThreshold::maskTimerExpired(void)
{
    qDebug() << "setting vref";
    diag.setVref(thresholdSpinBox->value());
    autoCalButton->setEnabled(true);
    storeButton->setEnabled(true);
}

/*
 *
 */
void VoltageThreshold::autoCalButtonClicked(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Auto-calibration",
            "Auto-calibration requires the special (PRESSED) and (RELEASED) "
            "scancodes to be set correctly. It is recommended that you try to "
            "manually calibrate the keyboard first before proceeding with "
            "auto-calibration. Are you sure you wish to proceed?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        setEnabled(false);

        diag.autoCalibrate();

        /* keyboard will disappear for a bit while it does its thing */
        QTimer::singleShot(1000, this, SLOT(autoCalComplete()));
    }
}

/*
 *
 */
void VoltageThreshold::autoCalComplete(void)
{
    cachedThreshold = diag.vref();

    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Auto-calibration complete",
            "Auto-calibration finished, threshold is " +
            QString::number(cachedThreshold) + ". "
            "Would you like auto-calibration to "
            "take place automatically every time the keyboard is powered up?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        diag.setScanEnabled(false);
        diag.setVref(65535);
        diag.storeVref();
        QTimer::singleShot(1000, this, SLOT(autoCalEnableComplete()));
    }
    else
        updateThreshold();
}

/*
 *
 */
void VoltageThreshold::autoCalEnableComplete(void)
{
    diag.setVref(cachedThreshold);
    setEnabled(true);
    updateThreshold();
    diag.setScanEnabled(true);
}

/*
 *
 */
void VoltageThreshold::storeButtonClicked(void)
{
    setEnabled(false);
    diag.storeVref();
    QTimer::singleShot(1000, this, SLOT(storeComplete()));
}

/*
 *
 */
void VoltageThreshold::storeComplete(void)
{
    setEnabled(true);
}
