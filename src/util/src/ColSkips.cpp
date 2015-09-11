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
#include "ColSkips.h"

using namespace std;

/*
 *
 */
ColSkips::ColSkips(DiagInterface &diag, QWidget *parent):
    QWidget(parent),
    diag(diag)
{
    QPushButton *storeButton = new QPushButton("Store in EEPROM");
    QToolButton *helpButton = new QToolButton;
    helpButton->setText("?");
    helpButton->setToolTip("Help");

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    hbox->addWidget(storeButton);
    hbox->addWidget(helpButton);

    QGridLayout *grid = new QGridLayout;

    for (int i = 0; i < diag.cols(); i++)
    {
        QLabel *l = new QLabel(QString::number(i + 1));
        grid->addWidget(l, 0, i, Qt::AlignCenter);

        QCheckBox *cb = new QCheckBox;
        skipCBs.push_back(cb);
        grid->addWidget(cb, 1, i, Qt::AlignCenter);

        connect(cb, SIGNAL(stateChanged(int)), SLOT(skipCBChanged(int)));
    }

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(grid);
    vbox->addLayout(hbox);

    setLayout(vbox);

    connect(storeButton, SIGNAL(clicked()), SLOT(storeButtonClicked()));
    connect(helpButton, SIGNAL(clicked()), SLOT(helpButtonClicked()));

    updateState();
}

/*
 *
 */
void ColSkips::updateState(void)
{
    vector<bool> skips = diag.kbdColSkips();

    for (int i = 0; i < diag.cols(); i++)
        skipCBs[i]->setChecked(skips[i]);
}

/*
 *
 */
std::vector<bool> ColSkips::state(void)
{
    vector<bool> skips(diag.cols(), false);

    for (int i = 0; i < diag.cols(); i++)
        skips[i] = skipCBs[i]->isChecked();

    return skips;
}

/*
 *
 */
void ColSkips::skipCBChanged(int)
{
    cerr << "setting kbd col skips" << endl;

    diag.setKbdColSkips(state());

    emit skipsChanged();
}

/*
 *
 */
void ColSkips::storeButtonClicked(void)
{
    setEnabled(false);

    diag.storeKbdColSkips();

    QTimer::singleShot(1000, this, SLOT(storeComplete()));
}

/*
 *
 */
void ColSkips::storeComplete(void)
{
    updateState();
    setEnabled(true);
}

/*
 *
 */
void ColSkips::helpButtonClicked(void)
{
    QMessageBox::information(this, "Column skips help",
            "<p>Some keyboards have unused columns. If these are connected to "
            "GND, as with some smaller Model F keyboards, the keyboard "
            "controller can eventually become damaged. Setting a column skip "
            "for those columns will mean they are never driven.</p>"
            "<p>This can also speed up the scan-rate for matrices which have "
            "valid columns but with no keys.</p>");
}
