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
#include "ImportExport.h"

using namespace std;

static bool importLayerCondition(QString &line,
        vector<LayerCondition> &layerConditions, QString &errorString);
static bool importColumnSkips(QString &line,
        vector<bool> &skips, QString &errorString);
static bool importExpansionMode(QString &line,
        int &expMode, int &expVal1, int &expVal2, QString &errorString);
static bool importScancodeLine(QString &line, int &layer, int &row,
        std::vector<std::vector<std::vector<unsigned char>>> &scancodes,
        QString &errorString);

/*
 *
 */
bool exportLayout(QString filename,
        std::vector<std::vector<std::vector<Key *>>> keys,
        std::vector<LayerConditionWatcher *> lcws,
        std::vector<bool> colSkips,
        int expMode, int expVal1, int expVal2,
        QString &errorString)
{
    int layers = keys.size();
    if (layers == 0)
        throw runtime_error("error: asked to export layout with no layers");
    int cols = keys[0].size();
    if (cols == 0)
        throw runtime_error("error: asked to export layout with no columns");
    int rows = keys[0][0].size();
    if (rows == 0)
        throw runtime_error("error: asked to export layout with no rows");

    QFile f(filename);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        errorString = f.errorString();
        return false;
    }

    QTextStream ts(&f);
    ts.setFieldAlignment(QTextStream::AlignRight);
    ts.setIntegerBase(16);
    ts.setPadChar('0');
    for (int layer = 0; layer < layers; layer++)
    {
        ts << "# ";
        if (layer == 0)
            ts << "Base Layer\n";
        else
            ts << "Layer " << layer << '\n';

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                ts << "0x" << qSetFieldWidth(2) <<
                    (unsigned int)keys[layer][col][row]->scancode();
                ts << qSetFieldWidth(1);
                ts << ((col < cols - 1) ? ' ' : '\n');
            }
        }

        ts << '\n';
    }

    ts.reset();
    ts << "# Layer Conditions\n";
    for (size_t i = 0; i < lcws.size(); i++)
    {
        LayerCondition lc = lcws[i]->toLayerCondition();
        ts << '(' << lc.fn1Set() << ' ' << lc.fn2Set() << ' ' << lc.fn3Set() <<
            ")\t" << lc.layer() << '\n';
    }
    ts << '\n';

    ts << "# Column Skips\n";
    ts << '[';
    for (size_t i = 0; i < colSkips.size(); i++)
    {
        ts << colSkips[i];
        if (i < colSkips.size() - 1)
            ts << ' ';
    }
    ts << "]\n";
    ts << '\n';

    ts << "# Expansion header\n";
    ts << "+ " << expMode << ' ' << expVal1 << ' ' << expVal2;

    f.close();

    return true;
}

/*
 *
 */
bool importLayerCondition(QString &line,
        vector<LayerCondition> &layerConditions, QString &errorString)
{
    QRegExp rx("\\(\\s*(\\d)\\s*(\\d)\\s*(\\d)\\s*\\)\\s*(\\d)",
            Qt::CaseSensitive, QRegExp::RegExp2);
    if (rx.indexIn(line) < 0 || rx.captureCount() != 4)
    {
        errorString = "layer condition line has incorrect syntax: \"" + line +
            '"';
        return false;
    }

    LayerCondition lc(rx.cap(1).toInt(), rx.cap(2).toInt(), rx.cap(3).toInt(),
            rx.cap(4).toInt());
    layerConditions.push_back(lc);

    return true;
}

/*
 *
 */
bool importColumnSkips(QString &line, vector<bool> &skips, QString &errorString)
{
    bool gotStart = false;
    bool gotEnd = false;

    for (auto c = line.begin(); c != line.end(); ++c)
    {
        if (*c == '0' || *c == '1')
        {
            if (!gotStart || gotEnd)
            {
                errorString = "column skips line has incorrect syntax: \"" +
                    line + '"';
                return false;
            }

            if (*c == '0')
                skips.push_back(false);
            else
                skips.push_back(true);
        }
        else if (*c == '[')
            gotStart = true;
        else if (*c == ']')
            gotEnd = true;
    }

    if (!gotStart || !gotEnd)
    {
        errorString = "column skips line has incorrect syntax: \"" +
            line + '"';
        return false;
    }
    
    return true;
}

