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
#include "Frontend.h"

using namespace std;

/*
 *
 */
Frontend::Frontend(DiagInterface &diag, QWidget *parent):
    QWidget(parent),
    diag(diag)
{
    setWindowTitle("IBM Capsense USB Util");

    /*
     * info section
     */
    QGroupBox *infoGroup = new QGroupBox("Info");

    QLabel *utilVersionLabel = new QLabel("Util version:\t\t" + QString(VER));
    kbdVersionLabel = new QLabel;
    kbdMatrixSizeLabel = new QLabel;

    QVBoxLayout *infoVBox = new QVBoxLayout;
    infoVBox->addWidget(utilVersionLabel);
    infoVBox->addWidget(kbdVersionLabel);
    infoVBox->addWidget(kbdMatrixSizeLabel);

    infoGroup->setLayout(infoVBox);

    /*
     * state section
     */
    QGroupBox *stateGroup = new QGroupBox("State");

    keyMon = new KeyMon;

    QVBoxLayout *stateVBox = new QVBoxLayout;
    stateVBox->setSizeConstraint(QLayout::SetMinimumSize);
    stateVBox->addStretch();
    stateVBox->addWidget(keyMon);
    stateVBox->addStretch();

    stateGroup->setLayout(stateVBox);

    /*
     * vref stuff
     */
    vrefMaskTimer = new QTimer(this);
    vrefMaskTimer->setSingleShot(true);

    QGroupBox *vrefGroup = new QGroupBox("Voltage threshold");

    QToolButton *vrefHelpButton = new QToolButton;
    vrefHelpButton->setText("?");
    vrefHelpButton->setToolTip("Help");

    QLabel *vrefSpinBoxLabel = new QLabel("Current threshold:");
    vrefSpinBox = new QSpinBox;
    vrefSpinBox->setMaximum(65535);
    QHBoxLayout *vrefSpinBoxHBox = new QHBoxLayout;
    vrefSpinBoxHBox->addWidget(vrefSpinBoxLabel);
    vrefSpinBoxHBox->addWidget(vrefSpinBox);

    autoCalButton = new QPushButton;
    storeVrefButton = new QPushButton("Store override in EEPROM");

    QHBoxLayout *vrefButtonHBox = new QHBoxLayout;
    vrefButtonHBox->addWidget(autoCalButton);
    vrefButtonHBox->addWidget(storeVrefButton);
    vrefButtonHBox->addWidget(vrefHelpButton);

    QVBoxLayout *vrefVBox = new QVBoxLayout;
    vrefVBox->addLayout(vrefSpinBoxHBox);
    vrefVBox->addLayout(vrefButtonHBox);

    vrefGroup->setLayout(vrefVBox);

    /*
     * expansion header section
     */
    QGroupBox *expGroup = new QGroupBox("Expansion header");

    QLabel *expModeComboLabel = new QLabel("Mode:");
    expModeCombo = new QComboBox;
    for (int i = 0; i < expModeEND; i++)
    {
        switch (i)
        {
            case expModeDisabled:
                expModeCombo->addItem("Disabled", i);
                break;
            case expModeSolenoid:
                expModeCombo->addItem("Solenoid", i);
                break;
            case expModeLockLEDs:
                expModeCombo->addItem("Lock LEDs", i);
                break;
            default:
                break;
        }
    }

    QLabel *expVal1Label = new QLabel("Extend time (ms):");
    expVal1SpinBox = new QSpinBox;
    expVal1SpinBox->setMaximum(255);

    QLabel *expVal2Label = new QLabel("Retract time (ms):");
    expVal2SpinBox = new QSpinBox;
    expVal2SpinBox->setMaximum(255);

    QPushButton *expStoreButton = new QPushButton("Store in EEPROM");

    QGridLayout *expGrid = new QGridLayout;
    expGrid->addWidget(expModeComboLabel, 0, 0);
    expGrid->addWidget(expModeCombo, 0, 1);
    expGrid->addWidget(expVal1Label, 1, 0);
    expGrid->addWidget(expVal1SpinBox, 1, 1);
    expGrid->addWidget(expVal2Label, 2, 0);
    expGrid->addWidget(expVal2SpinBox, 2, 1);
    expGrid->addWidget(expStoreButton, 3, 1);

    expGroup->setLayout(expGrid);

    /*
     * bootloader section
     */
    QGroupBox *bootloaderGroup = new QGroupBox("Bootloader");

    QToolButton *bootloaderHelpButton = new QToolButton;
    bootloaderHelpButton->setText("?");
    bootloaderHelpButton->setToolTip("Help");

    bootloaderButton = new QPushButton("Enter bootloader");

    QHBoxLayout *bootloaderHBox = new QHBoxLayout;
    bootloaderHBox->addWidget(bootloaderButton);
    bootloaderHBox->addWidget(bootloaderHelpButton);

    bootloaderGroup->setLayout(bootloaderHBox);

    /*
     * matrix stuff
     */
    QTimer *keyStatesTimer = new QTimer(this);
    keyStatesTimer->start(0);

    matrixTabWidget = new QTabWidget;

    QGroupBox *matrixGroup = new QGroupBox("Layout");
    layerConditionsGrid = new QGridLayout;
    QWidget *layerConditionsWidget = new QWidget;
    layerConditionsWidget->setLayout(layerConditionsGrid);
    matrixTabWidget->addTab(layerConditionsWidget, "Layer Conditions");

    colSkipsGrid = new QGridLayout;
    QWidget *colSkipsWidget = new QWidget;

    QPushButton *storeColSkipsButton = new QPushButton("Store in EEPROM");
    QToolButton *colSkipsHelpButton = new QToolButton;
    colSkipsHelpButton->setText("?");
    colSkipsHelpButton->setToolTip("Help");

    QHBoxLayout *colSkipsPBHBox = new QHBoxLayout;
    colSkipsPBHBox->addStretch();
    colSkipsPBHBox->addWidget(storeColSkipsButton);
    colSkipsPBHBox->addWidget(colSkipsHelpButton);

    QVBoxLayout *colSkipsVBox = new QVBoxLayout;
    colSkipsVBox->addStretch();
    colSkipsVBox->addLayout(colSkipsGrid);
    colSkipsVBox->addLayout(colSkipsPBHBox);
    colSkipsVBox->addStretch();

    colSkipsWidget->setLayout(colSkipsVBox);
    matrixTabWidget->addTab(colSkipsWidget, "Column Skips");
    buildColSkips();

    loadMatrixButton = new QPushButton("Load from EEPROM");
    storeMatrixButton = new QPushButton("Store in EEPROM");

    QPushButton *importMatrixButton = new QPushButton("Import layout");
    QPushButton *exportMatrixButton = new QPushButton("Export layout");

    QHBoxLayout *layoutHBox = new QHBoxLayout;
    layoutHBox->addWidget(loadMatrixButton);
    layoutHBox->addWidget(storeMatrixButton);
    layoutHBox->addWidget(importMatrixButton);
    layoutHBox->addWidget(exportMatrixButton);

    QVBoxLayout *matrixLayout = new QVBoxLayout;
    matrixLayout->setSizeConstraint(QLayout::SetMinimumSize);
    matrixLayout->addLayout(layoutHBox);
    matrixLayout->addWidget(matrixTabWidget);

    matrixGroup->setLayout(matrixLayout);

    /*
     * main layout
     */
    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->addWidget(infoGroup);
    hbox1->addWidget(stateGroup);
    hbox1->addWidget(vrefGroup);
    hbox1->addWidget(expGroup);
    hbox1->addWidget(bootloaderGroup);
    hbox1->setSizeConstraint(QLayout::SetMinimumSize);

    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addLayout(hbox1);
    vbox1->addWidget(matrixGroup);
    vbox1->setSizeConstraint(QLayout::SetMinimumSize);

    setLayout(vbox1);

    /* ask for a few things immediately */
    QTimer::singleShot(0, this, SLOT(updateVref()));
    QTimer::singleShot(0, this, SLOT(updateExpMode()));
    QTimer::singleShot(0, this, SLOT(buildMatrix()));
    QTimer::singleShot(0, this, SLOT(buildLayerConditions()));
    QTimer::singleShot(0, this, SLOT(updateColSkips()));
    QTimer::singleShot(0, this, SLOT(queryKbdVersion()));

    connect(vrefSpinBox, SIGNAL(valueChanged(int)),
            SLOT(vrefValueChanged(int)));
    connect(vrefMaskTimer, SIGNAL(timeout()), SLOT(setVrefFromBox()));
    connect(autoCalButton, SIGNAL(clicked()), SLOT(autoCalButtonClicked()));
    connect(storeVrefButton, SIGNAL(clicked()), SLOT(storeVrefButtonClicked()));
    connect(vrefHelpButton, SIGNAL(clicked()), SLOT(vrefHelpButtonClicked()));
    connect(expModeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(setExpMode(int)));
    connect(expVal1SpinBox, SIGNAL(valueChanged(int)), SLOT(setExpMode(int)));
    connect(expVal2SpinBox, SIGNAL(valueChanged(int)), SLOT(setExpMode(int)));
    connect(expStoreButton, SIGNAL(clicked()),
            SLOT(storeExpModeButtonClicked()));
    connect(bootloaderButton, SIGNAL(clicked()),
            SLOT(bootloaderButtonClicked()));
    connect(bootloaderHelpButton, SIGNAL(clicked()),
            SLOT(bootloaderHelpButtonClicked()));
    connect(storeColSkipsButton, SIGNAL(clicked()),
                SLOT(storeColSkipsButtonClicked()));
    connect(colSkipsHelpButton, SIGNAL(clicked()),
            SLOT(colSkipsHelpButtonClicked()));
    connect(keyStatesTimer, SIGNAL(timeout()), SLOT(updateKeyStates()));
    connect(storeMatrixButton, SIGNAL(clicked()),
            SLOT(storeMatrixButtonClicked()));
    connect(loadMatrixButton, SIGNAL(clicked()),
            SLOT(loadMatrixButtonClicked()));
    connect(importMatrixButton, SIGNAL(clicked()),
            SLOT(importMatrixButtonClicked()));
    connect(exportMatrixButton, SIGNAL(clicked()),
            SLOT(exportMatrixButtonClicked()));
}

