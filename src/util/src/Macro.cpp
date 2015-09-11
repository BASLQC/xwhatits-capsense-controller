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
#include "Macro.h"

#define MACRO_HEADER_SIZE 5

using namespace std;

/*
 *
 */
Macro::Macro(const bool &kbdFocusEnabled, QWidget *parent):
    QWidget(parent),
    kbdFocusEnabled(kbdFocusEnabled)
{
    QLabel *triggerLabel = new QLabel("Trigger on:");
    triggerScancodeCombo = new NonFocusedComboBox(&kbdFocusEnabled);
    populateScancodeCombo(triggerScancodeCombo);
    connect(triggerScancodeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(triggerNameChanged(int)));

    QLabel *triggerSetModsLabel = new QLabel("With mods:");
    triggerSetMods = new MacroTriggerModsWidget;
    connect(triggerSetMods, SIGNAL(changed()), SLOT(triggerNameChanged()));

    QLabel *triggerExcludedModsLabel = new QLabel("Excluding:");
    triggerExcludedMods = new MacroModsWidget;
    connect(triggerExcludedMods, SIGNAL(changed()), SLOT(triggerNameChanged()));

    retainPressedModsCB = new QCheckBox("Retain set mods on exit");

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(triggerLabel, 0, 0);
    grid->addWidget(triggerScancodeCombo, 0, 1);
    grid->setColumnStretch(2, 1);
    grid->addWidget(triggerSetModsLabel, 0, 3);
    grid->addWidget(triggerSetMods, 0, 4);
    grid->addWidget(triggerExcludedModsLabel, 1, 3);
    grid->addWidget(triggerExcludedMods, 1, 4);
    grid->addWidget(retainPressedModsCB, 1, 0, 1, 3);

    QPushButton *addRowButton = new QPushButton("+");
    connect(addRowButton, SIGNAL(clicked()), SLOT(addButtonClicked()));

    table = new QTableWidget(1, 2);
    table->setSizePolicy(QSizePolicy::MinimumExpanding,
            QSizePolicy::MinimumExpanding);
    table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);
    table->setHorizontalScrollMode(QTableWidget::ScrollPerPixel);
    table->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
    table->setHorizontalHeaderLabels(QStringList() << "Command" << "Value");
    table->horizontalHeader()->
        setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->
        setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setSpan(0, 0, 1, 2);
    table->setCellWidget(0, 0, addRowButton);
    table->setSelectionMode(QTableWidget::NoSelection);
    addContextMenu(table);
    addContextMenu(table->verticalHeader());

    bytesUsedLabel = new QLabel;

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(grid);
    vbox->addWidget(table);
    vbox->addWidget(bytesUsedLabel);

    setLayout(vbox);

    updateByteCountLabel();
}

/*
 *
 */
QString Macro::name(void)
{
    QString s;

    s += triggerScancodeCombo->currentText();

    QString withMods = triggerSetMods->stateAsText();
    QString excludeMods = triggerExcludedMods->stateAsText();

    if (withMods.size())
        s += " with (" + withMods + ")";
    if (excludeMods.size())
        s += " excluding (" + excludeMods + ")";

    return s;
}

/*
 *
 */
std::vector<unsigned char> Macro::asBytes(void)
{
    vector<unsigned char> v;

    unsigned char states, mask;
    calculateTriggerModBytes(&states, &mask);

    unsigned char sc = triggerScancodeCombo->currentData().toInt();
    if (sc == 0x00 || sc == 0xff)
        return v;

    v.push_back(sc);
    v.push_back(states);
    v.push_back(mask);
    v.push_back(0); // num make bytes
    v.push_back(0); // num break bytes

    int numMakeBytes = 0;
    int numBreakBytes = -1;
    bool nextCommandPushMods = false;
    for (int i = 0; i < table->rowCount() - 1; i++)
    {
        int flags;

        vector<unsigned char> rV = rowAsBytes(i, &flags);

        if (numBreakBytes == -1 && (flags & rowWaitBreak))
        {
            numBreakBytes = 0;
            continue;
        }
        else if (flags & rowPushMods)
        {
            nextCommandPushMods = true;
            continue;
        }

        if (numBreakBytes == -1)
            numMakeBytes += rV.size();
        else
            numBreakBytes += rV.size();

        if (nextCommandPushMods && rV.size())
        {
            nextCommandPushMods = false;
            rV[0] |= mcPushMods;
        }

        v.insert(v.end(), rV.begin(), rV.end());
    }

    v[3] = numMakeBytes;

    if (numBreakBytes != -1)
        v[4] = numBreakBytes;
    if (retainPressedModsCB->isChecked())
        v[4] |= (1 << 7);

    return v;
}