/*
 *
 */
bool importExpansionMode(QString &line, int &expMode, int &expVal1,
        int &expVal2, QString &errorString)
{
    QRegExp rx("\\+\\s*(\\d*)\\s*(\\d*)\\s*(\\d*)",
            Qt::CaseSensitive, QRegExp::RegExp2);
    if (rx.indexIn(line) < 0 || rx.captureCount() != 3)
    {
        errorString = "expansion mode line has incorrect syntax: \"" + line +
            '"';
        return false;
    }

    expMode = rx.cap(1).toInt();
    expVal1 = rx.cap(2).toInt();
    expVal2 = rx.cap(3).toInt();

    return true;
}

/*
 *
 */
bool importScancodeLine(QString &line, int &layer, int &row,
        std::vector<std::vector<std::vector<unsigned char>>> &scancodes,
        QString &errorString)
{
    QTextStream ts(&line);
    int col = 0;
    while (!ts.atEnd())
    {
        int scancode;
        ts >> scancode;

        if (row == 0)
            scancodes[layer].push_back(vector<unsigned char>());
        try
        {
            scancodes[layer].at(col++).push_back(scancode);
        }
        catch (std::out_of_range &e)
        {
            errorString = "unequal number of columns";
            return false;
        }
    }

    return true;
}

/*
 *
 */
bool importLayout(QString filename,
        std::vector<std::vector<std::vector<unsigned char>>> &scancodes,
        std::vector<LayerCondition> &layerConditions,
        std::vector<bool> &colSkips,
        int &expMode, int &expVal1, int &expVal2,
        QString &errorString)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        errorString = f.errorString();
        return false;
    }

    expMode = expVal1 = expVal2 = 0;

    QTextStream ls(&f);
    QString line;
    int row = 0;
    int layer = 0;
    scancodes.push_back(vector<vector<unsigned char>>());
    while (!(line = ls.readLine()).isNull())
    {
        line = line.trimmed();
        if (line.isEmpty() || line[0] == '#')
        {
            scancodes.push_back(vector<vector<unsigned char>>());
            layer++;
            row = 0;

            continue;
        }
        else if (line[0] == '(')
        {
            /* import layer condition */
            if (!importLayerCondition(line, layerConditions, errorString))
                return false;
        }
        else if (line[0] == '[')
        {
            /* import column skips */
            if (!importColumnSkips(line, colSkips, errorString))
                return false;
        }
        else if (line[0] == '+')
        {
            /* import expansion header mode/vals */
            if (!importExpansionMode(line, expMode, expVal1, expVal2,
                        errorString))
                return false;
        }
        else
        {
            /* import line of scancodes */
            if (!importScancodeLine(line, layer, row, scancodes, errorString))
                return false;
            row++;
        }
    }

    /* strip out blank layers from extra blank lines */
    for (auto i = scancodes.begin(); i != scancodes.end(); )
    {
        if (i->empty())
            i = scancodes.erase(i);
        else
            ++i;
    }

    /* consistency checks */
    size_t layers = scancodes.size();
    if (layers <= 0)
    {
        errorString = "no layers imported";
        return false;
    }

    size_t prevCols = 0;
    size_t prevRows = 0;
    for (size_t layer = 0; layer < layers; layer++)
    {
        size_t cols = scancodes[layer].size();
        if (cols == 0)
        {
            errorString = "no columns imported on layer " +
                QString::number(layer);
            return false;
        }
        else if (prevCols != 0 && prevCols != cols)
        {
            errorString = "unequal number of columns";
            return false;
        }

        for (size_t col = 0; col < cols; col++)
        {
            size_t rows = scancodes[layer][row].size();
            if (rows == 0)
            {
                errorString = "no rows imported on layer " +
                    QString::number(layer) + ", col " + QString::number(col);
                return false;
            }
            else if (prevRows != 0 && prevRows != rows)
            {
                errorString = "unequal number of rows";
                return false;
            }

            prevRows = rows;
        }

        prevCols = cols;
    }

    /* if there were no column skips, populate with blanks */
    if (colSkips.empty())
        colSkips = vector<bool>(scancodes[0].size());

    return true;
}
