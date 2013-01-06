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

#ifndef KOLOR_SERVER_WINDOW_H
#define KOLOR_SERVER_WINDOW_H

#include "x11-helpers.h"

#include <QList>
#include <QRect>

namespace KolorServer
{

class ColorContext;
class ColorOutput;

typedef struct WindowRegion
{
    QRect region;
    QList<ColorContext*> contexts;
} window_region_t;

class Window
{
public:
    Window(X11::Window id);
    virtual ~Window();

    /**
     * \return X11 specific window ID
     */
    X11::Window id() const;

    /**
     * Grab the regions contained in the _ICC_COLOR_REGIONS atom. Will create
     * the appropriate color conversions (color contexts) for the regions.
     * 
     * \param outputs Outputs for which separate color conversions should be
     * created. They define the destination profile. The source profile is given
     * by the region itself, and comes from the applications.
     */
    void updateRegions(const QList<ColorOutput*> &outputs);

    /**
     * \return the number of special color correction regions associated
     * with this window.
     */
    int regionCount() const;

    /**
     * \return The region area for the region given by \param index
     */
    QRect region(int index) const;

    /**
     * \return The color context for the region given by \param index
     */
    ColorContext * regionColorContext(int regionIndex, int outputIndex) const;

private:
    void clearRegions();

private:
    X11::Window m_id;
    QList<WindowRegion> m_regions;

    X11::Atom m_iccColorRegions;
};

} // KolorServer namespace

#endif // KOLOR_SERVER_WINDOW_H