/*
 *
 */
Frontend::~Frontend(void)
{
}

/*
 *
 */
void Frontend::updateVref(void)
{
    disconnect(vrefSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(vrefValueChanged(int)));

    vrefSpinBox->setValue(diag.vref());

    connect(vrefSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(vrefValueChanged(int)));

    /* re-enable widgets and set autocal button's text---should probably use
     * state machine or something as this is not needed every time
     */
    setEnabled(true);
    autoCalButton->setText("Auto-calibrate");
}

/*
 *
 */
void Frontend::vrefValueChanged(int)
{
    /* mask for a bit before actually setting value in case just mashing one
     * of the arrows, or typing etc.
     */
    vrefMaskTimer->start(500);
    autoCalButton->setEnabled(false);
    storeVrefButton->setEnabled(false);
}

/*
 *
 */
void Frontend::setVrefFromBox(void)
{
    qDebug() << "setting vref";
    diag.setVref(vrefSpinBox->value());
    autoCalButton->setEnabled(true);
    storeVrefButton->setEnabled(true);
}

/*
 *
 */
void Frontend::setExpValsEnabled(ExpMode mode)
{
    switch (mode)
    {
        case expModeSolenoid:
            expVal1SpinBox->setEnabled(true);
            expVal2SpinBox->setEnabled(true);
            break;
        default:
            expVal1SpinBox->setEnabled(false);
            expVal2SpinBox->setEnabled(false);
            break;
    }
}

