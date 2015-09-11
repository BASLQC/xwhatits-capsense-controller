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
#include <QApplication>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
#   include <QCommandLineParser>
#endif
#include <QMessageBox>
#include <QInputDialog>
#include <iomanip>
#include "DiagInterface.h"
#include "Frontend.h"

using namespace std;

/*
 *
 */
int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("ibm_capsense_usb_util");
    QApplication::setApplicationVersion(VER);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption listDevsOpt(QStringList() << "l" << "list",
            "list attached devices");
    parser.addOption(listDevsOpt);

    QCommandLineOption devOpt(QStringList() << "d" << "device",
            "use specified device (instead of first found)", "devpath");
    parser.addOption(devOpt);

    QCommandLineOption infoOpt(QStringList() << "i" << "info",
            "show controller info");
    parser.addOption(infoOpt);

    QCommandLineOption vrefOpt(QStringList() << "r" << "vref",
            "show current vref");
    parser.addOption(vrefOpt);

    QCommandLineOption kbdStateOpt(QStringList() << "k" << "key-states",
            "show keyboard state");
    parser.addOption(kbdStateOpt);

    QCommandLineOption scancodesOpt(QStringList() << "s" << "scancodes",
            "show keyboard scancodes");
    parser.addOption(scancodesOpt);

    QCommandLineOption kbdColSkipsOpt(QStringList() << "c" << "column-skips",
            "show keyboard column (scan) skips");
    parser.addOption(kbdColSkipsOpt);

    QCommandLineOption eepromOpt(QStringList() << "e" << "eeprom",
            "show contents of EEPROM");
    parser.addOption(eepromOpt);
    
    QCommandLineOption writeEEPROMOpt("write-eeprom-byte",
            "write single byte in EEPROM", "addr=val");
    parser.addOption(writeEEPROMOpt);

    QCommandLineOption debugOpt("debug", "show debugging information");
    parser.addOption(debugOpt);

    QCommandLineOption bootloaderOpt(QStringList() << "b" << "bootloader",
            "reboot controller into dfu bootloader");
    parser.addOption(bootloaderOpt);

    parser.process(a);
#endif

    DiagInterface diag;
    vector<string> devPaths = diag.listDevPaths();
    string devPath;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
    int numOpts = parser.optionNames().size();

    if (parser.isSet(listDevsOpt))
    {
        if (devPaths.empty())
            cerr << "error: no controller devices found" << endl;
        else
            for (auto i = devPaths.begin(); i != devPaths.end(); ++i)
                cout << *i << endl;

        /* exit if no more commands---we don't need to open the device */
        if (numOpts == 1)
            return 0;
    }

    if (parser.isSet(devOpt))
        devPath = parser.value(devOpt).toStdString();
    else if (!devPaths.empty())
        devPath = devPaths[0];

