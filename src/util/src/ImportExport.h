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
#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <stdexcept>
#include <vector>
#include "Key.h"
#include "LayerConditions.h"

bool exportLayout(QString filename,
        std::vector<std::vector<std::vector<Key *>>> keys,
        std::vector<LayerConditionWatcher *> lcws,
        std::vector<bool> colSkips,
        int expMode, int expVal1, int expVal2,
        std::vector<unsigned char> macroBytes,
        QString &errorString);
bool importLayout(QString filename,
        std::vector<std::vector<std::vector<unsigned char>>> &scancodes,
        std::vector<LayerCondition> &layerConditions,
        std::vector<bool> &colSkips,
        int &expMode, int &expVal1, int &expVal2,
        std::vector<unsigned char> &macroBytes,
        QString &errorString);

#endif
