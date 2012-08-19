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

    // TODO doc
    const ClutList& outputCluts() const;
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

    // TODO doc
    void updateProfileForAtom(const char *atomName, X11::Atom atom);

    // TODO doc
    int profileCount() const;

signals:
    void outputClutsChanged();

private:
    Display *m_parent;
    X11::Display *m_display;
    int m_screen;

    QList<ColorOutput*> m_outputs; // or contexts

private:
    ClutList m_outputCluts;
    RegionalClutMap m_regionCluts;     // TODO
};

} // KolorServer namespace

#endif // KOLOR_SERVER_SCREEN_H_
