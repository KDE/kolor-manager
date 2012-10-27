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
#include <QTimer>

#include <unistd.h>

#include "color-lookup-table.h"
#include "screen.h"

#include "display.h"

#include <oyranos_devices.h>
#include <X11/Xcm/XcmEvents.h>

namespace KolorServer
{

static const int X11_EVENTS_POLL_INTERVAL         = 4000; //ms

/*
 * Display
 */

Display *s_display = 0;

Display *Display::getInstance()
{
    if (!s_display) {
        s_display = new Display;
        s_display->initialize();
    }
    return s_display;
}

void Display::cleanup()
{
    delete s_display;
}

Display::Display()
    : m_screen(0)
    , m_xcmeContext(0)
    , m_colorDesktopActivated(true)
    , m_oyCache(0)
{
    // Determine the display name
    QByteArray displayName = qgetenv("DISPLAY");
    if (displayName.isEmpty())
        displayName = ":0";

    // Open connection to the X Server
    m_display = X11::XOpenDisplay(displayName.constData());
    if (!m_display) {
        kFatal() << "Cannot connect to X server" << displayName;
        activateColorDesktop(false);
        return;
    }
}

Display::~Display()
{
    if (!m_display)
        return;

    // Remove desktop colour management service mark
    X11::changeProperty(m_display, iccColorDesktop, XA_STRING, (unsigned char*) NULL, 0);

    // Delete default screen
    delete m_screen;

    // Uninit X11 Events monitoring
    if (m_xcmeContext)
        X11::XcmeContext_Release(&m_xcmeContext);

    // Delete cache
    oyStructList_Clear(m_oyCache);
    oyStructList_Release(&m_oyCache);

    // Close connection to the X Sever
    X11::XCloseDisplay(m_display);
}

void Display::initialize()
{
    // Setup X11 event monitoring
    m_xcmeContext = X11::XcmeContext_New();
    if (m_xcmeContext) {
        X11::XcmeContext_DisplaySet(m_xcmeContext, m_display);
        if (X11::XcmeContext_Setup(m_xcmeContext, ""))
            kWarning() << "Unable to setup X11 event monitor";
    } else
        kWarning() << "Unable to create X11 event monitor";

    // Setup a timer for polling for X11 events
    QTimer *eventTimer = new QTimer(this);
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(checkX11Events()));
    eventTimer->start(X11_EVENTS_POLL_INTERVAL);

    iccColorManagement  = X11::XInternAtom(m_display, "_ICC_COLOR_MANAGEMENT", False);

    iccColorProfiles    = X11::XInternAtom(m_display, XCM_COLOR_PROFILES, False);
    iccColorRegions     = X11::XInternAtom(m_display, XCM_COLOR_REGIONS, False);
    iccColorOutputs     = X11::XInternAtom(m_display, XCM_COLOR_OUTPUTS, False);
    iccColorDesktop     = X11::XInternAtom(m_display, XCM_COLOR_DESKTOP, False);
    netDesktopGeometry  = X11::XInternAtom(m_display, "_NET_DESKTOP_GEOMETRY", False);
    iccDisplayAdvanced  = X11::XInternAtom(m_display, XCM_COLOUR_DESKTOP_ADVANCED, False);

    // Get the default screen
    int screenNumber = X11::defaultScreen(m_display);
    m_screen = new Screen(m_display, screenNumber, this);
    m_screen->setupOutputs();
    m_screen->updateOutputConfiguration(true);

    // Initialize Oyranos Cache
    m_oyCache = oyStructList_New(0);
}

Screen * Display::screen() const
{
    return m_screen;
}

oyStructList_s* Display::cache()
{
    return m_oyCache;
}

bool Display::colorDesktopActivated() const
{
    return m_colorDesktopActivated;
}

void Display::activateColorDesktop (bool activate)
{
    m_colorDesktopActivated = activate;
}