/*
 * returns bytes that weren't used for this macro
 */
std::vector<unsigned char> Macro::fromBytes(std::vector<unsigned char> bytes)
{
    /* should be room for at least the header */
    if (bytes.size() < MACRO_HEADER_SIZE)
        return vector<unsigned char>();

    removeAllRows();

    unsigned char sc            = bytes[0];
    unsigned char modStates     = bytes[1];
    unsigned char modMask       = bytes[2];
    unsigned char numMakeBytes  = bytes[3];
    unsigned char numBreakBytes = bytes[4];

    /* check enough room for specified make/break bytes */
    if (bytes.size() < (size_t)MACRO_HEADER_SIZE + numMakeBytes + numBreakBytes)
        return vector<unsigned char>();

    triggerScancodeCombo->setCurrentIndex(triggerScancodeCombo->
            findData((int)sc));
    setTriggerModsFromBytes(modStates, modMask);

    retainPressedModsCB->setChecked(numBreakBytes & (1 << 7));
    numBreakBytes &= ~(1 << 7);

    /* first create make commands */
    auto i   = bytes.begin() + MACRO_HEADER_SIZE;
    auto end = i + numMakeBytes;
    while (i < end)
        i = appendRowFromBytes(i, end);

    if (numBreakBytes != 0)
    {
        /* add wait-for-break row */
        insertRowAt(table->rowCount() - 1, -1);

        /* create break commands */
        i   = end;
        end = i + numBreakBytes;
        while (i < end)
            i = appendRowFromBytes(i, end);
    }

    updateByteCountLabel();

    return vector<unsigned char>(bytes.begin() + MACRO_HEADER_SIZE +
            numMakeBytes + numBreakBytes, bytes.end());
}

/*
 *
 */
void Macro::removeAllRows(void)
{
    while (table->rowCount() > 1)
        table->removeRow(0);
}

/*
 *
 */
void Macro::setTriggerModsFromBytes(unsigned char state, unsigned char mask)
{
    triggerSetMods->setStateFromBytes(state, mask);
    triggerExcludedMods->setStateOfMods(~state & mask);
}

/*
 * returns iterator to next command
 */
std::vector<unsigned char>::iterator Macro::appendRowFromBytes(
        std::vector<unsigned char>::iterator i,
        std::vector<unsigned char>::iterator end)
{
    if (i >= end)
        return end;

    int row = table->rowCount() - 1;

    int cmd = *i++;
    if (cmd & mcPushMods)
    {
        cmd &= ~mcPushMods;
        insertRowAt(row++, mcPushMods);
    }

    insertRowAt(row, cmd);
    if (createValueCell(row, cmd, *i))
        i++;

    return i;
}

/*
 *
 */
void Macro::calculateTriggerModBytes(unsigned char *states, unsigned char *mask)
{
    unsigned char setEithers  = triggerSetMods->stateOfEithers();
    unsigned char setMods     = triggerSetMods->stateOfMods();
    unsigned char excludeMods = triggerExcludedMods->stateOfMods();

    *states  = setMods;
    *mask    = setMods;

    *states |=  (setEithers | setEithers << 4);
    *mask   |=  (setEithers & 0xf);
    *mask   &= ~(setEithers << 4);

    *states &= ~(excludeMods);
    *mask   |=   excludeMods;
}

/*
 *
 */
