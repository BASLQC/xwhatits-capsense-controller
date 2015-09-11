/*****************************************************************************
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
 *****************************************************************************/
#include "DiagInterface.h"

#define DEV_VID 0x0481
#define DEV_PID 0x0002
#define USB_TIMEOUT_MS 5000

using namespace std;

/*
 *
 */
LayerCondition::LayerCondition(void):
    fnKeys(0),
    layer_(0)
{
}

/*
 *
 */
LayerCondition::LayerCondition(unsigned char bin)
{
    fnKeys = (bin >> 4);
    if (fnKeys > 7)
        fnKeys = 0;

    layer_ = bin & 0xf;
}

/*
 *
 */
LayerCondition::LayerCondition(bool fn1, bool fn2, bool fn3, int layer):
    fnKeys(0),
    layer_(layer)
{
    if (fn1)
        fnKeys |= fkmFn1;
    if (fn2)
        fnKeys |= fkmFn2;
    if (fn3)
        fnKeys |= fkmFn3;
}

/*
 *
 */
bool LayerCondition::fn1Set(void)
{
    return (fnKeys & fkmFn1) > 0;
}

/*
 *
 */
bool LayerCondition::fn2Set(void)
{
    return (fnKeys & fkmFn2) > 0;
}

/*
 *
 */
bool LayerCondition::fn3Set(void)
{
    return (fnKeys & fkmFn3) > 0;
}

/*
 *
 */
int LayerCondition::layer(void)
{
    return layer_;
}

/*
 *
 */
unsigned char LayerCondition::toBin(void)
{
    return (fnKeys << 4) | (layer_ & 0xf);
}

/*
 *
 */
DiagInterface::DiagInterface(void):
    dev(NULL),
    _numLayers(0),
    _numLayerConditions(0),
    _cols(0),
    _rows(0)
{
    if (hid_init() != 0)
        throw runtime_error(string("error initialising hidapi"));
}

/*
 *
 */
DiagInterface::~DiagInterface(void)
{
    closeDevice();

    if (hid_exit() != 0)
        cerr << "warning: error during hid_exit" << endl;
}


/*
 *
 */
std::vector<std::string> DiagInterface::listDevPaths(void)
{
    vector<string> devPaths;
    hid_device_info *devs = hid_enumerate(DEV_VID, DEV_PID);
    if (!devs)
        return devPaths;

    for (hid_device_info *i = devs; i != NULL; i = i->next)
        if (i->interface_number == 1 ||
                (i->interface_number == -1 && i->usage != 6))
            devPaths.push_back(i->path);

    hid_free_enumeration(devs);

    return devPaths;
}

/*
 *
 */
void DiagInterface::openDev(std::string devPath)
{
    dev = hid_open_path(devPath.c_str());
    if (!dev)
        throw runtime_error("error: couldn't open device");

    updateControllerInfo();
}

/*
 *
 */
void DiagInterface::updateControllerInfo(void)
{
    setState(drNothing, false);

    setState(drInfo);
    _numLayers = currState[1];
    _numLayerConditions = currState[2];
    _cols = currState[3];
    _rows = currState[4];
    _keyboardType = (DiagKeyboardType)currState[5];
    _usingNKROReport = (currState[6] != 0);
}

/*
 *
 */
unsigned short DiagInterface::vref(void)
{
    unsigned short val;

    setState(drNothing, false);
    setState(drVref);
    val = currState[1] + (currState[2] << 8);

    return val;
}

/*
 *
 */
void DiagInterface::setVref(unsigned short val)
{
    sendCmd(dcSetVref, val);
}

/*
 *
 */
void DiagInterface::storeVref(void)
{
    sendCmd(dcStoreVref, 1);
}

/*
 *
 */
void DiagInterface::autoCalibrate(void)
{
    sendCmd(dcAutoCalibrate, 1);
}

/*
 *
 */
