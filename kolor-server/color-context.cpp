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

#include <KDebug>

#include "color-context.h"

#include "display.h"

#include <oyranos_devices.h>
#include <oyConversion_s.h>

namespace KolorServer
{

/*
 * Color Context
 */

ColorContext::ColorContext()
    : m_srcProfile(NULL)
    , m_dstProfile(NULL)
{
    buildDummyClut(m_clut);
}

ColorContext::~ColorContext()
{
    // Release profiles
    if (m_srcProfile)
        oyProfile_Release(&m_srcProfile);
    if (m_dstProfile)
        oyProfile_Release(&m_dstProfile);
}

oyProfile_s *ColorContext::sourceProfile()
{
    return m_srcProfile;
}

void ColorContext::setSourceProfile(oyProfile_s *profile)
{
    oyProfile_Release(&m_srcProfile);
    m_srcProfile = profile;
}

oyProfile_s *ColorContext::destinationProfile()
{
    return m_dstProfile;
}

void ColorContext::setDestinationProfile(oyProfile_s *profile)
{
    oyProfile_Release(&m_dstProfile);
    m_dstProfile = profile;
}

const QString& ColorContext::outputName()
{
    return m_outputName;
}

const Clut& ColorContext::colorLookupTable() const
{
    return m_clut;
}

void ColorContext::setupColorLookupTable(bool advanced)
{
    kDebug() << m_outputName;

    oyProfile_s *dummyProfile = 0;
    oyOptions_s *options = 0;

    if (!m_dstProfile)
        m_dstProfile = dummyProfile = oyProfile_FromStd(oyASSUMED_WEB, 0);

    /* skip dummyProfile to dummyProfile conversion */
    if (!m_srcProfile && dummyProfile) {
        if (dummyProfile)
            oyProfile_Release(&dummyProfile);
        return;
    }

    if (!m_srcProfile) {
        m_srcProfile = oyProfile_FromStd(oyASSUMED_WEB, 0);
        if (!m_srcProfile) {
            kError() << "Output" << m_outputName << ":" << "no assumed dummyProfile source profile";
            kWarning() << "Output" << m_outputName << "using dummy clut";
            buildDummyClut(m_clut);
            return;
        }
    }

    int error = 0;
    int flags = 0;

    // Optionally set advanced options from Oyranos
    if (advanced)
        flags = oyOPTIONATTRIBUTE_ADVANCED;

    // Allocate memory for clut data
    m_clut.resize(CLUT_ELEMENT_COUNT);

    kDebug() << "Color conversion for" << m_outputName << "flags" << flags << (advanced ? "advanced" : "");
    oyImage_s *imageIn = oyImage_Create(
        LUT_GRID_POINTS,
        LUT_GRID_POINTS * LUT_GRID_POINTS,
        m_clut.data(),
        OY_TYPE_123_16,
        m_srcProfile,
        0);
    oyImage_s *imageOut = oyImage_Create(
        LUT_GRID_POINTS,
        LUT_GRID_POINTS * LUT_GRID_POINTS,
        m_clut.data(),
        OY_TYPE_123_16,
        m_dstProfile,
        0);

    oyConversion_s *conversion = oyConversion_CreateBasicPixels(imageIn, imageOut, options, 0);
    if (!conversion) {
        kWarning() << "No conversion created for" << m_outputName;
        if (dummyProfile)
            oyProfile_Release(&dummyProfile);
        return;
    }
    oyOptions_Release(&options);

    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/display_mode", "1", OY_CREATE_NEW);
    if (error) {
        kWarning() << "Oy options error:" << error;
        if (dummyProfile)
            oyProfile_Release(&dummyProfile);
        return;
    }
    error = oyConversion_Correct(conversion, "//"OY_TYPE_STD"/icc", flags, options);
    if (error) {
        kWarning() << "Failed to correct conversion for" << m_outputName << "flags" << flags;
        if (dummyProfile)
            oyProfile_Release(&dummyProfile);
        return;
    }

    oyFilterGraph_s *conversionGraph = oyConversion_GetGraph(conversion);
    oyFilterNode_s *iccNode = oyFilterGraph_GetNode(conversionGraph, -1, "///icc", 0);

    // See what to search for in the cache
    QByteArray entryText;
    const char *t = oyFilterNode_GetText(iccNode, oyNAME_NAME);
    if (t)
        entryText = t;

    oyStructList_s *cache = Display::getInstance()->cache();
    oyHash_s *entry = oyStructList_GetHash(cache, 0, entryText.constData());
    oyArray2d_s *oyClut = (oyArray2d_s*) oyHash_GetPointer(entry, oyOBJECT_ARRAY2D_S);
    char ** array2d = (char**)oyArray2d_GetData( oyClut );

    oyFilterNode_Release(&iccNode);
    oyFilterGraph_Release(&conversionGraph);

    if (oyClut) {
        // Found in cache
        kDebug() << "clut" << oyClut << "obtained from cache using entry" << entryText;
        memcpy(m_clut.data(), array2d[0], CLUT_DATA_SIZE);
    } else {
        kDebug() << "clut not found in cache using entry" << entryText << ", doing conversion";

        // Create dummy / identity clut data for conversion input
        buildDummyClut(m_clut);

        // Do conversion
        error = oyConversion_RunPixels(conversion, 0);
        if (error) {
            kWarning() << "Output" << m_outputName << "Error" << error << "in conversion run pixels";
            if (dummyProfile)
                oyProfile_Release(&dummyProfile);
            return;
        }

        // Save to cache
        oyClut = oyArray2d_Create(
            NULL,
            LUT_GRID_POINTS * 3,
            LUT_GRID_POINTS * LUT_GRID_POINTS,
            oyUINT16,
            NULL);
        array2d = (char**)oyArray2d_GetData( oyClut );
        memcpy(array2d[0], m_clut.data(), CLUT_DATA_SIZE);
        oyHash_SetPointer(entry, (oyStruct_s*) oyClut);
    }

    oyOptions_Release(&options);
    oyImage_Release(&imageIn);
    oyImage_Release(&imageOut);
    oyConversion_Release(&conversion);

    if (!m_dstProfile)
        kDebug() << "Output" << m_outputName << "no profile";
}

void ColorContext::setupForOutput(const QString &name)
{
    kDebug();
    if (!Display::getInstance()->colorDesktopActivated())
        return;

    m_srcProfile = oyProfile_FromStd(oyASSUMED_WEB, 0);
    m_outputName = name;
    if (!m_srcProfile)
        kWarning() << "Output" << name << "no sRGB source profile";

    setupColorLookupTable(Display::getInstance()->isAdvancedIccDisplay());
}

void ColorContext::setupForRegion(const QString& outputName)
{
    m_outputName = outputName;
    setupColorLookupTable(Display::getInstance()->isAdvancedIccDisplay());
}

bool ColorContext::getDeviceProfile(oyConfig_s *device)
{
    kDebug() << device;

    oyProfile_Release(&m_dstProfile);

    oyOptions_s *options = 0;
    oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/command", "list", OY_CREATE_NEW);
    oyOptions_SetFromText(&options,
        "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
        "yes", OY_CREATE_NEW );
    int error = oyDeviceGetProfile(device, options, &m_dstProfile);
    oyOptions_Release(&options);

    if (m_dstProfile) {
        /* check that no sRGB is delivered */
        if (error) {
            oyProfile_s *dummyProfile = oyProfile_FromStd(oyASSUMED_WEB, 0);
            if (oyProfile_Equal(dummyProfile, m_dstProfile)) {
                kWarning() << "Output" << m_outputName << "ignoring fallback, error" << error;
                oyProfile_Release(&m_dstProfile);
                error = 1;
            } else
                error = 0;
            oyProfile_Release(&dummyProfile);
        }
    } else {
        kWarning() << "Output" << m_outputName << ": no ICC profile found, error" << error;
        error = 1;
    }

    return error == 0;
}

} // KolorServer namespace