void Display::clean()
{
    kDebug();

    int error;
    oyOptions_s *options = 0;
    oyConfigs_s *devices = 0;

    // Get display name
    QByteArray displayName = X11::XDisplayString(m_display);
    QByteArray t;
    if (displayName.contains('.'))
        displayName.resize(displayName.indexOf('.'));
    if (displayName.isEmpty())
        displayName = ":0";

    // Clean up old displays
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/command", "unset", OY_CREATE_NEW);
    for (int screen = 0; screen < 200; ++screen)
    {
        QByteArray screenName = displayName + "." + QByteArray::number(screen);
        error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/device_name",
                                      screenName.data(), OY_CREATE_NEW);
        error = oyDevicesGet(OY_TYPE_STD, "monitor", options, &devices);
        if (error)
            break;
        oyConfigs_Release(&devices);
    }
    oyOptions_Release(&options);


    /* get number of connected devices */
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/command", "list", OY_CREATE_NEW);
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/display_name", displayName.constData(), OY_CREATE_NEW);
    error = oyDevicesGet(OY_TYPE_STD, "monitor", options, &devices);
    oyConfigs_Release(&devices);
    oyOptions_Release(&options);

    /*
     * Monitor hotplugs can easily mess up the ICC profile to device assigment.
     * So first we erase the _ICC_PROFILE(_xxx) to get a clean state.
     * We setup the EDID atoms and ICC profiles new.
     * The ICC profiles are moved to the right places through the
     * PropertyChange events recieved by the colour server.
     */

    // Refresh EDID
    QByteArray screen0Name = displayName + ".0";
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/command", "list", OY_CREATE_NEW);
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/device_name", screen0Name.constData(), OY_CREATE_NEW);
    error = oyOptions_SetFromText(&options, "//"OY_TYPE_STD"/config/edid", "refresh", OY_CREATE_NEW);
    error = oyDevicesGet(OY_TYPE_STD, "monitor", options, &devices);
    oyConfigs_Release(&devices);
    oyOptions_Release(&options);
}

int Display::updateNetColorDesktopAtom(bool init)
{
    static time_t lastUpdateTime = 0;

    enum status_e {
        statusOk                = 0,
        statusInactive          = 1,
        statusActive            = 2,
        statusError             = 3
    } status = statusOk;
    const QByteArray myID = "kolorserver";
    const QByteArray myCaps = "|ICP|ICO|V0.4|"; // TODO add other capabilities?

    if (!colorDesktopActivated())
        return (int) statusInactive;

    kDebug() << init;

    X11::Window rootWindow = X11::rootWindow(m_display, 0);
    unsigned long n = 0;
    char *data = (char*) X11::fetchProperty(m_display, rootWindow, iccColorDesktop, XA_STRING, &n, False);

    pid_t pid = getpid();
    const char *oldData = 0;
    int oldPid = 0;
    long atomTime = 0;
    time_t currentTime = time(NULL);
    QByteArray colorServerAtomName(1024, '\0');
    QByteArray capabilitiesAtomText(1024, '\0');

    if (n && data && strlen(data)) {
        sscanf((const char*) data, "%d %ld %s %s",
               &oldPid,
               &atomTime,
               capabilitiesAtomText.data(),
               colorServerAtomName.data());
        oldData = data;
    }

    const QByteArray otherSrv = oldData ? oldData : "????";

    if (n && data && oldPid != (int) pid) {
        if (oldData && currentTime - atomTime > 60) {
            kWarning() << "Found old _ICC_COLOR_DESKTOP:" << otherSrv;
            kWarning() << "Either there was a previous crash or your setup may be double color corrected";
        }

        // Check for taking over of colour service
        if (colorServerAtomName != myID) {
            if (atomTime < lastUpdateTime || init) {
                kDebug() << "Taking over color service from old _ICC_COLOR_DESKTOP:" << otherSrv;
            } else {
                if (atomTime > lastUpdateTime) {
                    kDebug() << "Giving color service to _ICC_COLOR_DESKTOP:" << otherSrv;
                    activateColorDesktop(false);
                }
            }
        } else {
            kDebug() << "Taking over color service from old _ICC_COLOR_DESKTOP:" << otherSrv;
        }
    }

    if (lastUpdateTime - atomTime > 10 || init) {
        QByteArray newData(1024, '\0');
        snprintf(newData.data(), 1024, "%d %ld %s %s", (int) pid, (long) currentTime, myCaps.constData(), myID.constData());

        // Set the colour management desktop service activity atom
        if (m_screen->profileCount() > 0)
            X11::changeProperty(m_display, iccColorDesktop, XA_STRING, (const unsigned char*) newData.constData(), newData.size());
        else if (oldData) {
            /* switch off the plugin */
            X11::changeProperty(m_display, iccColorDesktop, XA_STRING, (const unsigned char*) NULL, 0);
            activateColorDesktop(false);
        }
    }

    lastUpdateTime = currentTime;

    return status;
}

