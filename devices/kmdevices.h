
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
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>

/*****************************************************************
              KMDevices Class Definition                         *
******************************************************************/

using namespace oyranos;

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

    // When the "Assign Profile" button is pressed...
    void openProfile(int index);
 
    // When user clicks on a device tree item.
    void changeDeviceItem( QTreeWidgetItem* );

    // When user clicks on a profile list item
    void profileListDoubleClicked( QListWidgetItem* );
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
    void assingProfile( QString & profile_name );
    
    // Refresh current profile list.
    void updateProfileList(oyConfig_s * device);

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

    // String used when user wants to add a new profile...
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

    // Point to device list column.
    QTreeWidgetItem * parent_monitor_item;
    QTreeWidgetItem * parent_printer_item;
    QTreeWidgetItem * parent_scanner_item;
    QTreeWidgetItem * parent_camera_item;
    QTreeWidgetItem * parent_other_item;
    
    bool listModified;                // Was the list changed by the user?

    // Global string values for Oyranos device identification 
    char * current_device_name;
    char * current_device_class;  
    void setCurrentDeviceName(const char * name)
    { if(current_device_name) free(current_device_name);
      current_device_name = strdup(name); };
    void setCurrentDeviceClass(const char * name)
    { if(current_device_name) free(current_device_class);
      current_device_class = strdup(name); };
};

#endif 
