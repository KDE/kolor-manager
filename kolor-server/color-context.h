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

#ifndef KOLOR_SERVER_CONTEXT_H_
#define KOLOR_SERVER_CONTEXT_H_

#include "color-lookup-table.h"

#include <QString>

struct oyConfig_s;
struct oyProfile_s;

namespace KolorServer
{

/**
 *  Used to create and use a color conversion.
 * \li lookup texture data
 * \li the source ICC profile for reference
 * \li the target profile for the used monitor.
 *
 * \note PrivColorContext
 */
class ColorContext
{
public:
    ColorContext();
    virtual ~ColorContext();

    /**
     * \return Source profile for this context
     */
    oyProfile_s *sourceProfile();

    /**
     * Set a custom source profile for this color context.
     * Any existing source profile will be discarded.
     */
    void setSourceProfile(oyProfile_s *profile);

    /**
     * \return Destination profile for this context
     */
    oyProfile_s *destinationProfile();

    /**
     * Set a custom destination profile for this color context.
     * Any existing destination profile will be discarded.
     */
    void setDestinationProfile(oyProfile_s *profile);

    /**
     * \return Name of the intended output device
     */
    const QString& outputName();

    /**
     * \return The color lookup table for this context
     */
    const ColorLookupTable& colorLookupTable() const;

    /**
     * Setup for an output with the given name.
     * - put cc.src_profile as sRGB (web)
     * - copy output name into cc
     * - call setupColourTable
     * \param name name for output
     */
    void setupForOutput(const QString &name);

    /**
     * Basically calls \ref setupColorLookupTable
     * \param outputName will set the output name to this
     */
    void setupForRegion(const QString &outputName);

    /**
     * Fetches the device profile for the given device.
     * Check out if the profile is sRGB (web thing), return error if so.
     *
     * \return True if successful
     */
    bool getDeviceProfile(oyConfig_s *device);

private:
    /**
     * \note setupColourTable
     * - prepare src_profile, dst_profile if necessary
     * - create oyImage_s for src_profile, dst_profile
     * - create oyConversion_s
     * - search for clut in cache, if not present do conversion
     * - probably the clut is properly set up after all this
     *
     * Used by: setupOutputTable
     *
     * \param advanced got from getDisplayAdvanced
     */
    void setupColorLookupTable(bool advanced);

private:
    oyProfile_s *m_srcProfile;          ///< the data profile or device link
    oyProfile_s *m_dstProfile;          ///< the monitor profile or none
    QString m_outputName;               ///< the intended output device

    /// Color lookup table (for color conversion)
    ColorLookupTable m_clut;
};

} // KolorServer namespace

#endif // KOLOR_SERVER_CONTEXT_H_