bool Display::isAdvancedIccDisplay()
{
    unsigned long nBytes;
    char *opt = 0;
    bool advanced = false;
    X11::Window rootWindow = X11::rootWindow(m_display, 0);

    // Optionally set advanced options from Oyranos
    opt = (char*) X11::fetchProperty(m_display, rootWindow, iccDisplayAdvanced, XA_STRING, &nBytes, False);
    kDebug() << "iccDisplayAdvanced, nBytes:" << nBytes;
    if (opt && nBytes && atoi(opt) > 0)
        advanced = atoi(opt) != 0;
    if (opt)
        X11::XFree(opt);

    return advanced;
}

void Display::handleEvent(X11::XEvent* event)
{
    const char *atomName = 0;

    if (!colorDesktopActivated())
        return;

    switch (event->type) {
    case PropertyNotify:
        atomName = X11::XGetAtomName(event->xany.display, event->xproperty.atom);

        if (event->xproperty.atom == iccColorProfiles) {
            kDebug() << "ICC Color Profiles atom changed";
            m_screen->updateProfiles();
        } else if (event->xproperty.atom == iccColorRegions) {
            kDebug() << "ICC Color Regions atom changed";
            // CompWindow *w = findWindowAtDisplay(d, event->xproperty.window);
            // updateWindowRegions(w);
            // colour_desktop_region_count = -1;
            // TODO
        } else if (event->xproperty.atom == iccColorOutputs) {
            kDebug() << "ICC Color Outputs atom changed";
            // CompWindow *w = findWindowAtDisplay(d, event->xproperty.window);
            // updateWindowOutput(w);
            // TODO
        } else if (event->xproperty.atom == iccColorDesktop && atomName) {
            // Possibly let others take over the colour server
            kDebug() << "ICC Color Desktop atom changed";
            updateNetColorDesktopAtom(false);
        } else if (strstr(atomName, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0) {
            // Update for a changing monitor profile
            kDebug() << "ICC Output Profile atom changed";
            m_screen->updateProfileForAtom(atomName, event->xproperty.atom);
        } else if (event->xproperty.atom == netDesktopGeometry) {
            // Update for changing geometry
            kDebug() << "Desktop geometry atom changed";
            m_screen->setupOutputs();
            m_screen->updateOutputConfiguration(true);
        } else if (event->xproperty.atom == iccDisplayAdvanced) {
            kDebug() << "ICC Display Advanced atom changed";
            m_screen->updateOutputConfiguration(false);
        }

        break;

    case ClientMessage:
        if (event->xclient.message_type == iccColorManagement)
        {
            kDebug() << "ICC Color Management atom changed";
            // CompWindow *w = findWindowAtDisplay (d, event->xclient.window);
            // PrivWindow *pw = compObjectGetPrivate((CompObject *) w);
            // pw->active = 1;
            // TODO
        }
        break;

    case RRNotify: {
        X11::XRRNotifyEvent *rrn = (X11::XRRNotifyEvent *) event;
        if (rrn->subtype == RRNotify_OutputChange) {
            kDebug() << "XRandR outputs changed";
            m_screen->setupOutputs();
            m_screen->updateOutputConfiguration(true);
        }
        break;
    }

    default:
        break;
    }
}

void Display::checkX11Events()
{
    X11::XEvent event;
    long eventMask = ExposureMask | PropertyChangeMask;

    while (X11::XCheckMaskEvent(m_display, eventMask, &event) == True) {
        X11::XcmeContext_InLoop(m_xcmeContext, &event);
        handleEvent(&event);
    }
}


} // KolorServer namespace

#include "moc_display.cpp"
