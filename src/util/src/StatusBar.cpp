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
#include "StatusBar.h"

/*
 *
 */
StatusBar::StatusBar(DiagInterface &diag, QWidget *parent):
    QStatusBar(parent),
    diag(diag)
{
    QLabel *guiVersionLabel = new QLabel("Util v" + QString(VER));
    firmwareVersionLabel = new QLabel;
    controllerTypeLabel = new QLabel;
    matrixSizeLabel = new QLabel;
    nkroStateLabel = new QLabel;

    insertWidget(0, guiVersionLabel);
    insertWidget(1, firmwareVersionLabel);
    insertWidget(2, controllerTypeLabel);
    insertWidget(3, matrixSizeLabel);
    insertWidget(4, nkroStateLabel);

    updateStatus();
    updateVersion();

    /* query controller every second for state */
    pollTimer.start(1000, this);
}

/*
 *
 */
void StatusBar::timerEvent(QTimerEvent *)
{
    /* the only thing that really changes while the keyboard is running is
     * the NKRO state; we get everything else for free (except version).
     */
    updateStatus();
}

/*
 *
 */
static QString keyboardTypeAsString(DiagKeyboardType type)
{
    switch (type)
    {
        case dktBeamspring:
            return "Beamspring";
        case dktBeamspringDisplaywriter:
            return "Beamspring Displaywriter";
        case dktModelF:
            return "Model F";
        default:
            return "(invalid controller)";
    }
}

/*
 *
 */
void StatusBar::updateStatus(void)
{
    diag.updateControllerInfo();

    controllerTypeLabel->setText(keyboardTypeAsString(diag.keyboardType()));
    matrixSizeLabel->setText(QString::number(diag.cols()) + "x" +
            QString::number(diag.rows()) + " matrix");
    nkroStateLabel->setText(diag.usingNKROReport() ? "NKRO mode" : "Boot mode");
}

/*
 *
 */
void StatusBar::updateVersion(void)
{
    firmwareVersionLabel->setText("Firmware v" +
            QString::fromStdString(diag.version()));
}
