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

#ifndef X11_HELPERS_H_
#define X11_HELPERS_H_

namespace X11 {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xdefs.h>

#include <X11/extensions/Xfixes.h>

#define HAVE_XRANDR
#ifdef HAVE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>

} // X11 namespace

using X11::Atom;
using X11::XcolorProfile;
using X11::intmax_t;
using X11::uint8_t;
using X11::uint16_t;
using X11::uint32_t;
using X11::uintmax_t;

using X11::ntohl;
using X11::htonl;


/*
 * Helper functions
 */

namespace X11 {

/**
 * Generic function to fetch a window property.
 */
static inline void *fetchProperty(
    Display *dpy,
    Window w,
    Atom prop,
    Atom type,
    unsigned long *n,
    Bool del)
{
    Atom actual;
    int format;
    unsigned long left;
    unsigned char *data;

    XFlush(dpy);

    int result = XGetWindowProperty(dpy, w, prop, 0, ~0, del, type, &actual, &format, n, &left, &data);
    if (result == Success)
        return (void *) data;

    return NULL;
}

/**
 * Generic function to change a root window property
 */
static inline void changeProperty(
    Display *display,
    Atom target_atom,
    int type,
    const unsigned char *data,
    unsigned long size)
{
    XChangeProperty(display, RootWindow(display, 0),
        target_atom, type, 8, PropModeReplace,
        data, size);
}

static inline Window rootWindow(Display *display, int screen)
{
    return RootWindow(display, screen);
}

static inline int defaultScreen(Display *display)
{
    return DefaultScreen(display);
}

#ifdef HAVE_XRANDR
static inline void setupXRandR(Display *display, int screen)
{
    XRRSelectInput(display, RootWindow(display, screen), RROutputPropertyNotifyMask);
    /*
     * TODO other notify masks? RROutputChangeMask?
     * http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
     */
}
#endif // HAVE_XRANDR

// TODO FIXME? shouldn't these be exported by XColor in it's headers?
/* code from Tomas Carnecky */
static inline XcolorProfile *XcolorProfileNext(XcolorProfile *profile)
{
  unsigned char *ptr = (unsigned char *) profile;
  return (XcolorProfile *) (ptr + sizeof(XcolorProfile) + ntohl(profile->length));
}

static inline unsigned long XcolorProfileCount(void *data, unsigned long nBytes)
{
  unsigned long count = 0;
  XcolorProfile * ptr;

  for (ptr = (XcolorProfile*)data;
       (uintptr_t)ptr < (uintptr_t)data + nBytes;
       ptr = XcolorProfileNext(ptr))
    ++count;

  return count;
}
/* end of code from Tomas Carnecky */

} // X11 namespace

#endif // X11_HELPERS_H_
