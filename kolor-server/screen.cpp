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
#include <QApplication>
#include <QDesktopWidget>

#include "screen.h"

#include "display.h"
#include "output.h"

#include <oyranos_devices.h>


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
    : QObject(parent)
    , m_parent(parent)
    , m_display(display)
    , m_screen(number)
{
    X11::setupXRandR(display, number);
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

const RegionalClutMap& Screen::regionCluts() const
{
    return m_regionCluts;
}

void Screen::cleanProfileAtoms()
{
    kDebug();
    for (int i = 0; i < m_outputs.size(); ++i)
        m_outputs[i]->cleanProfileAtom();
}

void Screen::cleanOutputs()
{
    kDebug();
    for (int i = 0; i < m_outputs.size(); ++i)
        delete m_outputs[i];
    m_outputs.clear();
    m_outputCluts.clear();
}

void Screen::setupOutputs()
{
    kDebug();

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
    kDebug() << init;

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
        kError() << "Unable to get devices, error" << error;
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

    emit outputClutsChanged();
}

void Screen::updateProfiles()
{
    kDebug();

    /* Fetch the profiles */
    unsigned long nBytes;
    int screen = X11::defaultScreen(m_display);
    X11::Window rootWindow = X11::rootWindow(m_display, screen);
    void *data = X11::fetchProperty(m_display, rootWindow, m_parent->iccColorProfiles, XA_CARDINAL, &nBytes, True);
    if (!data) {
        kDebug() << "No profiles from the applications to the color server";
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
        entry = oyStructList_GetHash(cache, exact_hash_size, hash_text);
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
                    kWarning() << "Could not create Oyranos xcmProfile" << hash_text;
                    goto updateProfiles_out;
                }

                oyHash_SetPointer(entry, (oyStruct_s*) oyProfile);
                ++ n;
            }
        }

        xcmProfile = X11::XcolorProfileNext(xcmProfile);
    }

    kDebug() << "Added" << n << "of" << count << "screen profiles";

updateProfiles_out:
    X11::XFree(data);
}

void Screen::updateProfileForAtom(const char *atomName, X11::Atom atom)
{
    int screen = 0;
    bool ignoreProfile = false;
    unsigned long n = 0;

    Atom csAtom;
    QByteArray colorServerProfileAtom = OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE;
    if (strlen(atomName) > (size_t) colorServerProfileAtom.size() + 1)
        sscanf((const char*) atomName, OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE"_%d", &screen);
    if (screen) {
        colorServerProfileAtom += "_";
        colorServerProfileAtom += QByteArray::number(screen);
    }

    csAtom = X11::XInternAtom(m_display, colorServerProfileAtom.constData(), False);
    if (csAtom) {
        void *data = X11::fetchProperty(m_display, X11::rootWindow(m_display, 0), atom, XA_CARDINAL, &n, False);
        if (data && n) {
            oyProfile_s *serverProfile = oyProfile_FromMem(n, data, 0,0);
            oyProfile_s *dummyProfile = oyProfile_FromStd(oyASSUMED_WEB, 0); // sRGB

            /* The distinction of sRGB profiles set by the server and ones
                * coming from outside the colour server is rather fragile.
                * So we ignore any sRGB profiles set into _ICC_PROFILE(_xxx).
                * The correct way to omit colour correction is to tag window
                * regions. As a last resort the colour server can be switched off.
                */
            if (oyProfile_Equal(serverProfile, dummyProfile)) {
                oyProfile_Release(&serverProfile);
                ignoreProfile = true;
            }
            oyProfile_Release(&dummyProfile);

            if (serverProfile) {
                if (screen < m_outputs.count()) {
                    m_outputs[screen]->setProfile(serverProfile);
                } else
                    kWarning() << "Contexts not ready for screen" << screen;

                X11::changeProperty(m_display, csAtom, XA_CARDINAL, (unsigned char *) 0, 0);
            }

            serverProfile = 0;
            X11::XFree(data);
        }
    }

    // Change only existing profiles, ignore removed ones
    if (!ignoreProfile && n) {
        updateOutputConfiguration(false);
    }
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

#include "moc_screen.cpp"