/*
 *
 */
void Frontend::updateExpMode(void)
{
    int mode, val1, val2;

    diag.expMode(mode, val1, val2);

    disconnect(expModeCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setExpMode(int)));
    disconnect(expVal1SpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setExpMode(int)));
    disconnect(expVal2SpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setExpMode(int)));

    expModeCombo->setCurrentIndex(expModeCombo->findData(mode));
    expVal1SpinBox->setValue(val1);
    expVal2SpinBox->setValue(val2);

    setExpValsEnabled((ExpMode)mode);

    connect(expModeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(setExpMode(int)));
    connect(expVal1SpinBox, SIGNAL(valueChanged(int)), SLOT(setExpMode(int)));
    connect(expVal2SpinBox, SIGNAL(valueChanged(int)), SLOT(setExpMode(int)));
}

/*
 *
 */
void Frontend::setExpMode(int)
{
    int mode = expModeCombo->itemData(expModeCombo->currentIndex()).toInt();
    int val1 = expVal1SpinBox->value();
    int val2 = expVal2SpinBox->value();

    setExpValsEnabled((ExpMode)mode);

    qDebug() << "setting exp mode";

    diag.setExpMode(mode, val1, val2);
}

/*
 *
 */
void Frontend::storeExpModeButtonClicked(void)
{
    setEnabled(false);

    diag.storeExpMode();

    QTimer::singleShot(2000, this, SLOT(storeExpModeComplete()));
}

