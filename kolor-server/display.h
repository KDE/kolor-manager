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

#ifndef KOLOR_SERVER_DISPLAY_H_
#define KOLOR_SERVER_DISPLAY_H_

#include "x11-helpers.h"

struct oyStructList_s;


namespace KolorServer
{

class Screen;

/**
 * Implements a color server conforming to the X Color Management
 * specification.
 *
 * \link http://oyranos.org/scm?p=xcolor.git;a=blob;f=docs/X_Color_Management.txt
 *
 * \note PrivDisplay
 */
class Display
{
    friend class KolorServer::Screen;

public:
    static Display *getInstance();
    static void cleanup();

    /**
     * \return Pointer to X11 display structure
     */
    X11::Display *x11Display() const;

    /**
     * \return The default screen for this display
     */
    Screen *screen() const;

    /**
     * \return Oyranos cache for profiles and other structures
     */
    oyStructList_s *cache();

    /**
     * \return Whether color desktop functionality is enabled
     */
    bool colorDesktopActivated() const;

    /**
     * Activates or deactivates color desktop functionality.
     * If it is deactivated, then the whole color server stands idle.
     */
    void activateColorDesktop(bool activate);

    /**
     * \note cleanDisplay
     * - perform some kind of get display name weirdness
     * - indicate that we want to clear up monitors from Oyranos
     * - and release everything (clean up old displays)
     * - get number of connected devices?
     * - refresh EDIDs (grab them from Oyranos)
     *
     * Used by: setupOutputs
     */
    void clean();

    /**
     * \note pluginHandleEvent
     * \todo
     * - handle PropertyNotify
     *  - iccColorProfiles
     *  - iccColorOutputs (not yet? for windows?)
     *  - iccColorDesktop
     *  - the weird icc target profile in x base
     *  - netDesktopGeometry
     *  - iccDisplayAdvanced
     * - handle ClientMessage, iccColorManagement (not yet? for windows?)
     * - handle RandR event, outputChange
     * - initialize stuff if necessary
     */
    void handleEvent(X11::XEvent *event);

    /**
     * \note updateNetColorDesktopAtom
     * Check and update the _ICC_COLOR_DESKTOP status atom. It is used to
     * communicate with the applications.
     *
     * The atom is attached on the root window to inform about the color server.
     * The content is of type XA_STRING and has four sections separated by a
     * empty space char ' '.
     * The _ICC_COLOR_DESKTOP atom is a string with following usages:
     * - uniquely identify the colour server
     * - tell the name of the colour server
     * - tell the colour server is alive
     * - list the colour server capabilities and spec compliance
     * All sections are separated by one space char ' ' for easy parsing.
     *
     * The first section contains the process id (pid_t) of the color server process,
     * which has set the atom.
     * The second section contains time since epoch GMT as returned by time(NULL).
     * The thired section contains the bar '|' separated and surrounded
     * capabilities:
     * - ICP  _ICC_COLOR_PROFILES XcolorRegion::md5 is handled
     * - ICT  _ICC_COLOR_TARGET - deprecated
     * - ICM  _ICC_COLOR_MANAGEMENT
     * - ICR  _ICC_COLOR_REGIONS XcolorRegion is handled
     * - ICO  _ICC_COLOR_OUTPUTS XcolorOutput is handled
     * - ICA  _ICC_COLOR_DISPLAY_ADVANCED
     * - V0.4 indicates version compliance to the _ICC_Profile in X spec
     * The fourth section contains the servers name identifier.
     *
     * @return                             - 0  all fine
     *                                     - 1  inactivate
     *                                     - 2  activate
     *                                     - 3  error
     *
     * Used by: moveICCprofileAtoms, pluginHandleEvent on iccColorDesktop
     */
    int updateNetColorDesktopAtom(bool init);

    /**
     * \note getDisplayAdvanced
     * - determine if some iccDisplayAdvanced atom is present
     * - return the result
     *
     * Used by: setupOutputTable
     *
     *  _ICC_COLOR_DISPLAY_ADVANCED:
     * The atom is optionaly attached to the root window. A value of "1" signals the
     * colour server to use advanced CMS options like proofing. The type is XA_STRING.
     */
    bool isAdvancedIccDisplay();

private:
    /**
     * \note pluginInitDisplay
     * - abort on no randrExtension
     * - grab the atoms
     * - use default screen for the display
     */
    Display();

    /**
     * \note pluginFiniDisplay
     * - destroy iccColorDesktop atom contents
     */
    virtual ~Display();

    // TODO doc
    void initialize();

    // TODO doc
    void updateProfiles();

private:
    X11::Display *m_display;
    Screen *m_screen;

    /* ClientMessage sent by the application */
    X11::Atom iccColorManagement;

    /* Window properties */
    X11::Atom iccColorProfiles;
    X11::Atom iccColorRegions;
    X11::Atom iccColorOutputs;
    X11::Atom iccColorDesktop;
    X11::Atom netDesktopGeometry;
    X11::Atom iccDisplayAdvanced;

    /**
     * Initially, the color desktop functionality is enabled, but
     * it can be later be disabled if there is another color server
     * running or there are other problems.
     */
    bool m_colorDesktopActivated;

    /**
     * Cache for color profiles and other structures
     */
    oyStructList_s *m_oyCache;
};

} // KolorServer namespace

#endif // KOLOR_SERVER_DISPLAY_H_
