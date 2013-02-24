/********************************************************************
 KolorServer - color server based on the X Color Management Specification
 This file is part of the KDE project.

Copyright (C) 2012 Casian Andrei <skeletk13@gmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#ifndef COLOR_LOOKUP_TABLE_H_
#define COLOR_LOOKUP_TABLE_H_

#include <QDBusMetaType>
#include <QList>
#include <QMultiMap>
#include <QPair>
#include <QRect>
#include <QVector>


/*
 * Clut
 */
typedef QVector<quint16> Clut;
typedef QList<Clut> ClutList;
typedef struct {
    uint windowId;
    int outputIndex;
    QRect region;
    Clut clut;
} RegionalClut;
typedef QMultiMap<uint, RegionalClut> RegionalClutMap;

Q_DECLARE_METATYPE(Clut)
Q_DECLARE_METATYPE(ClutList)
Q_DECLARE_METATYPE(RegionalClut)
Q_DECLARE_METATYPE(RegionalClutMap)

// Marshall the RegionalClut data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const RegionalClut &rc)
{
    argument.beginStructure();
    argument << rc.windowId << rc.outputIndex << rc.region << rc.clut;
    argument.endStructure();
    return argument;
}

// Retrieve the RegionalClut data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, RegionalClut &rc)
{
    argument.beginStructure();
    argument >> rc.windowId >> rc.outputIndex >> rc.region >> rc.clut;
    argument.endStructure();
    return argument;
}


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

inline static quint16 clutChecksum(const Clut &c)
{
    quint16 checksum = 0;
    for (int i = 0; i < c.size(); ++i)
        checksum ^= c[i];
    return checksum;
}

#endif // COLOR_LOOKUP_TABLE_H_
