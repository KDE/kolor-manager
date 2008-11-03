/*

Copyright (C) 2008 Joseph Simon III <j.simon.iii@astound.net>

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

*/

#ifndef KMDEVICES_H
#define KMDEVICES_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>

class QPushButton;
class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;

#include "ui_kmdevices.h"     // Gui header.
#include <oyranos/oyranos_alpha.h>

using oyranos::oyPROFILE_e;


/*****************************************************************
              KMDevices Class Definition                         *
******************************************************************/


class kmdevices : public KCModule, Ui::kmdevices
{
    Q_OBJECT

public:    
    kmdevices(QWidget *parent, const QVariantList &);
    ~kmdevices();

    // Integers to keep track of available devices
    int numberOfDetectedMonitors;
    int numberOfDetectedPrinters;

   

public Q_SLOTS:

    // load the settings from the config
    virtual void load();

    // save the current settings
    virtual void save();

// User-defined QT slots.
private slots:
    
    // When the "Add Profile" button is pressed...
    void openProfile();
    
    // "Remove Profile" button press.
    void removeProfile();    
    
    // "Set Profile as Device Default" button press.
    void setDefaultItem();
    
    // When user clicks on a device list item.
    void changeDeviceItem(QTreeWidgetItem*);
    
    // When user clicks on a profile list item.
    void changeProfileItem(QListWidgetItem*);

private:
    
    // Add profile to list (opens file open dialog)
    void addProfileToList();

    // Search for available monitors and install default profile
    void detectMonitor();

    // Search for available printers and install profile for device.
    void detectPrinter();

    // Detect scanner 
    void detectScanner();

    // FIXME Add camera support.
    //void detectCamera();

    // Function to detect all devices/directories.
    void populateDeviceListing();

    // Populate device-specified profile combo box listing.
    void populateDeviceComboBox(icProfileClassSignature deviceSignature);

     // Function to convert directory string of a profile into a file name.
    void changeDefaultItem(QString selected_profile);
    
    // Function to refresh device list and node with the new default profile.
    void setDefaultProfile(QString new_default_profile); 

    // Refresh current profile list.
    void updateProfileList(); 
    
    // Convert profile filename into profile description (using Oyranos).
    QString convertFilenameToDescription(QString profileFilename);  

    // KConfig calls to save, load, and delete associated profiles.
    void addNewDeviceConfig(QString device_name);
    void addDeviceProfile(QString device_name, QString profile);
    void saveDefaultProfile(QString device_name, QString profile);
    bool isProfileDuplicate(QString, QStringList);
    void saveDeviceType(QString device_name, QString deviceType);
    void saveProfileSettings(QString device_name);

    // Functions to save and load "display" information for Oyranos monitor.
    void saveMonitorLocationInfo (QString device_name, const char * display);
    const char * loadMonitorLocationInfo(QString device_name);


// PRIVATE DATA MEMBERS
// -----------------------------------------------

    // Current user selected profile/device.
    QListWidgetItem * currentProfileItem;

    // String used when user wants to add a new profile...
    QString recentlyAddedProfile;

    // Directory name variables.
    QString profileDirectoryDefault;

    // Pointer used to store address of 'initial' device item widget.
    QTreeWidgetItem * deviceListPointer;
    // Pointer used to store address of 'recently clicked' device item widget.
    QTreeWidgetItem * currentDevice;

    // Point to device list column.
    QTreeWidgetItem * parent_monitor_item;
    QTreeWidgetItem * parent_printer_item;
    QTreeWidgetItem * parent_scanner_item;
    QTreeWidgetItem * parent_camera_item;
    
    bool listModified;                // Was the list changed by the user?
       
    KSharedConfigPtr m_config;        // KConfig pointer.
};

#endif 
