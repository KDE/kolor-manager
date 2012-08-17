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

#include <QDebug>

#include "display.h"
#include "output.h"
#include "screen.h"

#include <alpha/oyranos_alpha.h>

namespace KolorServer
{

/*
 * Color Output
 */

ColorOutput::ColorOutput(Screen *parent, int index)
    : m_parent(parent)
    , m_index(index)
{
    qDebug() << Q_FUNC_INFO;
}

ColorOutput::~ColorOutput()
{
    qDebug() << Q_FUNC_INFO;

    if (profile()) {
        // Move profile atoms back
        moveProfileAtoms(false);
    }

    // Clean profile atoms
    if (hasProfileAtom())
        cleanProfileAtom();
}

oyProfile_s *ColorOutput::profile()
{
    return m_cc.destinationProfile();
}

const QString& ColorOutput::name()
{
    return m_name;
}

const Clut& ColorOutput::colorLookupTable() const
{
    return m_cc.colorLookupTable();
}

bool ColorOutput::hasProfileAtom()
{
    X11::Display *display = m_parent->display();
    X11::Window rootWindow = m_parent->rootWindow();
    X11::Atom a = iccProfileAtom(display, m_index, true);
    long unsigned int n;

    void *data = fetchProperty(display, rootWindow, a, XA_CARDINAL, &n, False);
    if (data)
        X11::XFree(data);

    return n != 0;
}

void ColorOutput::cleanProfileAtom()
{
    qDebug() << Q_FUNC_INFO;
    X11::Display *display = m_parent->display();
    X11::Window rootWindow = m_parent->rootWindow();
    X11::Atom a = iccProfileAtom(display, m_index, true);
    X11::XFlush(display);
    X11::XDeleteProperty(display, rootWindow, a);
}

void ColorOutput::updateConfiguration(oyConfig_s *device, bool init)
{
    qDebug() << Q_FUNC_INFO << device << init;

    if (init) {
        if (!m_cc.getDeviceProfile(device))
            qCritical() << "Unable to get device profile";
    }

    if (profile()) {
        moveProfileAtoms(true); // TODO really true?
    } else {
        qDebug() << "No profile found for output" << m_index << m_name;
    }

    setup();
}

X11::Atom ColorOutput::iccProfileAtom(X11::Display *display, int num, bool forServer)
{
    QByteArray atomName;

    if (forServer)
        atomName = OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE;
    else
        atomName = OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE;

    if (num) {
        atomName += "_";
        atomName += QByteArray::number(num);
    }

    return X11::XInternAtom(display, atomName.constData(), False);
}

bool ColorOutput::getDeviceProfile(oyConfig_s *device)
{
    qDebug() << Q_FUNC_INFO << device;

    oyOption_s *o = 0;
    oyRectangle_s *r = 0;
    const char *device_name = 0;
    int error = 0;

    o = oyConfig_Find(device, "device_rectangle");
    if (!o) {
        qWarning() << "Request for monitor rectangle failed";
        return false;
    }
    r = (oyRectangle_s*) oyOption_StructGet(o, oyOBJECT_RECTANGLE_S);
    if (!r) {
        qWarning() << "Request for monitor rectangle failed";
        return false;
    }
    oyOption_Release(&o);

    m_rect = QRect(r->x, r->y, r->width, r->height);

    device_name = oyConfig_FindString(device, "device_name", 0);
    if (device_name && device_name[0]) {
        m_name = device_name;
    } else {
        qWarning() << "Unable to get device name";
        m_name = QString::number(m_parent->screenNumber());
    }

    error = m_cc.getDeviceProfile(device);

    return !error;
}

void ColorOutput::setup()
{
    qDebug() << Q_FUNC_INFO;
    m_cc.setup(m_name);
}

/*
The atom will hold a native ICC profile with the exposed device
characteristics at the compositing window manager level.
The colour server shall if no _ICC_DEVICE_PROFILE(_xxx) is set, copy the
_ICC_PROFILE(_xxx) profiles to each equivalent _ICC_DEVICE_PROFILE(_xxx) atom.
The _ICC_PROFILE(_xxx) profiles shall be replaced by a sRGB ICC profile.
The counting in the atoms (_xxx) name section follows the rules outlined in
the ICC Profile in X recommendation. After finishing the session the the old
state has to be recovered by copying any _ICC_DEVICE_PROFILE(_xxx) atoms
content into the appropriate _ICC_PROFILE(_xxx) atoms and removing all
_ICC_DEVICE_PROFILE(_xxx) atoms.
The colour server must be aware about change property events indicating that
a _ICC_PROFILE(_xxx) atom has changed by a external application and needs to
move that profile to the appropriate _ICC_DEVICE_PROFILE(_xxx) atom and set
the _ICC_PROFILE(_xxx) atom to sRGB as well.
 */
void ColorOutput::moveProfileAtoms(bool init)
{
    qDebug() << Q_FUNC_INFO << init;

    X11::Display *display = m_parent->display();
    X11::Window rootWindow = m_parent->rootWindow();
    X11::Atom a1, a2, sourceAtom, targetAtom;

    a1 = iccProfileAtom(display, m_index, false);
    a2 = iccProfileAtom(display, m_index, true);

    // Select the atoms (source -> target)
    if (init) {
        sourceAtom = a1;
        targetAtom = a2;
    } else {
        sourceAtom = a2;
        targetAtom = a1;
    }

    void *source;
    unsigned long sourceSize = 0, targetSize = 0;
    bool sourceExists = false, targetExists = false;
    bool updatedColorDesktopAtom = false;

    fetchProperty(display, rootWindow, targetAtom, XA_CARDINAL, &targetSize, False);
    targetExists = targetSize > 0;

    if (!targetExists || !init) {
        // Copy the real device atom
        source = fetchProperty(display, rootWindow, sourceAtom, XA_CARDINAL, &sourceSize, False);
        sourceExists = sourceSize > 0;

        // _ICC_COLOR_DESKTOP atom is set before any _ICC_PROFILE(_xxx) changes.
        if (init) {
            Display::getInstance()->updateNetColorDesktopAtom(true);
            updatedColorDesktopAtom = true;
        }

        if (sourceExists)
            changeProperty(display, targetAtom, XA_CARDINAL, (const unsigned char *) source, sourceSize);
        if (source)
            X11::XFree(source);
        source = 0;
        sourceSize = 0;
        sourceExists = false;

        if (init) {
            /* setup the OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE(_xxx) atom as document colour space */
            size_t size = 0;
            oyProfile_s *screenDocumentProfile = oyProfile_FromStd(oyASSUMED_WEB, 0);

            if (screenDocumentProfile) {
                // Make sure the profile is ignored
                source = oyProfile_GetMem(screenDocumentProfile, &size, 0, malloc);
                oyProfile_Release(&screenDocumentProfile);
                sourceSize = size;
                sourceExists = sourceSize > 0;
            } else {
                qWarning() << "Could not get oyASSUMED_WEB profile";
                source = 0;
                sourceSize = 0;
                sourceExists = false;
            }

            if (!updatedColorDesktopAtom) {
                Display::getInstance()->updateNetColorDesktopAtom(true); // FIXME or false?
                updatedColorDesktopAtom = true;
            }

            if (sourceExists)
                changeProperty(display, sourceAtom, XA_CARDINAL, (const unsigned char *) source, sourceSize);
            if (source)
                free(source);
            source = 0;
            sourceSize = 0;
            sourceExists = false;
        } else {
            // Clear / erase the _ICC_DEVICE_PROFILE(_xxx) atom
            X11::XDeleteProperty(display, rootWindow, sourceAtom);
        }
    } else
        if (targetAtom && init)
            qWarning() << "ICC Color Server Atom already present:" << targetAtom << "size" << targetSize;
}

} // KolorServer namespace