/*
 *
 */
void Frontend::storeExpModeComplete(void)
{
    updateExpMode();
    setEnabled(true);
}

/*
 *
 */
void Frontend::autoCalButtonClicked(void)
{
    setEnabled(false);
    autoCalButton->setText("Auto-calibrating...");

    diag.autoCalibrate();

    /* keyboard will disappear for a bit while it does its thing */
    QTimer::singleShot(2000, this, SLOT(autoCalComplete()));
}

/*
 *
 */
void Frontend::autoCalComplete(void)
{
    unsigned short vref = diag.vref();

    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Auto-calibration complete",
            "Auto-calibration finished, threshold is " +
            QString::number(vref) + ". Would you like auto-calibration to "
            "take place automatically every time the keyboard is powered up?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        diag.setVref(65535);
        diag.storeVref();
        diag.setVref(vref);
    }

    updateVref();
}

/*
 *
 */
void Frontend::storeVrefButtonClicked(void)
{
    diag.storeVref();
}

/*
 *
 */
void Frontend::vrefHelpButtonClicked(void)
{
    QMessageBox::information(this, "Voltage threshold help",
            "<p>The voltage threshold is the threshold that a key is "
            "considered pressed or not.</p>"
            "<p>If a valid set of scancodes is loaded (some keys "
            "must be set to PRESSED or RELEASED) then in most cases the "
            "threshold can be auto-calibrated when the keyboard "
            "is plugged in (this happens if 65535 is stored in the "
            "controller's EEPROM).</p>"
            "<p>Alternatively, a forced override value can "
            "be stored in the controller's EEPROM.</p>");
}

/*
 *
 */
void Frontend::bootloaderButtonClicked(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Enter bootloader",
            "Keyboard will stop responding until either it is unplugged then "
            "plugged back in, or the programming tool resets it. Really "
            "enter bootloader?", QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        setEnabled(false);
        diag.enterBootloader();
        QMessageBox::information(this, "Rebooted into bootloader",
                "The keyboard is ready to be flashed. Will now exit.");
        QTimer::singleShot(0, this, SLOT(close()));
    }
}

/*
 *
 */
void Frontend::bootloaderHelpButtonClicked(void)
{
    QMessageBox::information(this, "Bootloader help",
            "Enter the bootloader to allow Atmel Flip or dfu-programmer "
            "to flash new firmware.");
}

/*
 *
 */
void Frontend::buildMatrix(void)
{
    vector<vector<vector<unsigned char>>> scancodes = diag.scancodes();

    for (int i = (int)keyWidgets.size() - 1; i >= 0; i--)
    {
        for (size_t j = 0; j < keyWidgets[i].size(); j++)
            for (size_t k = 0; k < keyWidgets[i][j].size(); k++)
                delete keyWidgets[i][j][k];

        /* hack, rely on layers being last tabs to be added */
        QWidget *w = matrixTabWidget->widget(matrixTabWidget->count() - 1);
        matrixTabWidget->removeTab(matrixTabWidget->count() - 1);
        delete w;
    }
    keyWidgets.clear();

    int layers = scancodes.size();
    if (layers == 0)
        throw runtime_error("error: keyboard reports matrix with no layers");
    int cols = scancodes[0].size();
    if (cols == 0)
        throw runtime_error("error: keyboard reports matrix with no columns");
    int rows = scancodes[0][0].size();

    kbdMatrixSizeLabel->setText("Matrix size (" +
            QString::number(layers) + " layers):\t" +
            QString::number(cols) + " x " + QString::number(rows));

    keyWidgets = vector<vector<vector<Key *>>>(layers,
            vector<vector<Key *>>(cols, vector<Key *>(rows)));

    for (int layer = 0; layer < layers; layer++)
    {
        QGridLayout *layerGrid = new QGridLayout;
        layerGrid->setSpacing(0);

        for (int col = 0; col < cols; col++)
        {
            QLabel *colLabel = new QLabel("<b>" + QString::number(col + 1) +
                    "</b>");
            colLabel->setAlignment(Qt::AlignCenter);
            layerGrid->addWidget(colLabel, 0, col);

            for (int row = 0; row < rows; row++)
            {
                Key *key = new Key(diag, layer, col, row,
                        scancodes[layer][col][row]);
                keyWidgets[layer][col][row] = key;
                layerGrid->addWidget(key, row + 1, col);
            }
        }

        QWidget *layerWidget = new QWidget;
        layerWidget->setLayout(layerGrid);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(layerWidget);
        scrollArea->setWidgetResizable(true);

        matrixTabWidget->addTab(scrollArea, layer == 0 ?
                "Base Layer" : "Layer " + QString::number(layer));
    }
}

