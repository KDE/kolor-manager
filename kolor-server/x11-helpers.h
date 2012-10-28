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

#ifndef X11_HELPERS_H_
#define X11_HELPERS_H_

namespace X11 {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xdefs.h>

#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrandr.h>

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
    kDebug() << "looking for " << X11::XGetAtomName( dpy,prop ) << " found:" << *n;
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
    kDebug() << "Change " << X11::XGetAtomName( display,target_atom ) << " size:" << size;
}

/**
 * Generic function to remove a property
 */
static inline void deleteProperty(
    Display *display,
    Atom     atom )
{
    XDeleteProperty(display, RootWindow(display, 0), atom);
    kDebug() << "Remove " << X11::XGetAtomName( display,atom );
}

static inline Window rootWindow(Display *display, int screen)
{
    return RootWindow(display, screen);
}

static inline int defaultScreen(Display *display)
{
    return DefaultScreen(display);
}

static inline void setupXRandR(Display *display, int screen)
{
    XRRSelectInput(display, RootWindow(display, screen), RROutputPropertyNotifyMask);
    /*
     * TODO other notify masks? RROutputChangeMask?
     * http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
     */
}

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