#endif
    try
    {
        /* run GUI if no arguments specified (except for --device) */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
        if (numOpts == 0 || (numOpts == 1 && parser.isSet(devOpt)))
        {
            /* find controller */
            if (!parser.isSet(devOpt))
#endif
            {
                if (devPaths.empty())
                {
                    QMessageBox::critical(NULL, "Error", "No controller found");
                    return 1;
                }
                else if (devPaths.size() == 1)
                    devPath = devPaths[0];
                else
                {
                    QStringList l;
                    for (vector<string>::iterator i = devPaths.begin();
                            i != devPaths.end(); ++i)
                        l.append(QString::fromStdString(*i));

                    bool itemChosen;
                    devPath = QInputDialog::getItem(NULL, "Choose keyboard", 
                            "Multiple beamspring controllers found, please "
                            "choose one:",
                            l, 0, false, &itemChosen).toStdString();
                    if (!itemChosen)
                        return 1;
                }
            }

            cerr << "path chosen: " << devPath << endl;
            diag.openDev(devPath);

            Frontend frontend(diag);
            frontend.show();

            return a.exec();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
        }
#endif
    }
    catch (std::exception &e)
    {
        QMessageBox::critical(NULL, "Error", QString::fromStdString(e.what()));
        return 1;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 1))
    /* open device */
    diag.openDev(devPath);

    if (parser.isSet(infoOpt))
    {
        cout << "Keyboard type:\t";
        switch (diag.keyboardType())
        {
            case dktBeamspring:
                cout << "Beamspring" << endl;
                break;
            case dktBeamspringDisplaywriter:
                cout << "Beamspring Displaywriter" << endl;
                break;
            case dktModelF:
                cout << "Model F" << endl;
                break;
            default:
                cout << "(invalid)" << endl;
                break;
        }

        cout << "Columns:\t" << diag.cols() << endl;
        cout << "Rows:\t\t" << diag.rows() << endl;
    }

    if (parser.isSet(vrefOpt))
    {
        cerr << "vref: ";
        cout << diag.vref() << endl;
    }

    if (parser.isSet(kbdStateOpt))
    {
        cerr << "keyboard state:" << endl;

        vector<vector<bool>> states = diag.keyStates();
        if (states.empty())
        {
            cerr << "error: keyboard has no columns" << endl;
            return 1;
        }
        for (size_t j = 0; j < states[0].size(); j++)
        {
            for (size_t i = 0; i < states.size(); i++)
                cout << (states[i][j] ? '*' : '_') << ' ';
            cout << endl;
        }
    }

    if (parser.isSet(scancodesOpt))
    {
        cerr << "scancodes:" << endl;

        vector<vector<vector<unsigned char>>> scancodes = diag.scancodes();
        for (size_t layer = 0; layer < scancodes.size(); layer++)
        {
            cout << endl << "layer " << layer + 1 << ":" << endl;
            if (scancodes[layer].empty())
            {
                cerr << "error: scancodes' layer has no columns" << endl;
                return 1;
            }

            for (size_t row = 0; row < scancodes[layer][0].size(); row++)
            {
                for (size_t col = 0; col < scancodes[layer].size(); col++)
                    cout << "0x" << hex << setw(2) << setfill('0') <<
                        (unsigned int)scancodes[layer][col][row] << ' ';
                cout << endl;
            }
        }
    }

    if (parser.isSet(kbdColSkipsOpt))
    {
        cerr << "column skips:" << endl;

        vector<bool> skips = diag.kbdColSkips();
        for (size_t i = 0; i < skips.size(); i++)
            cout << setw(2) << i + 1 << ": " <<
                (skips[i] ? "(SKIP)" : "(____)") << endl;
    }

    if (parser.isSet(eepromOpt))
    {
        cerr << "eeprom contents:" << endl;
        vector<unsigned char> eep = diag.eepromContents();
        for (size_t i = 0; i < eep.size(); i++)
            cout << dec << setw(4) << setfill(' ') << i << ": 0x" <<
                hex << setw(2) << setfill('0') << (unsigned int)eep[i] << endl;
    }

    if (parser.isSet(debugOpt))
    {
        vector<unsigned char> d = diag.debugInfo();
        for (size_t i = 0; i < d.size(); i++)
            cout << dec << setw(4) << setfill(' ') << i << ": 0x" <<
                hex << setw(2) << setfill('0') << (unsigned int)d[i] << endl;
    }

    if (parser.isSet(writeEEPROMOpt))
    {
        string s = parser.value(writeEEPROMOpt).toStdString();
        unsigned int addr;
        unsigned char val;
        if (sscanf(s.c_str(), "%u = 0x%hhx", &addr, &val) == 2)
            ;
        else if (sscanf(s.c_str(), "%u = %hhu", &addr, &val) == 2)
            ;
        else if (sscanf(s.c_str(), "0x%x = 0x%hhx", &addr, &val) == 2)
            ;
        else
        {
            cerr << "error: EEPROM write syntax should be in the form of "
                "addr=val" << endl;
            return 1;
        }
        cerr << "writing EEPROM byte " << addr << " to " << (unsigned int)val <<
            endl;
        diag.writeEEPROMByte(addr, val);
    }

    if (parser.isSet(bootloaderOpt))
    {
        cerr << "entering bootloader" << endl;
        diag.enterBootloader();
    }
#endif

    return 0;
}
