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

#include "output.h"
#include "display.h"
#include "window.h"

namespace X11 {
#include <X11/Xregion.h>
}

#include <oyProfile_s.h>
#include <oyStructList_s.h>

/**
 * Helper function to convert a MD5 into a readable string.
 */
static const char *md5string(const uint8_t md5[16])
{
    static char buffer[33] = {0};
    const uint32_t * h = (const uint32_t*)md5;

    sprintf( buffer, "%x%x%x%x", h[0],h[1],h[2],h[3]);

    return buffer;
}

static oyProfile_s *profileFromMD5(uint8_t *md5)
{
  uint32_t exact_hash_size = 0;
  oyProfile_s *prof = NULL;
  oyStructList_s *cache = KolorServer::Display::getInstance()->cache();

  /* Copy the profiles into the array, and create the Oyranos handles. */
  const char *hash_text = md5string(md5);
  prof = (oyProfile_s *) oyStructList_GetHashStruct(cache, exact_hash_size, hash_text, oyOBJECT_PROFILE_S);

  return prof;
}


namespace KolorServer
{

Window::Window(X11::Window id)
    : m_id(id)
{
    m_iccColorRegions = X11::XInternAtom(Display::getInstance()->m_display, XCM_COLOR_REGIONS, False);
}

Window::~Window()
{
    clearRegions();
}

X11::Window Window::id() const
{
    return m_id;
}

/*
 * The type is XA_CARDINAL and values are stored in network byte order.
 * _ICC_COLOR_OUTPUTS must not be intermixed with _ICC_COLOR_REGIONS on the same
 * window. Intermixing shall cause the colour server to fall back to sRGB."
 */
void Window::updateRegions(const QList<ColorOutput*> &outputs)
{
    kDebug() << "Updating regions for window" << m_id;

    // Clear the old regions
    clearRegions();

    unsigned long dataSize;
    void *data = X11::fetchProperty(Display::getInstance()->m_display, m_id,
            m_iccColorRegions, XA_CARDINAL, &dataSize, False);

    int regionCount = 0;
    if (data)
        regionCount += X11::XcolorRegionCount(data, dataSize + 1);

    X11::XcolorRegion *xcregion = (X11::XcolorRegion*) data; // data is a list of regions
    for (int i_region = 0; i_region < regionCount; ++i_region) {
        unsigned char zeroMd5[16] = { 0 };

        X11::Region xregion = X11::getRegionFromId(Display::getInstance()->m_display, ntohl(xcregion->region));
        WindowRegion region;

        // Sanity check for the region
        if (xregion->numRects < 0 || xregion->numRects > 0x1000) {
            kError() << "Invalid X Region, rect count" << xregion->numRects;
            continue;
        }

        /*
         * From Xregion.h:
         * typedef struct { short x1, x2, y1, y2;} Box, BOX, BoxRec, *BoxPtr;
         * typedef struct _XRegion { long size; long numRects; BOX *rects; BOX extents; } REGION;
         */
        for (int i_box = 0; i_box < xregion->numRects; ++i_box) {
            const X11::Box &b = xregion->rects[i_box];
            region.region |= QRect(QPoint(b.x1, b.y1), QPoint(b.x2, b.y2));
        }

        /*
         * If the region has a particular profile, create specific color contexts 
         * for that region, one for for each output
         */
        if (memcmp(xcregion->md5, zeroMd5, 16) != 0) {
            for (int i_output = 0; i_output < outputs.size(); ++i_output) {
                ColorContext *cc = new ColorContext;
                oyProfile_s *profile;

                profile = oyProfile_Copy(outputs[i_output]->profile(), 0);
                cc->setDestinationProfile(profile);

                profile = profileFromMD5(xcregion->md5);
                cc->setSourceProfile(profile);

                cc->setupForRegion(outputs[i_output]->name());

                region.contexts.append(cc);
            }
        }

        m_regions.append(region);
        xcregion = X11::XcolorRegionNext(xcregion);
    }

    kDebug() << "Regions updated for window" << m_id << "region count is" << m_regions.count();
}

int Window::regionCount() const
{
    return m_regions.count();
}

QRect Window::region(int index) const
{
    Q_ASSERT(index >= 0 && index < m_regions.count());
    return m_regions[index].region;
}

ColorContext* Window::regionColorContext(int regionIndex, int outputIndex) const
{
    Q_ASSERT(regionIndex >= 0 && regionIndex < m_regions.count());
    Q_ASSERT(outputIndex >= 0 && outputIndex < m_regions[regionIndex].contexts.count());
    return m_regions[regionIndex].contexts[outputIndex];
}

void Window::clearRegions()
{
    for (int i = 0; i < m_regions.size(); ++i) {
        for (int i_output = 0; i_output < m_regions[i].contexts.size(); ++i_output)
            delete m_regions[i].contexts[i_output];
        m_regions[i].contexts.clear();
    }
    m_regions.clear();
}

} // KolorServer namespace

#include "moc_window.cpp"