static void populateCols(vector<vector<bool>> &states, size_t startCol,
        size_t endCol, size_t rows, unsigned char vals[8])
{
    int currByte = 1;

    for (size_t col = startCol; col <= endCol; col++, currByte++)
        for (size_t row = 0; row < rows; row++)
            states[col][row] = vals[currByte] & (1 << row);
}
std::vector<std::vector<bool>> DiagInterface::keyStates(void)
{
    vector<vector<bool>> states(_cols, vector<bool>(_rows, false));
    bool got1stCols = false;
    bool got2ndCols = false;
    bool got3rdCols = false;
    bool got4thCols = _cols <= 21 ? true : false;

    setState(drNothing, false);
    setState(dr1stCols);
    while (!(got1stCols && got2ndCols && got3rdCols && got4thCols))
    {
        int startCol = 0;
        int endCol = 0;
        switch (currState[0])
        {
            case dr1stCols:
                got1stCols = true;
                startCol = 0;
                endCol = min(_cols - 1, 6);
                break;
            case dr2ndCols:
                got2ndCols = true;
                startCol = 7;
                endCol = min(_cols - 1, 13);
                break;
            case dr3rdCols:
                got3rdCols = true;
                startCol = 14;
                endCol = min(_cols - 1, 20);
                break;
            case dr4thCols:
                got4thCols = true;
                startCol = 21;
                endCol = min(_cols - 1, 27);
                break;
        }

        populateCols(states, startCol, endCol, _rows, currState);

        updateState();
    }

    return states;
}

/*
 *
 */
void DiagInterface::enterBootloader(void)
{
    sendCmd(dcJumpBootloader, 1);
}

/*
 *
 */
std::vector<std::vector<std::vector<unsigned char>>> DiagInterface::scancodes(
        void)
{
    vector<vector<vector<unsigned char>>> sc(_numLayers,
            vector<vector<unsigned char>>(_cols,
                vector<unsigned char>(_rows, 0x00)));

    setState(drNothing, false);
    setState(drScanCodes);

    for (int i = 0; i < (_rows <= 4 ? 1 : 2); i++)
    {
        for (int i = 0; i < _numLayers; i++)
        {
            for (int i = 0; i < _cols; i++)
            {
                for (int i = 0; i < 4; i++)
                {
                    unsigned char layer = currState[1] >> 6;
                    unsigned char slice = (currState[1] >> 5) & 0x1;
                    unsigned char col = currState[1] & 0x1f;
                    sc[layer][col][slice == 0 ? i : i + 4] = currState[i + 2];
                }
                updateState();
            }
        }
    }

    return sc;
}

/*
 *
 */
void DiagInterface::setScancodes(std::vector<std::vector<std::vector<
        unsigned char>>> codes)
{
    int layers = codes.size();
    if (layers == 0)
        throw runtime_error("error setting scancodes: no layers");
    int cols = codes[0].size();
    if (cols != _cols)
        throw runtime_error("error setting scancodes: number of columns "
                "differs from keyboard");
    int rows = codes[0][0].size();
    if (rows != _rows)
        throw runtime_error("error setting scancodes: number of rows "
                "differs from keyboard");

    /* copy base layer to other layers if not specified */
    if (layers < _numLayers)
        for (int i = layers; i < _numLayers; i++)
            codes.push_back(codes[0]);

    setState(drNothing, false);
    for (int layer = 0; layer < _numLayers; layer++)
        for (int col = 0; col < _cols; col++)
            for (int j = 0; j < _rows; j++)
                setScancode(layer, col, j, codes[layer][col][j]);
}

/*
 *
 */
void DiagInterface::setScancode(int layer, int col, int row,
        unsigned char scancode)
{
    if (layer < 0 || layer >= _numLayers)
    {
        stringstream ss;
        ss << "error setting scancode: layer out of range (min=0, max=" <<
            _numLayers << ", val=" << layer << ")";
        throw runtime_error(ss.str());
    }
    if (col < 0 || col >= _cols)
    {
        stringstream ss;
        ss << "error setting scancode: column out of range (min=0, max=" << 
            _cols << ", val=" << col << ")";
        throw runtime_error(ss.str());
    }
    if (row < 0 || row >= _rows)
    {
        stringstream ss;
        ss << "error setting scancode: row out of range (min=0, max=" <<
            _rows << ", val=" << row << ")";
        throw runtime_error(ss.str());
    }
    sendCmd(dcSetScanCode, (layer << 6) | (col & 0x3f), row, scancode);
}

/*
 *
 */
void DiagInterface::loadScancodes(void)
{
    sendCmd(dcLoadScanCodes, 1);
}

/*
 *
 */
void DiagInterface::storeScancodes(void)
{
    sendCmd(dcStoreScanCodes, 1);
}

/*
 *
 */
std::vector<LayerCondition> DiagInterface::layerConditions(void)
{
    vector<LayerCondition> cnds(_numLayerConditions);

    setState(drNothing, false);
    setState(drLayerConditions);
    for (int i = 0; i < 2; i++)
    {
        unsigned char set = currState[1];

        for (int i = 0; i < 4; i++)
        {
            unsigned char val = currState[2 + i];
            cnds[set * 4 + i] = LayerCondition(val);
        }

        updateState();
    }

    return cnds;
}

