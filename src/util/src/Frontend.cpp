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
    QMainWindow(parent),
    diag(diag),
    kbdFocusEnabled(false)
{
    setWindowTitle("IBM Capsense USB Util");

    buildMenuBar();
    setStatusBar(new StatusBar(diag));

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
     * control section
     */
    QGroupBox *controlGroup = new QGroupBox("Control");

    QToolButton *haltScanHelpButton = new QToolButton;
    haltScanHelpButton->setText("?");
    haltScanHelpButton->setToolTip("Help");
    QPushButton *haltScanButton = new QPushButton("Emergency halt");
    haltScanButton->setCheckable(true);
    haltScanButton->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Expanding);

    QGridLayout *controlGrid = new QGridLayout;
    controlGrid->addWidget(haltScanButton, 0, 0, 3, 1);
    controlGrid->addWidget(haltScanHelpButton, 1, 1);

    controlGroup->setLayout(controlGrid);

    /*
     * subtabs
     */
    VoltageThreshold *vref = new VoltageThreshold(diag, kbdFocusEnabled);
    expansionHeader = new ExpansionHeader(diag, kbdFocusEnabled);
    layerConditions = new LayerConditions(diag, kbdFocusEnabled);
    colSkips = new ColSkips(diag);
    macros = new Macros(diag, kbdFocusEnabled);

    /*
     * matrix
     */
    QTimer *keyStatesTimer = new QTimer(this);
    keyStatesTimer->start(0);

    /*
     * config tabs
     */
    mainTabWidget = new QTabWidget;

    mainTabWidget->addTab(new PaddedBox(vref), "Voltage Threshold");
    mainTabWidget->addTab(new PaddedBox(expansionHeader), "Expansion Header");
    mainTabWidget->addTab(new PaddedBox(layerConditions), "Layer Conditions");
    mainTabWidget->addTab(new PaddedBox(colSkips), "Column Skips");
    mainTabWidget->addTab(macros, "Macros");

    /*
     * main layout
     */
    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->addWidget(stateGroup);
    hbox1->addWidget(controlGroup);
    hbox1->setSizeConstraint(QLayout::SetMinimumSize);

    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addLayout(hbox1, 1);
    vbox1->addWidget(mainTabWidget, 10);
    vbox1->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget *w = new QWidget;
    w->setLayout(vbox1);
    setCentralWidget(w);

    QTimer::singleShot(0, this, SLOT(buildMatrix()));

    connect(haltScanButton, SIGNAL(toggled(bool)),
            SLOT(haltScanButtonToggled(bool)));
    connect(haltScanHelpButton, SIGNAL(clicked()),
            SLOT(haltScanHelpButtonClicked()));
    connect(colSkips, SIGNAL(skipsChanged()), SLOT(setKeyColsEnabled()));
    connect(keyStatesTimer, SIGNAL(timeout()), SLOT(updateKeyStates()));
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
void Frontend::buildMenuBar(void)
{
    QMenuBar *menuBar = new QMenuBar;

    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Import", this, SLOT(importConfig()));
    fileMenu->addAction("&Export", this, SLOT(exportConfig()));
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(close()))->
        setMenuRole(QAction::QuitRole);

    QMenu *toolsMenu = menuBar->addMenu("&Tools");
    toolsMenu->addAction("Enter &bootloader", this, SLOT(enterBootloader()));

    QAction *guiKbdLockAction = toolsMenu->addAction("&GUI keyboard unlock");
    guiKbdLockAction->setCheckable(true);
    connect(guiKbdLockAction, SIGNAL(toggled(bool)),
            SLOT(guiKbdLockToggled(bool)));

    setMenuBar(menuBar);
}

/*
 *
 */
void Frontend::enterBootloader(void)
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
        QTimer::singleShot(0, this, SLOT(close()));
    }
}

/*
 *
 */
void Frontend::guiKbdLockToggled(bool checked)
{
    kbdFocusEnabled = checked;
}

/*
 *
 */
void Frontend::haltScanButtonToggled(bool checked)
{
    if (checked)
    {
        cerr << "halting scanning..." << endl;
        diag.setScanEnabled(false);
    }
    else
    {
        cerr << "resuming scanning..." << endl;
        diag.setScanEnabled(true);
    }
}

