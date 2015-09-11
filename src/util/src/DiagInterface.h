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
#ifndef DIAGINTERFACE_H
#define DIAGINTERFACE_H

#include <cstdlib>
#include <cstring>
#include <hidapi.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "diag_types.h"

class LayerCondition
{
    public:
        enum FnKeyMask
        {
            fkmFn1 = (1 << 0),
            fkmFn2 = (1 << 1),
            fkmFn3 = (1 << 2)
        };

        LayerCondition(void);
        LayerCondition(unsigned char bin);
        LayerCondition(bool fn1, bool fn2, bool fn3, int layer);
        bool fn1Set(void);
        bool fn2Set(void);
        bool fn3Set(void);
        int layer(void);
        unsigned char toBin(void);

    private:
        unsigned char fnKeys;
        unsigned char layer_;
};

class DiagInterface
{
    public:
        DiagInterface(void);
        ~DiagInterface(void);

        std::vector<std::string> listDevPaths(void);
        void openDev(std::string devPath);

        int numLayers(void) { return _numLayers; }
        int numLayerConditions(void) { return _numLayerConditions; }
        int cols(void) { return _cols; }
        int rows(void) { return _rows; }
        DiagKeyboardType keyboardType(void) { return _keyboardType; }

        unsigned short vref(void);
        void setVref(unsigned short val);
        void storeVref(void);
        void autoCalibrate(void);

        std::vector<std::vector<bool>> keyStates(void);

        void enterBootloader(void);

        std::vector<std::vector<std::vector<unsigned char>>> scancodes(void);
        void setScancodes(std::vector<std::vector<std::vector<
                unsigned char>>> codes);
        void setScancode(int layer, int col, int row, unsigned char scancode);
        void loadScancodes(void);
        void storeScancodes(void);

        std::vector<LayerCondition> layerConditions(void);
        void setLayerCondition(int conditionIdx, LayerCondition cnd);
        void setLayerConditions(std::vector<LayerCondition> lcs);

        std::string version(void);

        void expMode(int &mode, int &val1, int &val2);
        void setExpMode(int mode, int val1, int val2);
        void storeExpMode(void);

        std::vector<bool> kbdColSkips(void);
        void setKbdColSkips(std::vector<bool> skips);
        void storeKbdColSkips(void);

    private:
        hid_device *dev;
        unsigned char currState[8];
        int _numLayers;
        int _numLayerConditions;
        int _cols;
        int _rows;
        DiagKeyboardType _keyboardType;

        void closeDevice(void);
        void sendCmd(DiagReportCommand cmd, unsigned char p1, unsigned char p2);
        void sendCmd(DiagReportCommand cmd, unsigned char p1, unsigned char p2,
                unsigned char p3);
        void sendCmd(DiagReportCommand cmd, unsigned int data);
        void sendCtrl(unsigned char buf[8]);
        void setState(DiagReportState state, bool block = true);
        void updateState(void);
};

#endif
