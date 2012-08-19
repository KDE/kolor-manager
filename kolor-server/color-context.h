/********************************************************************
 KolorServer - color server based on the X Color Management Specification
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
    const Clut& colorLookupTable() const;

    /**
     * Setup for an output with the given name.
     * - put cc.src_profile as sRGB (web)
     * - copy output name into cc
     * - call setupColourTable
     */
    void setup(const QString &name);

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
    Clut m_clut;
};

} // KolorServer namespace

#endif // KOLOR_SERVER_CONTEXT_H_