/*
 *
 */
void Frontend::haltScanHelpButtonClicked(void)
{
    QMessageBox::information(this, "Emergency halt help",
            "Depressing this button immediately stops the keyboard from "
            "scanning and emitting keycodes in case of an incorrect voltage "
            "threshold causing rapid keypresses. Pressing the button again "
            "will resume scanning.");
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
        QWidget *w = mainTabWidget->widget(mainTabWidget->count() - 1);
        mainTabWidget->removeTab(mainTabWidget->count() - 1);
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

    keyWidgets = vector<vector<vector<Key *>>>(layers,
            vector<vector<Key *>>(cols, vector<Key *>(rows)));

    for (int layer = 0; layer < layers; layer++)
    {
        QGridLayout *layerGrid = new QGridLayout;
        layerGrid->setRowStretch(0, 1);
        layerGrid->setSpacing(0);

        for (int col = 0; col < cols; col++)
        {
            QLabel *colLabel = new QLabel("<b>" + QString::number(col + 1) +
                    "</b>");
            colLabel->setAlignment(Qt::AlignCenter);
            layerGrid->addWidget(colLabel, 1, col);

            for (int row = 0; row < rows; row++)
            {
                Key *key = new Key(diag, kbdFocusEnabled, layer, col, row,
                        scancodes[layer][col][row]);
                keyWidgets[layer][col][row] = key;
                layerGrid->addWidget(key, row + 2, col);
            }
        }
        layerGrid->setRowStretch(layerGrid->rowCount(), 1);

        QWidget *layerWidget = new QWidget;
        layerWidget->setLayout(layerGrid);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(layerWidget);
        scrollArea->setWidgetResizable(true);

        mainTabWidget->addTab(scrollArea, layer == 0 ?
                "Base Layer" : "Layer " + QString::number(layer));
    }

    setKeyColsEnabled();
}

/*
 *
 */
void Frontend::setKeyColsEnabled(void)
{
    vector<bool> skips = colSkips->state();
    for (size_t layer = 0; layer < keyWidgets.size(); layer++)
        for (int col = 0; col < diag.cols(); col++)
            for (int row = 0; row < diag.rows(); row++)
                keyWidgets[layer][col][row]->setEnabled(!skips[col]);
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
void Frontend::importConfig(void)
{
    QString filename = QFileDialog::getOpenFileName(this, "Import layout",
            QString(), "Layout files (*.l)");
    if (filename == QString())
        return;

    QString errStr;
    vector<vector<vector<unsigned char>>> scancodes;
    vector<LayerCondition> lcs;
    vector<bool> cs;
    int expMode, expVal1, expVal2;
    vector<unsigned char> macroBytes;
    if (!importLayout(filename,
                scancodes,
                lcs,
                cs,
                expMode, expVal1, expVal2,
                macroBytes,
                errStr))
    {
        QMessageBox::critical(this, "Error",
                QString("Could not import layout: ") + errStr);
        return;
    }

    try
    {
        diag.setScancodes(scancodes);
        diag.setLayerConditions(lcs);
        diag.setKbdColSkips(cs);
        diag.setExpMode(expMode, expVal1, expVal2);
        diag.writeMacroBytes(macroBytes);
        buildMatrix();
        layerConditions->rebuild();
        colSkips->update();
        expansionHeader->updateMode();
        macros->loadFromKeyboard();
    }
    catch (std::exception &e)
    {
        QMessageBox::critical(this, "Error", QString::fromUtf8(e.what()));
    }
}

/*
 *
 */
void Frontend::exportConfig(void)
{
    QString filename = QFileDialog::getSaveFileName(this, "Export layout",
            "layout.l", "Layout files (*.l)");
    if (filename == QString())
        return;

    QString errStr;
    if (!exportLayout(filename,
                keyWidgets,
                layerConditions->layerConditionWatchers,
                colSkips->state(),
                expansionHeader->mode(),
                expansionHeader->val1(),
                expansionHeader->val2(),
                macros->asBytes(),
                errStr))
        QMessageBox::critical(this, "Error",
                QString("Could not export layout: ") + errStr);
}
