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

#ifndef KOLOR_SERVER_DISPLAY_H_
#define KOLOR_SERVER_DISPLAY_H_

#include <QObject>

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
class Display : public QObject
{
    Q_OBJECT

    friend class KolorServer::Screen;

public:
    static Display *getInstance();
    static void cleanup();

    /**
     * \return The default screen for this display
     */
    KolorServer::Screen *screen() const;

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

    /**
     * Initializes the color server, mainly X11 related, like grab atoms, setup
     * event monitoring, etc.
     */
    void initialize();

    /**
     * \note pluginHandleEvent
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

private slots:
    void checkX11Events();

private:
    X11::Display *m_display;
    KolorServer::Screen *m_screen;

    X11::XcmeContext_s *m_xcmeContext;

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