std::vector<unsigned char> Macro::rowAsBytes(int row, int *flags)
{
    vector<unsigned char> v;

    QComboBox *cb = qobject_cast<QComboBox *>(table->cellWidget(row, 0));
    if (cb == NULL)
        return v;

    int command = cb->currentData().toInt();
    if (command == mcPushMods)
    {
        *flags = rowPushMods;
        return v;
    }
    else if (command == -1)
    {
        *flags = rowWaitBreak;
        return v;
    }
    else
        *flags = 0;

    v.push_back(command);
    switch (command)
    {
        case mcPress:
            v.push_back(valueByteScancode(row));
            break;
        case mcAssignMods:
        case mcSetMods:
        case mcClearMods:
        case mcToggleMods:
            v.push_back(valueByteMods(row));
            break;
        case mcDelay:
            v.push_back(valueByteDelay(row));
            break;
    }

    return v;
}

/*
 *
 */
unsigned char Macro::valueByteScancode(int row)
{
    QComboBox *cb = qobject_cast<QComboBox *>(table->cellWidget(row, 1));
    if (cb == NULL)
        return 0x00;
    return cb->currentData().toInt();
}

/*
 *
 */
unsigned char Macro::valueByteMods(int row)
{
    MacroModsWidget *w =
        qobject_cast<MacroModsWidget *>(table->cellWidget(row, 1));
    if (w == NULL)
        return 0x00;
    return w->stateOfMods();
}

/*
 *
 */
unsigned char Macro::valueByteDelay(int row)
{
    QSpinBox *sb = qobject_cast<QSpinBox *>(table->cellWidget(row, 1));
    if (sb == NULL)
        return 0x00;

    /* stored as tens of ms */
    return sb->value() / 10;
}

/*
 *
 */
void Macro::triggerNameChanged(int)
{
    emit nameChanged(name());
}

/*
 *
 */
void Macro::addContextMenu(QWidget *w)
{
    w->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(w, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showTableContextMenu(QPoint)));
}

/*
 *
 */
void Macro::addButtonClicked(void)
{
    insertRowAt(table->rowCount() - 1);
    updateByteCountLabel();
}

/*
 *
 */
void Macro::insertRowAt(int row, int cmd)
{
    table->insertRow(row);

    NonFocusedComboBox *cb = new NonFocusedComboBox(kbdFocusEnabled);
    populateCommandCombo(cb);
    cb->setCurrentIndex(cb->findData(cmd));
    connect(cb, SIGNAL(currentIndexChanged(int)),
            SLOT(commandComboIndexChanged(int)));
    table->setCellWidget(row, 0, cb);
    addContextMenu(cb);
    createNullValueCell(row);
}

/*
 *
 */
void Macro::populateCommandCombo(QComboBox *cb)
{
    for (int i = mcNOP; i <= mcDelay; i++)
    {
        QString s;
        switch (i)
        {
            case mcNOP:
                s = "(do nothing)";
                break;
            case mcPress:
                s = "Press";
                break;
            case mcAssignMods:
                s = "Assign mods";
                break;
            case mcSetMods:
                s = "Set mods";
                break;
            case mcClearMods:
                s = "Clear mods";
                break;
            case mcToggleMods:
                s = "Toggle mods";
                break;
            case mcPopMods:
                s = "Pop mods";
                break;
            case mcPopAllMods:
                s = "Pop all mods";
                break;
            case mcDelay:
                s = "Delay";
                break;
        }

        cb->addItem(s, i);
    }
    cb->insertItem(1, "Push Mods", (int)mcPushMods);
    cb->addItem("Wait for key-up", -1);
}

/*
 *
 */
void Macro::populateScancodeCombo(QComboBox *cb)
{
    for (unsigned char i = 0x00; i < 0xff; i++)
    {
        string scS = scancodeName(i);
        if (scS.empty())
            continue;

        cb->addItem(QString::fromStdString(scS), i);
    }
}

/*
 * not ideal, but much simpler than maintaining row numbers through
 * QSignalMapper and keeping them updated.
 */
int Macro::findWidgetRow(QWidget *w)
{
    for (int i = 0; i < table->rowCount() - 1; i++)
        if (table->cellWidget(i, 0) == w)
            return i;
    return -1;
}

