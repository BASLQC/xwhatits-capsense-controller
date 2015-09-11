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
#include "Macros.h"

using namespace std;

/*
 *
 */
Macros::Macros(DiagInterface &diag, const bool &kbdFocusEnabled,
        QWidget *parent):
    QWidget(parent),
    diag(diag),
    kbdFocusEnabled(kbdFocusEnabled)
{
    QPushButton *loadButton = new QPushButton("Load");
    QPushButton *storeButton = new QPushButton("Store");

    QFrame *separator1 = new QFrame;
    separator1->setFrameShape(QFrame::HLine);
    separator1->setFrameShadow(QFrame::Sunken);

    QFrame *separator2 = new QFrame;
    separator2->setFrameShape(QFrame::HLine);
    separator2->setFrameShadow(QFrame::Sunken);

    QPushButton *addButton = new QPushButton("Add");
    QPushButton *removeButton = new QPushButton("Remove");
    QPushButton *removeAllButton = new QPushButton("Remove all");

    bytesUsedLabel = new QLabel;

    QLabel *macroLabel = new QLabel("Macro:");
    macroCombo = new NonFocusedComboBox(kbdFocusEnabled);

    QVBoxLayout *buttonVBox = new QVBoxLayout;
    buttonVBox->addWidget(macroLabel);
    buttonVBox->addWidget(macroCombo);
    buttonVBox->addWidget(separator1);
    buttonVBox->addWidget(addButton);
    buttonVBox->addWidget(removeButton);
    buttonVBox->addWidget(removeAllButton);
    buttonVBox->addWidget(separator2);
    buttonVBox->addWidget(loadButton);
    buttonVBox->addWidget(storeButton);
    buttonVBox->addStretch();
    buttonVBox->addWidget(bytesUsedLabel);

    macroStack = new QStackedWidget;

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(macroStack);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addLayout(buttonVBox);
    hbox->addWidget(scrollArea);

    setLayout(hbox);

    connect(macroCombo, SIGNAL(currentIndexChanged(int)),
            macroStack, SLOT(setCurrentIndex(int)));
    connect(loadButton, SIGNAL(clicked()), SLOT(loadButtonClicked()));
    connect(storeButton, SIGNAL(clicked()), SLOT(storeButtonClicked()));
    connect(addButton, SIGNAL(clicked()), SLOT(addButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(removeButtonClicked()));
    connect(removeAllButton, SIGNAL(clicked()), SLOT(removeAllButtonClicked()));

    loadFromKeyboard();
}

/*
 *
 */
std::vector<unsigned char> Macros::asBytes(void)
{
    vector<unsigned char> v;

    for (int i = 0; i < macroStack->count(); i++)
    {
        Macro *m = qobject_cast<Macro *>(macroStack->widget(i));
        if (m == NULL)
            continue;

        vector<unsigned char> mV = m->asBytes();
        v.insert(v.end(), mV.begin(), mV.end());
    }

    return v;
}

/*
 *
 */
void Macros::loadFromKeyboard(void)
{
    bytesAvailable = diag.macroEEPROMSize();
    vector<unsigned char> bytes = diag.macroBytes();

    removeAllMacros();

    /* keep going until there's at most one byte left---should be terminator
     * byte 0xff unless something's gone wrong.
     */
    while (bytes.size() > 1 && bytes[0] != 0xff && macroStack->count() < 32)
    {
        Macro *m = new Macro(kbdFocusEnabled);
        bytes = m->fromBytes(bytes);

        macroCombo->addItem(m->name());
        macroStack->addWidget(m);
        connect(m, SIGNAL(nameChanged(QString)),
                SLOT(macroNameChanged(QString)));
        connect(m, SIGNAL(byteCountChanged()),
                SLOT(macroByteCountChanged()));
    }

    macroByteCountChanged();
}

/*
 *
 */
void Macros::storeInKeyboard(void)
{
    vector<unsigned char> v = asBytes();
    v.push_back(0xff);

    size_t availableSpace = diag.macroEEPROMSize();
    if (v.size() > availableSpace)
    {
        QMessageBox::warning(this, "Cannot write macros",
                "The size of macros (" + QString::number(v.size()) + " bytes) "
                "exceed the amount of available macro space (" +
                QString::number(availableSpace) + " bytes).");
        return;
    }

    diag.writeMacroBytes(v);
}

/*
 *
 */
void Macros::removeAllMacros(void)
{
    while (macroStack->count())
    {
        QWidget *w = macroStack->widget(0);
        macroStack->removeWidget(w);
        delete w;

    }
    
    while (macroCombo->count())
        macroCombo->removeItem(0);
}

/*
 *
 */
void Macros::loadButtonClicked(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Load macros?",
            "Reloading macros from the keyboard will overwrite any changes you "
            "have made here. Proceed?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
        loadFromKeyboard();
}

/*
 *
 */
void Macros::storeButtonClicked(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Store macros?",
            "This will overwrite any macros stored in the keyboard. Proceed?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
        storeInKeyboard();
}

/*
 *
 */
void Macros::addButtonClicked(void)
{
    if (macroStack->count() >= 32)
    {
        QMessageBox::warning(this, "Cannot add macro",
                "Cannot add any more macros; limit is 32.");
        return;
    }

    Macro *m = new Macro(kbdFocusEnabled);
    macroCombo->addItem("Macro " + QString::number(macroStack->count() + 1));
    macroStack->addWidget(m);
    connect(m, SIGNAL(nameChanged(QString)), SLOT(macroNameChanged(QString)));
    connect(m, SIGNAL(byteCountChanged()), SLOT(macroByteCountChanged()));

    macroCombo->setCurrentIndex(macroCombo->count() - 1);

    macroByteCountChanged();
}

/*
 *
 */
void Macros::removeButtonClicked(void)
{
    int currIdx = macroCombo->currentIndex();
    if (currIdx == -1)
        return;

    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Remove macro?",
            "Are you sure you want to remove " +
            macroCombo->currentText() + "?",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        QWidget *w = macroStack->widget(currIdx);
        macroStack->removeWidget(w);
        delete w;

        macroCombo->removeItem(currIdx);

        macroByteCountChanged();
    }
}

/*
 *
 */
void Macros::removeAllButtonClicked(void)
{
    if (macroStack->count() == 0)
        return;

    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Remove all macros?",
            "Are you sure you want to remove all macros?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
        removeAllMacros();

    macroByteCountChanged();
}

/*
 *
 */
void Macros::macroNameChanged(QString name)
{
    QWidget *w = qobject_cast<QWidget *>(QObject::sender());
    if (w == NULL)
        return;

    int index = macroStack->indexOf(w);
    if (index == -1)
        return;

    macroCombo->setItemText(index, name);
}

/*
 *
 */
void Macros::macroByteCountChanged(void)
{
    int used = 1; // terminator byte

    for (int i = 0; i < macroStack->count(); i++)
    {
        Macro *m = qobject_cast<Macro *>(macroStack->widget(i));
        if (m == NULL)
            continue;

        used += m->asBytes().size();
    }

    bytesUsedLabel->setText(QString::number(used) + "/" +
            QString::number(bytesAvailable) + " bytes");
}
