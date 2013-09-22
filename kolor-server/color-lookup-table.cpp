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

#include "color-lookup-table.h"

#include <KDebug>

#include <QCoreApplication>

#include <cstdlib>

namespace KolorServer
{

/*
 * Color Lookup Table
 */
ColorLookupTable::ColorLookupTable(bool initializeWithDummy)
{
    if (initializeWithDummy)
        fillWithDummyValues();
}

ColorLookupTable::~ColorLookupTable()
{

}

void ColorLookupTable::fillWithDummyValues()
{
    quint16 *p = &m_table[0][0][0][0];

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

void ColorLookupTable::copyTableData(const ColorLookupTable& other)
{
    memcpy(m_table, other.m_table, CLUT_DATA_SIZE);
}

quint16* ColorLookupTable::table()
{
    return &m_table[0][0][0][0];
}

const quint16* ColorLookupTable::table() const
{
    return &m_table[0][0][0][0];
}

quint16 ColorLookupTable::checksum() const
{
    quint16 checksum = 0;
    const quint16 *p = &m_table[0][0][0][0];
    for (uint i = 0; i < CLUT_ELEMENT_COUNT; ++i)
        checksum ^= *(p++);
    return checksum;
}

uint ColorLookupTable::hash() const
{
    return qHash(*this);
}

int ColorLookupTable::indexInPool() const
{
    return ColorLookupTablePool::instance()->colorLookupTableIndex(*this);
}

bool ColorLookupTable::loadFromPool(int index)
{
    const ColorLookupTablePool *clutPool = ColorLookupTablePool::instance();
    const quint16 *clutData = clutPool->colorLookupTableData(index);
    if (!clutData)
        return false;

    memcpy(m_table, clutData, CLUT_DATA_SIZE);

    return true;
}

void ColorLookupTable::operator=(const ColorLookupTable& other)
{
    copyTableData(other);
}

bool ColorLookupTable::operator==(const ColorLookupTable& other) const
{
    const quint16 *p1 = &m_table[0][0][0][0];
    const quint16 *p2 = &other.m_table[0][0][0][0];
    for (uint i = 0; i < CLUT_ELEMENT_COUNT; ++i)
        if (*(p1++) != *(p2++))
            return false;
    return true;
}


/*
 * Region Color Lookup Table
 */

RegionColorLookupTable::RegionColorLookupTable(bool initializeWithDummy)
    : ColorLookupTable(initializeWithDummy)
{

}

RegionColorLookupTable::RegionColorLookupTable(uint windowId, int outputIndex, 
        const QRect& region, bool initializeWithDummy)
    : ColorLookupTable(initializeWithDummy)
    , m_windowId(windowId)
    , m_outputIndex(outputIndex)
    , m_region(region)
{

}


RegionColorLookupTable::~RegionColorLookupTable()
{

}

uint RegionColorLookupTable::windowId() const
{
    return m_windowId;
}

void RegionColorLookupTable::setWindowId(uint windowId)
{
    m_windowId = windowId;
}

int RegionColorLookupTable::outputIndex() const
{
    return m_outputIndex;
}

void RegionColorLookupTable::setOutputIndex(int outputIndex)
{
    m_outputIndex = outputIndex;
}

QRect RegionColorLookupTable::region() const
{
    return m_region;
}

void RegionColorLookupTable::setRegion(const QRect& region)
{
    m_region = region;
}


/*
 * Color Lookup Table Shared Memory Pool
 */
ColorLookupTablePool * ColorLookupTablePool::m_instance = NULL;

ColorLookupTablePool * ColorLookupTablePool::instance()
{
    if (!m_instance)
        m_instance = new ColorLookupTablePool(NULL);
    return m_instance;
}

void ColorLookupTablePool::cleanup()
{
    delete m_instance;
    m_instance = NULL;
}

ColorLookupTablePool::ColorLookupTablePool(QObject* parent)
    : QSharedMemory(parent)
    , m_elementCount(0)
    , m_capacity(4)
{
    kDebug();
    resetKey();
    if (!create(m_capacity * CLUT_DATA_SIZE, ReadWrite)) {
        kError() << "Unable to create the initial shared memory segment:" << errorString();
        m_capacity = 0;
    }
    emit ready();
}

ColorLookupTablePool::~ColorLookupTablePool()
{

}

void ColorLookupTablePool::resetKey()
{
    const QString newKey(QString("KolorServerClutPool-pid%1-cap%2").arg(QCoreApplication::applicationPid()).arg(m_capacity));
    setKey(newKey);
    emit keyChanged(newKey);
}

int ColorLookupTablePool::colorLookupTableIndex(const ColorLookupTable& c)
{
    if (m_elements.contains(c))
        return m_elements.value(c);
    else
        return -1;
}

void ColorLookupTablePool::addColorLookupTable(const ColorLookupTable& c)
{
    if (m_elements.contains(c) || m_capacity == 0)
        return;

    kDebug() << key() << "size" << m_elementCount << "<-- clut" << c.checksum();

    const bool needToExpand = m_elementCount >= m_capacity;

    m_elementCount++;
    if (needToExpand) {
        m_capacity *= 4;

        int oldSize = size();
        int newSize = m_capacity * CLUT_DATA_SIZE;

        void *dataCopy = malloc(oldSize);
        if (!dataCopy) {
            kError() << "Unable to allocate memory to preserve old data";
            return;
        }

        lock();
        memcpy(dataCopy, data(), oldSize);
        unlock();

        if (!detach()) {
            kError() << "Unable to detach from current shared memory segment";
            return;
        }

        resetKey();

        if (!create(newSize, ReadWrite)) {
            kError() << "Unable to create a bigger shared memory segment:" << errorString();
            return;
        }

        lock();
        memcpy(data(), dataCopy, oldSize);
        unlock();
        free(dataCopy);
    }

    m_elements.insert(c, m_elementCount - 1);

    // Copy data (at the position of the newest clut element)
    lock();
    memcpy((quint16*) data() + (m_elementCount - 1) * CLUT_ELEMENT_COUNT, c.table(), CLUT_DATA_SIZE);
    unlock();

    // Notify the clients that the new shared memory segment is ready and they should reattach
    if (needToExpand)
        emit ready();
}

const quint16* ColorLookupTablePool::colorLookupTableData(int index) const
{
    if (index < 0 || index >= m_elementCount)
        return NULL;
    return ((quint16*) data() + index * CLUT_ELEMENT_COUNT);
}


} // KolorServer namespace