/*
 *
 */
void DiagInterface::setLayerCondition(int conditionIdx, LayerCondition cnd)
{
    sendCmd(dcSetLayerCondition, (unsigned char)conditionIdx, cnd.toBin());
}

/*
 *
 */
std::string DiagInterface::version(void)
{
    char buf[7];

    setState(drNothing, false);
    setState(drVersion);

    for (int i = 0; i < 7; i++)
        buf[i] = currState[i + 1];
    buf[6] = '\0';

    return buf;
}

/*
 *
 */
void DiagInterface::expMode(int &mode, int &val1, int &val2)
{
    setState(drNothing, false);
    setState(drExpMode);

    mode = currState[1];
    val1 = currState[2];
    val2 = currState[3];
}

/*
 *
 */
void DiagInterface::setExpMode(int mode, int val1, int val2)
{
    sendCmd(dcSetExpMode, mode, val1, val2);
}

/*
 *
 */
void DiagInterface::storeExpMode(void)
{
    sendCmd(dcStoreExpMode, 1);
}

/*
 *
 */
std::vector<bool> DiagInterface::kbdColSkips(void)
{
    vector<bool> colSkips(_cols, false);

    setState(drNothing, false);
    setState(drKbdColSkips);

    for (int i = 0; i < _cols; i++)
        colSkips[i] = (currState[i / 8 + 1] & (1 << (i % 8)));

    return colSkips;
}

/*
 *
 */
void DiagInterface::setKbdColSkips(std::vector<bool> skips)
{
    char buf[7] = { 0 };

    if (skips.size() != (size_t)_cols)
        throw runtime_error("error setting keyboard column skips: size of "
                "skips doesn't match number of reported keyboard columns");

    memset(buf, 0x00, sizeof(buf));
    for (int i = 0; i < _cols; i++)
        if (skips[i])
            buf[i / 8] |= (1 << (i % 8));

    sendCmd(dcSetKbdColSkips, buf[0], buf[1], buf[2]);
}

/*
 *
 */
void DiagInterface::storeKbdColSkips(void)
{
    sendCmd(dcStoreKbdColSkips, 1);
}

/*
 *
 */
std::vector<unsigned char> DiagInterface::eepromContents(void)
{
    vector<unsigned char> v;

    setState(drNothing, false);
    setState(drEEPROM);

    int firstLoc = -1;
    while (firstLoc != currState[1])
    {
        if (firstLoc < 0)
            firstLoc = currState[1];

        unsigned int startAddr = (unsigned int)currState[1] * 4;
        unsigned int endAddr = startAddr + 4;
        if (endAddr >= v.size())
            v.resize(endAddr, 0xaa);

        for (int i = 0; i < 4; i++)
            v[startAddr + i] = (unsigned int)currState[i + 2];

        updateState();
    }

    return v;
}

/*
 *
 */
void DiagInterface::writeEEPROMByte(int addr, unsigned char val)
{
    if (addr < 0 || addr > 1023)
        throw runtime_error("error writing EEPROM byte: address must be "
                "between 0 and 1023");
    sendCmd(dcWriteEEPROMByte, addr & 0xff, addr >> 8, val);
}

/*
 *
 */
std::vector<unsigned char> DiagInterface::debugInfo(void)
{
    vector<unsigned char> v;

    setState(drNothing, false);
    setState(drDebug);

    int firstLoc = -1;
    while (firstLoc != currState[1])
    {
        unsigned int addr = (unsigned int)currState[1];
        unsigned int data = (unsigned int)currState[2];

        if (firstLoc < 0)
            firstLoc = addr;

        if (addr >= v.size())
            v.resize(addr + 1, 0xff);

        v[addr] = data;

        updateState();
    }

    return v;
}

/*
 *
 */
unsigned short DiagInterface::macroEEPROMSize(void)
{
    unsigned short val;

    setState(drNothing, false);
    setState(drMacroEEPROMSize);
    val = currState[1] + (currState[2] << 8);

    return val;
}

/*
 *
 */
std::vector<unsigned char> DiagInterface::macroBytes(void)
{
    vector<unsigned char> v;

    setState(drNothing, false);
    setState(drMacros);

    int firstLoc = -1;
    while (firstLoc != currState[1])
    {
        if (firstLoc < 0)
            firstLoc = currState[1];


        unsigned int startAddr = (unsigned int)currState[1] * 4;
        unsigned int endAddr = startAddr + 4;
        if (endAddr >= v.size())
            v.resize(endAddr, 0xaa);

        for (int i = 0; i < 4; i++)
            v[startAddr + i] = (unsigned int)currState[i + 2];

        updateState();
    }

    return v;
}

