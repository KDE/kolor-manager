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

#ifndef KOLOR_SERVER_SCREEN_H_
#define KOLOR_SERVER_SCREEN_H_

#include "color-lookup-table.h"
#include "x11-helpers.h"

namespace KolorServer
{

class ColorOutput;
class Display;

/**
 * X11 Screen
 *
 * May have multiple outputs.
 *
 * \note PrivScreen
 *
 * \todo privatize some of those methods
 * \todo review the atoms and profiles stuff
 */
class Screen : public QObject
{
    Q_OBJECT

public:
    /**
     * \note pluginInitScreen
     * - init empty list for outputs
     * - init Xrandr stuff (like property notify)
     */
    Screen(X11::Display *display, int number, KolorServer::Display *parent);

    /**
     * \note pluginFiniScreen
     * \warning Only in CompICC:
     * - get all devices from Oyranos
     * - for each device call moveICCprofileAtoms
     * - call freeOutput
     *
     * Destroys all associated outputs.
     */
    virtual ~Screen();

    /**
     * \return The X11 display associated with this screen
     */
    X11::Display *display() const;

    /**
     * \return The X11 screen number
     */
    int screenNumber() const;

    /**
     * \return The X11 root window associated with this screen
     */
    X11::Window rootWindow() const;

    /**
     * \return A list with the color lookup tables for all the outputs.
     */
    const ClutList& outputCluts() const;

    /**
     * \return A map of region-specific lookup tables to window XID's.
     */
    const RegionalClutMap& regionCluts() const;

    /**
     * \note cleanDisplayProfiles
     * - get number of connected devices
     * - for each output, if it has a profile, clean it with cleanScreenProfile
     *
     * Used by: setupOutputs
     */
    void cleanProfileAtoms();

    /**
     * \note freeOutput
     * \warning Only in CompICC
     * - go through all contexts
     * - free their dst profiles (destroy them?)
     * - clear the contexts list
     *
     * Used by: setupOutputs, pluginFiniScreen
     */
    void cleanOutputs();

    /**
     * \note setupOutputs
     * \warning Only in CompICC
     * - call freeOutput
     * - call cleanDisplayProfiles
     * - redo contexts - init n empty contexts
     * - call cleanDisplay
     *
     * Used by: pluginHandleEvent on netDesktopGeometry, randr output change, initialize
     */
    void setupOutputs();

    /**
     * \note updateOutputConfiguration
     * - list devices using Oyranos
     * - for each device
     *  - on init, call getDeviceProfile
     *  - if a dst_profile appears on that context, call moveICCprofileAtoms
     *  - call setupOutputTable on that device
     *
     * Used by: pluginHandleEvent
     */
    void updateOutputConfiguration(bool init);

    /**
     * \note updateScreenProfiles
     * - get profiles data from display property (whole)
     * - separate them and iterate (with Xcolor)
     * - add them to the oyHash
     *
     * " Called when new profiles have been attached to the root window. Fetches
     * these and saves them in a local database."
     *
     * Used by: property notify event, iccColorProfiles atom
     *
     * "_ICC_COLOR_PROFILES:
     * Is used to upload color profiles to the compositing manager. Clients attach a
     * a list of one or more XColorProfile to the root window. Compositing manager then
     * fetches the list and saves the profiles in an internal database and deletes the
     * property."
     */
    void updateProfiles();

    /**
     * Updates the profile for an output deduced from \param atomName,
     * with the new profile data being in \param atom. Tries to conform
     * to the XCM spec.
     * \see Display::handleEvent
     */
    void updateProfileForAtom(const char *atomName, X11::Atom atom);

    /**
     * \return The number of outputs that have a valid profile set up.
     */
    int profileCount() const;

signals:
    void outputClutsChanged();

private:
    Display *m_parent;
    X11::Display *m_display;
    int m_screen;
    int icc_profile_flags;              ///< profile selection flags from oyProfile_s.h

    QList<ColorOutput*> m_outputs; // or contexts

private:
    ClutList m_outputCluts;
    RegionalClutMap m_regionCluts;     // TODO
};

} // KolorServer namespace

#endif // KOLOR_SERVER_SCREEN_H_
