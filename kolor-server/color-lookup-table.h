/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2012 Casian Andrei <skeletk13@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef COLOR_LOOKUP_TABLE_H_
#define COLOR_LOOKUP_TABLE_H_

#include <QDBusMetaType>
#include <QList>
#include <QRect>
#include <QVector>

/*
 * Clut
 */
typedef QVector<quint16> Clut;
typedef QList<Clut> ClutList;

Q_DECLARE_METATYPE(Clut)
Q_DECLARE_METATYPE(ClutList)
Q_DECLARE_METATYPE(QList<QRect>)

/*
 * Color lookup table
 *
 * The 3D lookup texture has 64 points in each dimension, using 16 bit integers.
 * That means each active region will use 1.5MiB of texture memory.
 */
static const int LUT_GRID_POINTS = 64;
static const size_t CLUT_ELEMENT_SIZE = sizeof(quint16);
static const uint CLUT_ELEMENT_COUNT = LUT_GRID_POINTS * LUT_GRID_POINTS * LUT_GRID_POINTS * 3;
static const size_t CLUT_DATA_SIZE = CLUT_ELEMENT_COUNT * CLUT_ELEMENT_SIZE;

inline static void buildDummyClut(Clut &c)
{
    c.resize(CLUT_ELEMENT_COUNT);
    quint16 *p = c.data();

    for (int ib = 0; ib < LUT_GRID_POINTS; ++ ib) {
        quint16 b = (quint16) ((float) ib / (LUT_GRID_POINTS - 1) * 65535.0 + 0.5);
        for (int ig = 0; ig < LUT_GRID_POINTS; ++ ig) {
            quint16 g = (quint16) ((float) ig / (LUT_GRID_POINTS - 1) * 65535.0 + 0.5);
            for (int ir = 0; ir < LUT_GRID_POINTS; ++ ir) {
                quint16 r = (quint16) ((float) ir / (LUT_GRID_POINTS - 1) * 65535.0 + 0.5);

                *(p ++) = r;
                *(p ++) = g;
                *(p ++) = b;
            }
        }
    }
}

#endif // COLOR_LOOKUP_TABLE_H_