/*
 * returns true if a non-null value cell was created
 */
bool Macro::createValueCell(int row, int commandType, int value)
{
    if (table->cellWidget(row, 1))
        table->removeCellWidget(row, 1);
    if (table->item(row, 1))
        delete table->takeItem(row, 1);

    switch (commandType)
    {
        case mcPress:
            createScancodeValueCell(row, value);
            return true;
        case mcAssignMods:
        case mcSetMods:
        case mcClearMods:
        case mcToggleMods:
            createModsValueCell(row, value);
            return true;
        case mcDelay:
            createDelayValueCell(row, value);
            return true;
        default:
            createNullValueCell(row);
            return false;
    }
}

/*
 *
 */
void Macro::createNullValueCell(int row)
{
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, 1, item);
}

/*
 *
 */
void Macro::createScancodeValueCell(int row, int value)
{
    NonFocusedComboBox *cb = new NonFocusedComboBox(kbdFocusEnabled);
    populateScancodeCombo(cb);
    cb->setCurrentIndex(cb->findData(value));
    addContextMenu(cb);

    table->setCellWidget(row, 1, cb);
}

/*
 *
 */
void Macro::createModsValueCell(int row, int value)
{
    MacroModsWidget *mw = new MacroModsWidget;
    mw->setStateOfMods(value);
    table->setCellWidget(row, 1, mw);
    addContextMenu(mw);
}

/*
 *
 */
void Macro::createDelayValueCell(int row, int value)
{
    NonFocusedSpinBox *sb = new NonFocusedSpinBox(kbdFocusEnabled);
    sb->setMinimum(0);
    sb->setMaximum(2550);
    sb->setSingleStep(10);
    sb->setSuffix(" ms");
    sb->setValue(value * 10);
    addContextMenu(sb);

    table->setCellWidget(row, 1, sb);
}

/*
 *
 */
void Macro::showTableContextMenu(QPoint pos)
{
    /* find sending widget */
    QWidget *w = qobject_cast<QWidget *>(QObject::sender());
    if (w == NULL)
        return;
    QAbstractScrollArea *sa = qobject_cast<QAbstractScrollArea *>(w);

    /* map to global coords */
    if (sa == NULL)
        pos = table->viewport()->mapFromGlobal(w->mapToGlobal(pos));
    else
        pos = table->viewport()->mapFromGlobal(sa->viewport()->
                mapToGlobal(pos));

    /* correct point for LHS of table viewport, in case click was on vertical
     * header
     */
    pos.setX(max(pos.x(), 0));

    QModelIndex index = table->indexAt(pos);
    if (!index.isValid() || index.row() == table->rowCount() - 1)
        return;

    contextMenuRow = index.row();

    QMenu *menu = new QMenu(this);

    menu->addAction("&Insert row", this, SLOT(contextMenuInsertTriggered()));
    menu->addAction("&Delete row", this, SLOT(contextMenuDeleteTriggered()));

    menu->popup(table->viewport()->mapToGlobal(pos));
}

/*
 *
 */
void Macro::commandComboIndexChanged(int)
{
    QComboBox *cb = qobject_cast<QComboBox *>(QObject::sender());
    if (cb == NULL)
        return;

    int commandType = cb->currentData().toInt();

    int row = findWidgetRow(cb);
    if (row >= 0 && row < table->rowCount() - 1)
        createValueCell(row, commandType);

    resizeCells();
    updateByteCountLabel();
}

/*
 * yes, this is a horribly inefficient way to do it :)
 */
void Macro::updateByteCountLabel(void)
{
    bytesUsedLabel->setText(QString::number(asBytes().size()) + " bytes used");
    emit byteCountChanged();
}

/*
 *
 */
void Macro::contextMenuInsertTriggered(void)
{
    if (contextMenuRow >= 0 && contextMenuRow < table->rowCount() - 1)
    {
        insertRowAt(contextMenuRow);
        updateByteCountLabel();
    }
}

/*
 *
 */
void Macro::contextMenuDeleteTriggered(void)
{
    if (contextMenuRow >= 0 && contextMenuRow < table->rowCount() - 1)
    {
        table->removeRow(contextMenuRow);
        updateByteCountLabel();
    }
}

