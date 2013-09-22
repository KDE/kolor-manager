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

#include <QCryptographicHash>
#include <QDBusMetaType>
#include <QRect>
#include <QSharedMemory>

namespace KolorServer
{

/**
 * Color lookup table
 *
 * The 3D lookup texture has 64 points in each dimension, using 16 bit integers.
 * That means each active region will use 1.5MiB of texture memory.
 */
static const int LUT_GRID_POINTS = 64;
static const size_t CLUT_ELEMENT_SIZE = sizeof(quint16);
static const uint CLUT_ELEMENT_COUNT = LUT_GRID_POINTS * LUT_GRID_POINTS * LUT_GRID_POINTS * 3;
static const size_t CLUT_DATA_SIZE = CLUT_ELEMENT_COUNT * CLUT_ELEMENT_SIZE;

class ColorLookupTable
{
    friend QDBusArgument &operator<<(QDBusArgument &argument, const ColorLookupTable &c);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ColorLookupTable &c);
    friend uint qHash(const ColorLookupTable &c);

public:
    explicit ColorLookupTable(bool initializeWithDummy = false);
    virtual ~ColorLookupTable();

    void fillWithDummyValues();
    void copyTableData(const ColorLookupTable &other);
    quint16* table();
    const quint16* table() const;
    quint16 checksum() const;
    uint hash() const;

    int indexInPool() const;
    bool loadFromPool(int index);

    void operator =(const ColorLookupTable &other);
    bool operator ==(const ColorLookupTable &other) const;

private:
    quint16 m_table[LUT_GRID_POINTS][LUT_GRID_POINTS][LUT_GRID_POINTS][3];
};

typedef QList<ColorLookupTable> ColorLookupTableList;

// Marshall the ColorLookupTable data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const ColorLookupTable &c)
{
    argument.beginStructure();
    argument << c.indexInPool();
    argument.endStructure();
    return argument;
}

// Retrieve the ColorLookupTable data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, ColorLookupTable &c)
{
    int indexInPool;
    argument.beginStructure();
    argument >> indexInPool;
    c.loadFromPool(indexInPool);
    argument.endStructure();
    return argument;
}

inline uint qHash(const ColorLookupTable &c)
{
    const QByteArray data((const char *) &c.m_table[0][0][0][0], CLUT_DATA_SIZE);
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toUInt();
}


/**
 * Color lookup table for a particular window region
 * Has a window region id, output index and region of the window
 */
class RegionColorLookupTable : public ColorLookupTable
{
    friend QDBusArgument &operator<<(QDBusArgument &argument, const RegionColorLookupTable &rc);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, RegionColorLookupTable &rc);

public:
    explicit RegionColorLookupTable(bool initializeWithDummy = false);
    explicit RegionColorLookupTable(uint windowId, int outputIndex, const QRect &region,
                                    bool initializeWithDummy = false);
    virtual ~RegionColorLookupTable();

    uint windowId() const;
    void setWindowId(uint windowId);
    int outputIndex() const;
    void setOutputIndex(int outputIndex);
    QRect region() const;
    void setRegion(const QRect &region);

private:
    uint m_windowId;
    int m_outputIndex;
    QRect m_region;
};

typedef QList<RegionColorLookupTable> RegionColorLookupTableList;

// Marshall the RegionColorLookupTable data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const RegionColorLookupTable &rc)
{
    argument.beginStructure();
    argument << rc.m_windowId << rc.m_outputIndex << rc.m_region << rc.indexInPool();
    argument.endStructure();
    return argument;
}

// Retrieve the RegionColorLookupTable data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, RegionColorLookupTable &rc)
{
    int indexInPool;
    argument.beginStructure();
    argument >> rc.m_windowId >> rc.m_outputIndex >> rc.m_region >> indexInPool;
    rc.loadFromPool(indexInPool);
    argument.endStructure();
    return argument;
}


/**
 * Color lookup table shared memory pool
 * All the lookup tables are copied inside this pool - which will be shared
 * with the processes that need the lookup table data.
 *
 * Access to this shared memory region should be read-write for the process containing
 * kolor-server (kded) and read-only for the other processes (for example kwin).
 *
 * There should be only one instance of this color lookup table pool.
 *
 * The data contains all the color lookup tables in sequence.
 */
class ColorLookupTablePool : public QSharedMemory
{
    Q_OBJECT
public:
    static ColorLookupTablePool *instance();
    static void cleanup();

    int colorLookupTableIndex(const ColorLookupTable &c);
    void addColorLookupTable(const ColorLookupTable &c);
    const quint16 *colorLookupTableData(int index) const;

signals:
    void keyChanged(const QString &newKey);
    void ready();

private:
    ColorLookupTablePool(QObject* parent = 0);
    virtual ~ColorLookupTablePool();

    void resetKey();

private:
    /// Hash for determining which index corresponds for a color lookup table
    QHash<ColorLookupTable, int> m_elements;
    int m_elementCount;
    int m_capacity;

    static ColorLookupTablePool *m_instance;
};

} // KolorServer namespace

/*
 * Qt Metatype declarations
 */
Q_DECLARE_METATYPE(KolorServer::ColorLookupTable)
Q_DECLARE_METATYPE(KolorServer::RegionColorLookupTable)
Q_DECLARE_METATYPE(KolorServer::ColorLookupTableList)
Q_DECLARE_METATYPE(KolorServer::RegionColorLookupTableList)

#endif // COLOR_LOOKUP_TABLE_H_
