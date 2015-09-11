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
#include "ExpansionHeader.h"

/*
 *
 */
ExpansionHeader::ExpansionHeader(DiagInterface &diag,
        const bool &kbdFocusEnabled, QWidget *parent):
    QWidget(parent),
    diag(diag),
    kbdFocusEnabled(kbdFocusEnabled)
{
    modeCombo = new NonFocusedComboBox(kbdFocusEnabled);
    populateModeCombo();

    val1Label = new QLabel;
    val1SpinBox = new NonFocusedSpinBox(kbdFocusEnabled);
    val1SpinBox->setMaximum(255);

    val2Label = new QLabel;
    val2SpinBox = new NonFocusedSpinBox(kbdFocusEnabled);
    val2SpinBox->setMaximum(255);

    QPushButton *storeButton = new QPushButton("Store in EEPROM");

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(modeCombo);
    vbox->addWidget(val1Label);
    vbox->addWidget(val1SpinBox);
    vbox->addWidget(val2Label);
    vbox->addWidget(val2SpinBox);
    vbox->addWidget(storeButton);

    setLayout(vbox);

    connect(storeButton, SIGNAL(clicked()), SLOT(storeModeButtonClicked()));

    updateMode();
}

/*
 *
 */
ExpMode ExpansionHeader::mode(void)
{
    return (ExpMode)modeCombo->itemData(modeCombo->currentIndex()).toInt();
}

/*
 *
 */
void ExpansionHeader::populateModeCombo(void)
{
    for (int i = 0; i < expModeEND; i++)
    {
        switch (i)
        {
            case expModeDisabled:
                modeCombo->addItem("Disabled");
                break;
            case expModeSolenoid:
                modeCombo->addItem("Solenoid/Buzzer");
                break;
            case expModeLockLEDs:
                modeCombo->addItem("Lock LEDs");
                break;
            case expModeSolenoidPlusNOCapsLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Caps Lock Switch (NO)");
                break;
            case expModeSolenoidPlusNCCapsLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Caps Lock Switch (NC)");
                break;
            case expModeSolenoidPlusNONumLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Num Lock switch (NO)");
                break;
            case expModeSolenoidPlusNCNumLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Num Lock switch (NC)");
                break;
            case expModeSolenoidPlusNOShiftLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Shift Lock Switch (NO)");
                break;
            case expModeSolenoidPlusNCShiftLockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Shift Lock Switch (NC)");
                break;
            case expModeSolenoidPlusNOFn1LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn1 Lock Switch (NO)");
                break;
            case expModeSolenoidPlusNCFn1LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn1 Lock Switch (NC)");
                break;
            case expModeSolenoidPlusNOFn2LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn2 Lock Switch (NO)");
                break;
            case expModeSolenoidPlusNCFn2LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn2 Lock Switch (NC)");
                break;
            case expModeSolenoidPlusNOFn3LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn3 Lock Switch (NO)");
                break;
            case expModeSolenoidPlusNCFn3LockSwitch:
                modeCombo->addItem("Solenoid/Buzzer + Fn3 Lock Switch (NC)");
                break;
        }

        modeCombo->setItemData(i, i);
    }
}

/*
 *
 */
void ExpansionHeader::adjustVals(ExpMode mode)
{
    switch (mode)
    {
        case expModeSolenoid:
        case expModeSolenoidPlusNOCapsLockSwitch:
        case expModeSolenoidPlusNCCapsLockSwitch:
        case expModeSolenoidPlusNONumLockSwitch:
        case expModeSolenoidPlusNCNumLockSwitch:
        case expModeSolenoidPlusNOShiftLockSwitch:
        case expModeSolenoidPlusNCShiftLockSwitch:
        case expModeSolenoidPlusNOFn1LockSwitch:
        case expModeSolenoidPlusNCFn1LockSwitch:
        case expModeSolenoidPlusNOFn2LockSwitch:
        case expModeSolenoidPlusNCFn2LockSwitch:
        case expModeSolenoidPlusNOFn3LockSwitch:
        case expModeSolenoidPlusNCFn3LockSwitch:
            val1Label->setText("Extend time (ms):");
            val2Label->setText("Retract time (ms):");
            val1SpinBox->setEnabled(true);
            val2SpinBox->setEnabled(true);
            break;
        default:
            val1Label->setText("");
            val2Label->setText("");
            val1SpinBox->setEnabled(false);
            val2SpinBox->setEnabled(false);
            break;
    }
}

/*
 *
 */
void ExpansionHeader::updateMode(void)
{
    int mode, val1, val2;

    diag.expMode(mode, val1, val2);

    disconnect(modeCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setMode(int)));
    disconnect(val1SpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setMode(int)));
    disconnect(val2SpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setMode(int)));

    modeCombo->setCurrentIndex(modeCombo->findData(mode));
    val1SpinBox->setValue(val1);
    val2SpinBox->setValue(val2);

    adjustVals((ExpMode)mode);

    connect(modeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(setMode(int)));
    connect(val1SpinBox, SIGNAL(valueChanged(int)), SLOT(setMode(int)));
    connect(val2SpinBox, SIGNAL(valueChanged(int)), SLOT(setMode(int)));
}

/*
 *
 */
void ExpansionHeader::setMode(int)
{
    int mode = modeCombo->itemData(modeCombo->currentIndex()).toInt();
    int val1 = val1SpinBox->value();
    int val2 = val2SpinBox->value();

    adjustVals((ExpMode)mode);

    qDebug() << "setting exp mode";

    diag.setExpMode(mode, val1, val2);
}

/*
 *
 */
void ExpansionHeader::storeModeButtonClicked(void)
{
    setEnabled(false);

    diag.storeExpMode();

    QTimer::singleShot(1000, this, SLOT(storeModeComplete()));
}

/*
 *
 */
void ExpansionHeader::storeModeComplete(void)
{
    updateMode();
    setEnabled(true);
}
