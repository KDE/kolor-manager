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

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>

#include "screen.h"

#include "display.h"
#include "output.h"

#include <alpha/oyranos_alpha.h>


/**
 * Helper function to convert a MD5 into a readable string.
 */
static const char *md5string(const uint8_t md5[16])
{
    static char buffer[33] = { 0 };
    const uint32_t *h = (const uint32_t*) md5;

    snprintf(buffer, 33, "%x%x%x%x", h[0], h[1], h[2], h[3]);

    return buffer;
}


namespace KolorServer
{

/*
 * Screen
 */

Screen::Screen(X11::Display *display, int number, Display *parent)
    : m_parent(parent)
    , m_display(display)
    , m_screen(number)
{
#ifdef HAVE_XRANDR
    X11::setupXRandR(display, number);
#endif
}

Screen::~Screen()
{
    cleanOutputs();
}

X11::Display *Screen::display() const
{
    return m_display;
}

int Screen::screenNumber() const
{
    return m_screen;
}

X11::Window Screen::rootWindow() const
{
    return X11::rootWindow(m_display, m_screen);
}

const ClutList& Screen::outputCluts() const
{
    return m_outputCluts;
}

const ClutList& Screen::regionCluts() const
{
    return m_regionCluts;
}

const QList<QRect>& Screen::regions() const
{
    return m_regions;
}

void Screen::cleanProfileAtoms()
{
    qDebug() << Q_FUNC_INFO;
    for (int i = 0; i < m_outputs.size(); ++i)
        m_outputs[i]->cleanProfileAtom();
}

void Screen::cleanOutputs()
{
    qDebug() << Q_FUNC_INFO;
    for (int i = 0; i < m_outputs.size(); ++i)
        delete m_outputs[i];
    m_outputs.clear();
    m_outputCluts.clear();
}

void Screen::setupOutputs()
{
    qDebug() << Q_FUNC_INFO;

    // Cleanup old outputs
    cleanOutputs();

    int n = QApplication::desktop()->screenCount();
    m_outputs.clear();
    for (int i = 0; i < n; ++i)
        m_outputs << new ColorOutput(this, i);

    // Allow Oyranos to see modifications made to the process's Xlib context
    X11::XFlush(m_display);

    m_parent->clean(); // does not seem to be too nice
}

void Screen::updateOutputConfiguration(bool init)
{
    qDebug() << Q_FUNC_INFO << init;

    int error;
    oyOptions_s *options = 0;
    oyConfigs_s *devices = 0;
    oyConfig_s *device = 0;

    // Allow Oyranos to see modifications made to the process's Xlib context
    X11::XFlush(m_display);

    /*
     * Obtain device information, including geometry and ICC profiles
     * from the according Oyranos module
     */
    oyOptions_SetFromText(&options, "//" OY_TYPE_STD "/config/command", "list", OY_CREATE_NEW);
    oyOptions_SetFromText(&options, "//" OY_TYPE_STD "/config/device_rectangle", "true", OY_CREATE_NEW);
    error = oyDevicesGet(OY_TYPE_STD, "monitor", options, &devices);
    if(error > 0)
        qCritical() << "Unable to get devices, error" << error;
    oyOptions_Release(&options);

    if (Display::getInstance()->colorDesktopActivated()) {
        for (int i = 0; i < m_outputs.size(); ++i) {
            device = oyConfigs_Get(devices, i);
            m_outputs[i]->updateConfiguration(device, init);
            oyConfig_Release(&device);
        }
    }

    oyConfigs_Release(&devices);

    // Recreate output clut list
    m_outputCluts.clear();
    for (int i = 0; i < m_outputs.size(); ++i)
        m_outputCluts << m_outputs[i]->colorLookupTable();

    // TODO maybe some signals via dbus for this?
//     int all = 1;
//     forEachWindowOnScreen( s, damageWindow, &all );
}

void Screen::updateProfiles()
{
    qDebug() << Q_FUNC_INFO;

    /* Fetch the profiles */
    unsigned long nBytes;
    int screen = X11::defaultScreen(m_display);
    X11::Window rootWindow = X11::rootWindow(m_display, screen);
    void *data = fetchProperty(m_display, rootWindow, m_parent->iccColorProfiles, XA_CARDINAL, &nBytes, True);
    if (!data) {
        qDebug() << "No profiles from the applications to the color server";
        return;
    }

    uint32_t exact_hash_size = 0;
    oyHash_s *entry;
    oyProfile_s *oyProfile = NULL;
    oyStructList_s *cache = Display::getInstance()->cache();
    int n = 0;

    // Grow or shring the array as needed
    unsigned long count = X11::XcolorProfileCount(data, nBytes);

    // Copy the profiles into the array, and create the Oyranos handles
    XcolorProfile *xcmProfile = (XcolorProfile*) data;
    for (unsigned long i = 0; i < count; ++i) {
        const char *hash_text = md5string(xcmProfile->md5);
        entry = oyCacheListGetEntry_(cache, exact_hash_size, hash_text);
        oyProfile = (oyProfile_s *) oyHash_GetPointer(entry, oyOBJECT_PROFILE_S);

        /* XcolorProfile::length == 0 means the clients wants to delete the xcmProfile. */
        if (ntohl(xcmProfile->length)) {
            if (!oyProfile) {
                // Not found in cache, get from the data
                oyProfile = oyProfile_FromMem(htonl(xcmProfile->length), xcmProfile + 1, 0, NULL);
                if (!oyProfile) {
                    /*
                     * If creating the Oyranos xcmProfile fails, don't try to parse
                     * any further profiles and just quit.
                     */
                    qWarning() << "Could not create Oyranos xcmProfile" << hash_text;
                    goto updateProfiles_out;
                }

                oyHash_SetPointer(entry, (oyStruct_s*) oyProfile);
                ++ n;
            }
        }

        xcmProfile = X11::XcolorProfileNext(xcmProfile);
    }

    qDebug() << "Added" << n << "of" << count << "screen profiles";

updateProfiles_out:
    X11::XFree(data);
}

int Screen::profileCount() const
{
    int c = 0;
    for (int i = 0; i < m_outputs.size(); ++i)
        if (m_outputs[i]->profile())
            c ++;
    return c;
}

} // KolorServer namespace
