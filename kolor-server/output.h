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

#ifndef KOLOR_SERVER_OUTPUT_H_
#define KOLOR_SERVER_OUTPUT_H_

#include "color-context.h"
#include "x11-helpers.h"

namespace KolorServer
{

class Screen;

/**
 * Handles an output color profile.
 * Output profiles are currently only fetched using XRandR.
 *
 * \note PrivColorOutput
 */
class ColorOutput
{
public:
    ColorOutput(Screen *parent, int index);
    virtual ~ColorOutput();

    /**
     * \return The destination profile for this output
     */
    oyProfile_s *profile();

    /**
     * Set a custom profile for this output (the embedded context's
     * destination profile). Any existing profile will be discarded.
     */
    void setProfile(oyProfile_s *profile);

    /**
     * \return Name of the output
     */
    const QString& name();

    /**
     * \return The color lookup table for this context
     */
    const Clut& colorLookupTable() const;

    /**
     * \note hasScreenProfile
     * - get a magic atom name from the output index
     * - fetch the data from the atom
     * - return true if not empty
     *
     * Used by: cleanDisplayProfiles
     */
    bool hasProfileAtom();

    /**
     * \note cleanScreenProfile
     * - get a magic atom name from the output index
     * - delete the atom
     *
     * Used by: cleanDisplayProfiles
     */
    void cleanProfileAtom();

    /**
     * \param init operate in initialization mode
     * \param device device (output) to be used for this color output
     * In init mode, first grabs the device profile.
     * Moves profile atoms according to XCM spec.
     * Finally, configures the color lookup table.
     */
    void updateConfiguration(oyConfig_s *device, bool init);

    /**
     * \return The right X11 atom for the output given by \param num.
     */
    static X11::Atom iccProfileAtom(X11::Display *display, int num, bool forServer);

private:
    /**
     * \note getDeviceProfile
     * - get some device rectangle and setup the output's rect
     * - get the device name and setup the output's name
     * - release old cc.dst_profile
     * - get the cc.dst_profile from the specified device
     *
     * Used by: updateOutputConfiguration (on init)
     *
     * \return true on success
     */
    bool getDeviceProfile(oyConfig_s *device);

    /**
     * \note setupOutputTable
     *
     * Used by: updateOutputConfiguration for all contexts
     */
    void setup();

    /**
     * \note moveICCprofileAtoms
     * - get magic atom names for both x_base and color_server things
     * - on init copy a -> da, else copy da -> a
     * - fetch source data from atom
     * - fetch dest data from atom
     * - on init call updateNetColorDesktopAtom
     * - set data in target atom copy from source data
     * - on init destabilize source atom with "screen document profile" data
     * - else just delete it
     *
     * Used by: updateOutputConfiguration, pluginFiniScreen (destructor?)
     */
    void moveProfileAtoms(bool init);

private:
    Screen *m_parent;
    int m_index;
    QString m_name;
    ColorContext m_cc;
    QRect m_rect;
};

} // KolorServer namespace

#endif // KOLOR_SERVER_OUTPUT_H_