/*
 *
 */
void Frontend::buildLayerConditions(void)
{
    vector<LayerCondition> cnds = diag.layerConditions();

    /* clear existing widgets */
    QLayoutItem *item;
    while ((item = layerConditionsGrid->takeAt(0)) != NULL)
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
        layerConditionsGrid->addWidget(fnLabel, 0, i + 2);
    }
    for (int i = 0; i < count; i++)
    {
        QLabel *rowLabel = new QLabel("<b>" + QString::number(i + 1) + "</b>");
        layerConditionsGrid->addWidget(rowLabel, i + 1, 1);

        QCheckBox *fn1Check = new QCheckBox;
        fn1Check->setChecked(cnds[i].fn1Set());

        QCheckBox *fn2Check = new QCheckBox;
        fn2Check->setChecked(cnds[i].fn2Set());

        QCheckBox *fn3Check = new QCheckBox;
        fn3Check->setChecked(cnds[i].fn3Set());

        QLabel *arrowLabel = new QLabel(QChar(0x2192));

        QComboBox *layerCombo = new QComboBox;
        for (int i = 0; i < 4; i++)
            layerCombo->addItem(i == 0 ? "Base Layer" : "Layer " +
                    QString::number(i), i);
        layerCombo->setCurrentIndex(cnds[i].layer());

        layerConditionsGrid->addWidget(fn1Check, i + 1, 2);
        layerConditionsGrid->addWidget(fn2Check, i + 1, 3);
        layerConditionsGrid->addWidget(fn3Check, i + 1, 4);
        layerConditionsGrid->addWidget(arrowLabel, i + 1, 5);
        layerConditionsGrid->addWidget(layerCombo, i + 1, 6);

        layerConditionWatchers.push_back(new LayerConditionWatcher(diag,
                    i, fn1Check, fn2Check, fn3Check, layerCombo, this));
    }

    layerConditionsGrid->setRowStretch(layerConditionsGrid->rowCount(), 1);
    layerConditionsGrid->setColumnStretch(0, 4);
    for (int i = 1; i < layerConditionsGrid->columnCount(); i++)
        layerConditionsGrid->setColumnStretch(i, 1);
    layerConditionsGrid->setColumnStretch(layerConditionsGrid->columnCount(),
            4);
}

/*
 *
 */
void Frontend::buildColSkips(void)
{
    for (int i = 0; i < diag.cols(); i++)
    {
        QLabel *l = new QLabel(QString::number(i + 1));
        colSkipsGrid->addWidget(l, 0, i, Qt::AlignCenter);

        QCheckBox *cb = new QCheckBox;
        colSkipsCBs.push_back(cb);
        colSkipsGrid->addWidget(cb, 1, i, Qt::AlignCenter);

        connect(cb, SIGNAL(stateChanged(int)), SLOT(colSkipCBChanged(int)));
    }
}

/*
 *
 */
std::vector<bool> Frontend::colSkipsFromCBs(void)
{
    vector<bool> skips(diag.cols(), false);

    for (int i = 0; i < diag.cols(); i++)
        skips[i] = colSkipsCBs[i]->isChecked();

    return skips;
}

/*
 *
 */
void Frontend::updateColSkips(void)
{
    vector<bool> skips = diag.kbdColSkips();

    for (int i = 0; i < diag.cols(); i++)
        colSkipsCBs[i]->setChecked(skips[i]);
}

/*
 *
 */
void Frontend::colSkipCBChanged(int)
{
    cerr << "setting kbd col skips" << endl;

    diag.setKbdColSkips(colSkipsFromCBs());
}

/*
 *
 */
void Frontend::storeColSkipsButtonClicked(void)
{
    setEnabled(false);

    diag.storeKbdColSkips();

    QTimer::singleShot(2000, this, SLOT(storeColSkipsComplete()));
}