/*
 *
 */
void Macro::resizeCells(void)
{
    table->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

/*
 *
 */
MacroTriggerModsWidget::MacroTriggerModsWidget(QWidget *parent):
    QWidget(parent)
{
    QGridLayout *grid = new QGridLayout;

    eitherCtrlCB = new QCheckBox("Ctrl");
    connect(eitherCtrlCB, SIGNAL(toggled(bool)),
            SLOT(eitherCtrlCBToggled(bool)));
    grid->addWidget(eitherCtrlCB, 0, 0);

    eitherShiftCB = new QCheckBox("Shift");
    connect(eitherShiftCB, SIGNAL(toggled(bool)),
            SLOT(eitherShiftCBToggled(bool)));
    grid->addWidget(eitherShiftCB, 0, 1);

    eitherAltCB = new QCheckBox("Alt");
    connect(eitherAltCB, SIGNAL(toggled(bool)),
            SLOT(eitherAltCBToggled(bool)));
    grid->addWidget(eitherAltCB, 0, 2);

    eitherGUICB = new QCheckBox("GUI");
    connect(eitherGUICB, SIGNAL(toggled(bool)),
            SLOT(eitherGUICBToggled(bool)));
    grid->addWidget(eitherGUICB, 0, 3);

    for (int i = smbLCtrl; i <= smbRGUI; i++)
    {
        modCBs[i] = new QCheckBox(QString::fromStdString(scancodeModName(i)));
        connect(modCBs[i], SIGNAL(toggled(bool)), SLOT(modCBToggled(bool)));
        grid->addWidget(modCBs[i], 1, i);
    }

    setLayout(grid);
}

/*
 *
 */
QString MacroTriggerModsWidget::stateAsText(void)
{
    QString s;

    if (eitherCtrlCB->isChecked())
        s += "Ctrl, ";
    if (eitherShiftCB->isChecked())
        s += "Shift, ";
    if (eitherAltCB->isChecked())
        s += "Alt, ";
    if (eitherGUICB->isChecked())
        s += "GUI, ";

    for (int i = 0; i < 8; i++)
        if (modCBs[i]->checkState() == Qt::Checked)
            s += QString::fromStdString(scancodeModName(i)) + ", ";

    if (s.size())
    {
        s = s.trimmed();
        if (s[s.size() - 1] == ',')
            s.truncate(s.size() - 1);
    }

    return s;
}

/*
 *
 */
int MacroTriggerModsWidget::stateOfEithers(void)
{
    int val = 0;
    if (eitherCtrlCB->isChecked())
        val |= (1 << 0);
    if (eitherShiftCB->isChecked())
        val |= (1 << 1);
    if (eitherAltCB->isChecked())
        val |= (1 << 2);
    if (eitherGUICB->isChecked())
        val |= (1 << 3);

    return val;
}

/*
 *
 */
int MacroTriggerModsWidget::stateOfMods(void)
{
    int val = 0;
    for (int i = 0; i < 8; i++)
        if (modCBs[i]->isChecked())
            val |= (1 << i);
    return val;
}

/*
 *
 */
void MacroTriggerModsWidget::setStateFromBytes(unsigned char state,
        unsigned char mask)
{
    blockCBSignals(true);

    eitherCtrlCB->setChecked(false);
    eitherShiftCB->setChecked(false);
    eitherAltCB->setChecked(false);
    eitherGUICB->setChecked(false);

    unsigned char basicCBs = (state & mask);
    for (int i = 0; i < 8; i++)
        modCBs[i]->setChecked(basicCBs & (1 << i));

    blockCBSignals(false);

    if ((state & ((state & ~mask) >> 4)) & (1 << 0))
        eitherCtrlCB->setChecked(true);
    if ((state & ((state & ~mask) >> 4)) & (1 << 1))
        eitherShiftCB->setChecked(true);
    if ((state & ((state & ~mask) >> 4)) & (1 << 2))
        eitherAltCB->setChecked(true);
    if ((state & ((state & ~mask) >> 4)) & (1 << 3))
        eitherGUICB->setChecked(true);
}

/*
 *
 */
void MacroTriggerModsWidget::blockCBSignals(bool value)
{
    eitherCtrlCB->blockSignals(value);
    eitherShiftCB->blockSignals(value);
    eitherAltCB->blockSignals(value);
    eitherGUICB->blockSignals(value);
    for (int i = 0; i < 8; i++)
        modCBs[i]->blockSignals(value);
}

/*
 *
 */
void MacroTriggerModsWidget::eitherCBToggled(bool state, int lhsMod)
{
    if (lhsMod < 0 || lhsMod > 7)
        return;

    blockCBSignals(true);
    if (state)
    {
        modCBs[lhsMod    ]->setCheckState(Qt::PartiallyChecked);
        modCBs[lhsMod + 4]->setCheckState(Qt::PartiallyChecked);
    }
    else
    {
        modCBs[lhsMod    ]->setChecked(false);
        modCBs[lhsMod    ]->setTristate(false);
        modCBs[lhsMod + 4]->setChecked(false);
        modCBs[lhsMod + 4]->setTristate(false);
    }
    blockCBSignals(false);

    modCBs[lhsMod    ]->setEnabled(!state);
    modCBs[lhsMod + 4]->setEnabled(!state);

    emit changed();
}

/*
 *
 */
void MacroTriggerModsWidget::eitherCtrlCBToggled(bool state)
{
    eitherCBToggled(state, smbLCtrl);
}

/*
 *
 */
void MacroTriggerModsWidget::eitherShiftCBToggled(bool state)
{
    eitherCBToggled(state, smbLShift);
}

/*
 *
 */
void MacroTriggerModsWidget::eitherAltCBToggled(bool state)
{
    eitherCBToggled(state, smbLAlt);
}

/*
 *
 */
void MacroTriggerModsWidget::eitherGUICBToggled(bool state)
{
    eitherCBToggled(state, smbLGUI);
}

/*
 *
 */
void MacroTriggerModsWidget::modCBToggled(bool)
{
    emit changed();
}

/*
 *
 */
MacroModsWidget::MacroModsWidget(QWidget *parent):
    QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout;

    for (int i = smbLCtrl; i <= smbRGUI; i++)
    {
        modCBs[i] = new QCheckBox(QString::fromStdString(scancodeModName(i)));
        connect(modCBs[i], SIGNAL(toggled(bool)), SLOT(modCBToggled(bool)));
        hbox->addWidget(modCBs[i]);
    }

    QPushButton *allButton = new QPushButton("Set/clear all");
    connect(allButton, SIGNAL(clicked()), SLOT(allButtonClicked()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    vbox->addWidget(allButton);

    setLayout(vbox);
}

/*
 *
 */
QString MacroModsWidget::stateAsText(void)
{
    QString s;

    for (int i = 0; i < 8; i++)
        if (modCBs[i]->isChecked())
            s += QString::fromStdString(scancodeModName(i)) + ", ";

    if (s.size())
    {
        s = s.trimmed();
        if (s[s.size() - 1] == ',')
            s.truncate(s.size() - 1);
    }

    return s;
}

/*
 *
 */
int MacroModsWidget::stateOfMods(void)
{
    int val = 0;
    for (int i = 0; i < 8; i++)
        if (modCBs[i]->isChecked())
            val |= (1 << i);
    return val;
}

/*
 *
 */
void MacroModsWidget::setStateOfMods(int state)
{
    for (int i = 0; i < 8; i++)
        modCBs[i]->setChecked(state & (1 << i));
}

/*
 *
 */
void MacroModsWidget::allButtonClicked(void)
{
    /* if all clear, then set all */
    bool allClear = true;
    for (int i = 0; i < 8; i++)
    {
        if (modCBs[i]->isChecked())
        {
            allClear = false;
            break;
        }
    }

    for (int i = 0; i < 8; i++)
        modCBs[i]->setChecked(allClear);

    emit changed();
}

/*
 *
 */
void MacroModsWidget::modCBToggled(bool)
{
    emit changed();
}