/*
 *
 */
void DiagInterface::writeMacroBytes(std::vector<unsigned char> bytes)
{
    unsigned char buf[8];
    buf[0] = dcWriteMacroSegment;

    for (unsigned int segment = 0; segment <= (bytes.size() / 4); segment++)
    {
        buf[1] = segment;

        fill(&buf[2], &buf[8], 0xff);

        auto it = bytes.begin() + (segment * 4);
        copy(it, min(it + 4, bytes.end()), &buf[2]);
        sendCtrl(buf);
    }

    sendCmd(dcLoadMacros, 1);
}

/*
 *
 */
void DiagInterface::setScanEnabled(bool enabled)
{
    sendCmd(dcSetScanEnabled, enabled ? 1 : 0);
}

/*
 *
 */
void DiagInterface::setLayerConditions(std::vector<LayerCondition> lcs)
{
    int numLCs = lcs.size();

    /* if no layer conditions, set default */
    if (numLCs == 0)
    {
        lcs.push_back(LayerCondition(true, false, false, 1));
        lcs.push_back(LayerCondition(false, true, false, 2));
        lcs.push_back(LayerCondition(false, false, true, 3));
        for (int i = 3; i < _numLayerConditions; i++)
            lcs.push_back(LayerCondition());
        numLCs = _numLayerConditions;
    }
    else if (numLCs > _numLayerConditions)
    {
        stringstream ss;
        ss << "error setting layer conditions: too many layers (max=" <<
            _numLayerConditions << ", val=" << numLCs << ")";
        throw runtime_error(ss.str());
    }

    for (int i = 0; i < numLCs; i++)
        setLayerCondition(i, lcs[i]);
}

/*
 *
 */
void DiagInterface::closeDevice(void)
{
    if (dev)
        hid_close(dev);
}

/*
 *
 */
void DiagInterface::sendCmd(DiagReportCommand cmd, unsigned char p1,
        unsigned char p2)
{
    unsigned char buf[8];
    buf[0] = cmd;
    buf[1] = p1;
    buf[2] = p2;
    sendCtrl(buf);
}

/*
 *
 */
void DiagInterface::sendCmd(DiagReportCommand cmd, unsigned char p1,
        unsigned char p2, unsigned char p3)
{
    unsigned char buf[8];
    buf[0] = cmd;
    buf[1] = p1;
    buf[2] = p2;
    buf[3] = p3;
    sendCtrl(buf);
}

/*
 *
 */
void DiagInterface::sendCmd(DiagReportCommand cmd, unsigned int data)
{
    unsigned char buf[8];
    buf[0] = cmd;
    buf[1] = data & 0xff;
    buf[2] = data >> 8;
    buf[3] = data >> 16;
    buf[4] = data >> 24;
    sendCtrl(buf);
}

/*
 *
 */
void DiagInterface::sendCtrl(unsigned char buf[8])
{
    unsigned char data[9];

    data[0] = 0; 
    memcpy(&data[1], buf, sizeof(data));

    bool success = false;
    QTime t;
    t.start();
    do
    {
        if (hid_write(dev, data, sizeof(data)) >= 0)
        {
            success = true;
            break;
        }

        cerr << "hid_write failed..." << endl;
    } while (t.elapsed() < USB_TIMEOUT_MS);

    if (!success)
        throw runtime_error("error: could not read from keyboard");
}

/*
 *
 */
void DiagInterface::setState(DiagReportState state, bool block)
{
    sendCmd(dcSetState, state);

    if (block)
    {
        do
        {
            updateState();
        } while (currState[0] != state);
    }
}

/*
 *
 */
void DiagInterface::updateState(void)
{
    unsigned char buf[8];
    int bytesRead = 0;

    bool success = false;
    QTime t;
    t.start();
    do
    {
        memset(buf, 0x00, sizeof(buf));
        bytesRead = hid_read(dev, buf, sizeof(buf));
        if (bytesRead >= 0)
        {
            success = true;
            break;
        }

        cerr << "hid_read failed..." << endl;
    } while (t.elapsed() < USB_TIMEOUT_MS);

    if (!success)
        throw runtime_error("error: could not read from keyboard");

    memcpy(currState, buf, bytesRead);
}