/*
 *
 */
void Frontend::storeColSkipsComplete(void)
{
    updateColSkips();
    setEnabled(true);
}

/*
 *
 */
void Frontend::colSkipsHelpButtonClicked(void)
{
    QMessageBox::information(this, "Column skips help",
            "<p>Some keyboards have unused columns. If these are connected to "
            "GND, as with some smaller Model F keyboards, the keyboard "
            "controller can eventually become damaged. Setting a column skip "
            "for those columns will mean they are never driven.</p>"
            "<p>This can also speed up the scan-rate for matrices which have "
            "valid columns but with no keys.</p>");
}

/*
 *
 */
void Frontend::queryKbdVersion(void)
{
    kbdVersionLabel->setText("Firmware version:\t\t" +
            QString::fromStdString(diag.version()));
}

/*
 *
 */
void Frontend::updateKeyStates(void)
{
    if (!isEnabled())
        return;
    if (keyWidgets.size() == 0)
        return;

    vector<vector<bool>> states = diag.keyStates();

    int cols = states.size();
    if (cols == 0)
        throw runtime_error("error: keyboard reports key states with no "
                "columns");
    int rows = states[0].size();

    for (size_t layer = 0; layer < keyWidgets.size(); layer++)
        for (int col = 0; col < cols; col++)
            for (int row = 0; row < rows; row++)
                keyWidgets[layer][col][row]->setPressed(states[col][row]);

    keyMon->updateStates(states);
}

/*
 *
 */
void Frontend::storeMatrixButtonClicked(void)
{
    setEnabled(false);
    storeMatrixButton->setText("Writing to EEPROM...");

    diag.storeScancodes();

    /* keyboard will disappear for a bit while it does its thing */
    QTimer::singleShot(2000, this, SLOT(storeMatrixComplete()));
}

/*
 *
 */
void Frontend::storeMatrixComplete(void)
{
    setEnabled(true);
    storeMatrixButton->setText("Store layout in EEPROM");
}

/*
 *
 */
void Frontend::loadMatrixButtonClicked(void)
{
    setEnabled(false);
    loadMatrixButton->setText("Reading from EEPROM...");

    diag.loadScancodes();

    /* keyboard will disappear for a bit while it does its thing */
    QTimer::singleShot(2000, this, SLOT(loadMatrixComplete()));
}

/*
 *
 */
void Frontend::loadMatrixComplete(void)
{
    setEnabled(true);
    loadMatrixButton->setText("Load layout from EEPROM");

    buildMatrix();
}

/*
 *
 */
void Frontend::importMatrixButtonClicked(void)
{
    QString filename = QFileDialog::getOpenFileName(this, "Import layout",
            QString(), "Layout files (*.l)");
    if (filename == QString())
        return;

    QString errStr;
    vector<vector<vector<unsigned char>>> scancodes;
    vector<LayerCondition> layerConditions;
    vector<bool> colSkips;
    int expMode, expVal1, expVal2;
    if (!importLayout(filename,
                scancodes,
                layerConditions,
                colSkips,
                expMode, expVal1, expVal2,
                errStr))
    {
        QMessageBox::critical(this, "Error",
                QString("Could not import layout: ") + errStr);
        return;
    }

    try
    {
        diag.setScancodes(scancodes);
        diag.setLayerConditions(layerConditions);
        diag.setKbdColSkips(colSkips);
        diag.setExpMode(expMode, expVal1, expVal2);
        buildMatrix();
        buildLayerConditions();
        updateColSkips();
        updateExpMode();
    }
    catch (std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString::fromUtf8(e.what()));
    }
}

/*
 *
 */
void Frontend::exportMatrixButtonClicked(void)
{
    QString filename = QFileDialog::getSaveFileName(this, "Export layout",
            "layout.l", "Layout files (*.l)");
    if (filename == QString())
        return;

    QString errStr;
    if (!exportLayout(filename,
                keyWidgets,
                layerConditionWatchers,
                colSkipsFromCBs(),
                expModeCombo->itemData(expModeCombo->currentIndex()).toInt(),
                expVal1SpinBox->value(),
                expVal2SpinBox->value(),
                errStr))
        QMessageBox::critical(this, "Error",
                QString("Could not export layout: ") + errStr);
}
