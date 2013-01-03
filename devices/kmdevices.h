/*
Copyright (C) 2008-2009 Joseph Simon III <j.simon.iii@astound.net>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions o9hband the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORTcd
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef KMDEVICES_H
#define KMDEVICES_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>
#include <QThread>

class QPushButton;
class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;

#include "ui_kmdevices.h"     // Gui header.
#include <oyranos_devices.h>

/*****************************************************************
              KMDevices Class Definition                         *
******************************************************************/


class kmdevices : public KCModule, Ui::kmdevices
{
    Q_OBJECT

public:
    kmdevices(QWidget *parent, const QVariantList &);
    ~kmdevices();

public Q_SLOTS:

    // load the settings from the config
    virtual void load();

    // save the current settings
    virtual void save();

// User-defined QT slots.
private slots:

    // When the "Assign Profile" button is pressed
    void openProfile(int index);

    // When user clicks on a device tree item.
    void changeDeviceItem( QTreeWidgetItem* );

    // Hitting the "Show only device related ICC profiles" button.
    void changeDeviceItem( int state );

    void installProfile();

    // obtain the Taxi DB finished event
    void getTaxiSlot( oyConfigs_s * );

    // get the ICC profile
    void downloadFromTaxiDB( );

private:

    // General device detection
    int detectDevices(const char * device_type);

    // Detect camera
    void detectRaw();

    // Function to detect all devices/directories.
    void populateDeviceListing();

    // Populate device-specified profile combo box listing.
    void populateDeviceComboBox(icProfileClassSignature deviceSignature);

     // Function to convert directory string of a profile into a file name.
    void changeDefaultItem(QString selected_profile);

    // Function to refresh device list and node with the new default profile.
    void setDefaultProfile(QString new_default_profile);

    // set the new profile to a Oyranos device
    void assignProfile( QString & profile_name );

    // get the actual device from currentDevice
    oyConfig_s * getCurrentDevice( void );

    // Convert profile filename into profile description (using Oyranos).
    QString convertFilenameToDescription(QString profileFilename);

    // KConfig calls to save, load, and delete associated profiles.
    void addNewDeviceConfig(QString device_name);
    void addDeviceProfile(QString device_name, QString profile);
    bool isProfileDuplicate(QString, QStringList);
    void saveDeviceType(QString device_name, QString deviceType);
    void saveProfileSettings(QString device_name);

    // Functions to save and load "display" information for Oyranos monitor.
    void saveMonitorLocationInfo (QString device_name, const char * display);
    const char * loadMonitorLocationInfo(QString device_name);


// PRIVATE DATA MEMBERS
// -----------------------------------------------

    // String used when user wants to add a new profile
    QString recentlyAddedProfile;

    // Directory name variables.
    QString profileDirectoryDefault;

    // Pointer used to store address of 'initial' device item widget.
    QTreeWidgetItem * deviceListPointer;
    enum {
      DEVICE_DESCRIPTION,
      DEVICE_NAME,
      PROFILE_DESCRIPTION,
      PROFILE_FILENAME
    };
    // Pointer used to store address of 'recently clicked' device item widget.
    QTreeWidgetItem * currentDevice;

    bool listModified;                // Was the list changed by the user?

    // Global string values for Oyranos device identification
    char * current_device_name;
    char * current_device_class;
    void setCurrentDeviceName(const char * name)
    { if(current_device_name) free(current_device_name);
      current_device_name = strdup(name); }
    void setCurrentDeviceClass(const char * name)
    { if(current_device_name) free(current_device_class);
      current_device_class = strdup(name); }
};

class TaxiLoad : public QThread
{
    Q_OBJECT

    oyConfig_s * d_;

        TaxiLoad( ) { d_ = 0; }
    public:
        TaxiLoad( oyConfig_s * device ) { d_ = device; }
        ~TaxiLoad( ) { }
     
    signals:
        void finishedSignal( oyConfigs_s * taxi_devices );
     
    protected:
        void run() {
            oyConfigs_s * taxi_devices = 0;
            if(d_)
              oyDevicesFromTaxiDB( d_, 0, &taxi_devices, 0);
            oyConfig_Release( &d_ );
            emit finishedSignal( taxi_devices );
        }
};

#endif
