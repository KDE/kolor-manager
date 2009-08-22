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

#include "kmdevices.h"

#include <KComponentData>
#include <KGenericFactory>
#include <KGlobal>
#include <KGlobalSettings>
#include <KInputDialog>
#include <KListWidget>
#include <KMessageBox>
#include <KStandardDirs>
#include <kio/netaccess.h>
#include <knewstuff2/engine.h>
#include <KPluginFactory>
#include <KPluginLoader>
#include <QFileDialog>
#include <KConfig>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <KConfig>
#include <KSharedConfigPtr>
#include <KAboutData>

#include <oyranos/oyranos.h>
#include <oyranos/oyranos_alpha.h>
#include <oyranos/oyranos_icc.h>
#include <oyranos/oyranos_cmm.h>

#define CONFIG_REGISTRATION ("//" OY_TYPE_STD "/config/command")

// Code to provide KDE module functionality for color manager.
K_PLUGIN_FACTORY( kmdevicesFactory, 
         registerPlugin<kmdevices>(); 
         )
K_EXPORT_PLUGIN( kmdevicesFactory("kmdevices") )

using namespace oyranos;

void kmdevices::load()
{
    
}

void kmdevices::save()
{
                 
}

kmdevices::kmdevices(QWidget *parent, const QVariantList &) :
    KCModule( kmdevicesFactory::componentData(), parent)
{       
     KAboutData* about = new KAboutData(
        "kmdevices", 0, ki18n("KMDevices"), 0, KLocalizedString(),
        KAboutData::License_GPL,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    
    setAboutData( about );

    current_device_name = 0;
    current_device_class = 0;

    listModified = false;       // Nothing has been modified yet...
  
    setupUi(this);              // Load Gui. 
 
    // Disable all buttons
    addProfileButton->setEnabled(false);
    removeButton->setEnabled(false);
    deviceProfileComboBox->setEnabled(false);    
    defaultProfileButton->setEnabled(false);

    // Set column width of device list.
    deviceList->setColumnWidth(0, 350);
    deviceList->setColumnWidth(1, 175);
    deviceList->setColumnWidth(2, 175);
    deviceList->setColumnWidth(3, 175);

    // Load directories and device listing.
    populateDeviceListing();   
    
    // Expand list for user.
    deviceList->expandAll();

    // QT-related Signal/Slot calls (for button-press and item selection events)si.
    connect(deviceList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(changeDeviceItem(QTreeWidgetItem*)));    
    connect(addProfileButton, SIGNAL(clicked()), this, SLOT(openProfile()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeProfile()));
    connect(profileAssociationList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(changeProfileItem(QListWidgetItem*)));
    connect(defaultProfileButton, SIGNAL(clicked()), this, SLOT(setDefaultItem()));

}

// Populate devices and profiles.
void kmdevices::populateDeviceListing()
{
     // TODO Work out a solution to use raw/camera stuff.
     detectRaw();  

     int error = 0;

     error = detectDevices("scanner");
     error = detectDevices("printer");
     error = detectDevices("monitor");     

}

// NEW General function to detect and retrieve devices via the Oyranos CMM backend.
int kmdevices::detectDevices(const char * device_type)
{ 
    oyConfigs_s * device_list = 0;
    oyOptions_s * options = oyOptions_New(0);

    oyOptions_SetFromText(&options, "//" OY_TYPE_STD "/config/command", 
                          "properties", OY_CREATE_NEW);

    oyDevicesGet( 0, device_type, 0, &device_list); 
        
    int j = 0, 
        device_num = oyConfigs_Count(device_list);

    // Must have at least one device detected to add to the list.
    if(device_num > 0)
    {        
        QIcon device_icon;
        QSize icon_size(30, 30);

        // Set up Kolor Manager gui "logistics" for a specified device.
        if(strcmp(device_type, "monitor") == 0)
        {
            device_icon.addFile( ":/resources/monitor.png", icon_size , QIcon::Normal, QIcon::On);

            parent_monitor_item = new QTreeWidgetItem;
            parent_monitor_item->setText(0, "Monitor(s)");
            deviceList->insertTopLevelItem(0, parent_monitor_item);
        }
        else if(strcmp(device_type, "printer") == 0)
        {
            device_icon.addFile( ":/resources/printer1.png", icon_size , QIcon::Normal, QIcon::On);

            parent_printer_item = new QTreeWidgetItem;
            parent_printer_item->setText(0, "Printer(s)");
            deviceList->insertTopLevelItem(0, parent_printer_item);
        }
        else if(strcmp( device_type, "scanner" ) == 0)
        {
            device_icon.addFile( ":/resources/scanner.png", icon_size , QIcon::Normal, QIcon::On);

            parent_scanner_item = new QTreeWidgetItem;
            parent_scanner_item->setText(0, "Scanner(s)");
            deviceList->insertTopLevelItem(0, parent_scanner_item);
        }

        // Traverse through the available devices 
        for (j = 0; j < device_num; j++)
        {
            QString deviceItemString, deviceProfileDescription;

            oyConfig_s * device = oyConfigs_Get(device_list, j);     
            oyDeviceBackendCall(device, options);          
            
            const char * device_manufacturer = 0;
            const char * device_model = 0;
            const char * device_serial = 0;   
            char * device_designation = 0;

            const char * profile_filename = 0;
            oyProfile_s * profile = 0;
            
            device_manufacturer = oyConfig_FindString( device, "manufacturer", 0);
            device_model = oyConfig_FindString( device, "model", 0);
            device_serial = oyConfig_FindString( device, "serial", 0);  

            oyDeviceGetInfo(device, oyNAME_NICK, 0, &device_designation, malloc);
            
            // A printer will only take a "device model"
            if (device_type != "printer")
            {
                deviceItemString.append(device_manufacturer);
                deviceItemString.append(" ");
            }

            deviceItemString.append(device_model);
            deviceItemString.append(" ");
            deviceItemString.append(device_serial);
 
            // NOTE Profiles are not functional for printers yet. 
            if (device_type != "printer")
            {
                // How do we properly set/unset devices
                //      with a new profile?

                oyDeviceGetProfile(device, &profile);
                oyDeviceUnset(device);
                profile_filename = oyProfile_GetFileName(profile, 0);
                oyDeviceSetup(device);
            }
            
            deviceListPointer = new QTreeWidgetItem();

            if (profile_filename == NULL)
            {
                deviceProfileDescription = "(No Profile Installed!)";
                profile_filename = "------";
            }
            else                               
                deviceProfileDescription = convertFilenameToDescription(profile_filename);
                         
            deviceListPointer->setIcon(0, device_icon);
            deviceListPointer->setText(0, deviceItemString);
            deviceListPointer->setText(1, device_designation);
            deviceListPointer->setText(2, deviceProfileDescription);   
            deviceListPointer->setText(3, profile_filename);
        
            if (strcmp(device_type, "monitor") == 0)
                parent_monitor_item->addChild(deviceListPointer);
            else if (strcmp(device_type, "printer") == 0)
                parent_printer_item->addChild(deviceListPointer);
            else if (strcmp(device_type, "scanner") == 0)
                parent_scanner_item->addChild(deviceListPointer);     

        }
     }   
     else
         return -1;    

     oyOptions_Release ( &options );
     oyConfigs_Release ( &device_list );
     
     return 0;
}

// Display an item that will open a dialog to use camera functionality.
void kmdevices::detectRaw()
{
    parent_camera_item = new QTreeWidgetItem;
    parent_camera_item->setText(0, "Camera(s)");
    deviceList->insertTopLevelItem(0, parent_camera_item);

    deviceListPointer = new QTreeWidgetItem(); 

    const char * dummy = "Click to add camera support...";

    deviceListPointer->setText(0, dummy);
 
    parent_camera_item->addChild(deviceListPointer);
}


//        ** SIGNAL/SLOT Related Functions **  

// When the user clicks on an item in the devices tree list.
void kmdevices::changeDeviceItem(QTreeWidgetItem * selected_device)
{     
    // Don't count top parent items as a "selected device".
    if (selected_device == parent_monitor_item ||
        selected_device == parent_printer_item ||
        selected_device == parent_scanner_item ||
        selected_device == parent_camera_item  )
    {         
         addProfileButton->setEnabled(false);                   
         removeButton->setEnabled(false);         
         deviceProfileComboBox->setEnabled(false);
         defaultProfileButton->setEnabled(false);
        
         return;
    }

    // The user modifies the list, but clicks away from the selected device item.
    if (listModified)   
    {
        if(KMessageBox::questionYesNo(this,
        i18n("The associated profiles list has changed. \nDo you wish to save it?"),
        i18n("Save device-associated profiles."))
           == KMessageBox::Yes)

        saveProfileSettings(currentDevice->text(0));

        listModified = false;
     }

    // If we click on a device item, the current device is stored and options are available.
    addProfileButton->setEnabled(true);
    removeButton->setEnabled(true);    
    deviceProfileComboBox->setEnabled(true);
    defaultProfileButton->setEnabled(true);

    // Change "Available Device Profiles" combobox to device-related profiles.
    if ( selected_device->parent() == parent_monitor_item )      
    {  
             current_device_class = "monitor";
             populateDeviceComboBox(icSigDisplayClass);             
    }
    // NOTE First, the profile need to be fixed in order to be functional.
    else if ( selected_device->parent() == parent_printer_item )
    {
            current_device_class = "printer";
            populateDeviceComboBox(icSigOutputClass);
            profileAssociationList->clear();
            return;  
    }
    
     // TODO Does icSigInputClass return any profile?
    else if ( selected_device->parent() == parent_scanner_item )
    {
             current_device_class = "scanner";
             populateDeviceComboBox(icSigInputClass);               
    }

    else if ( selected_device->parent() == parent_camera_item )
             populateDeviceComboBox(icSigInputClass);  

    currentDevice = selected_device;        
    
    // Convert QString to proper C string.
    QByteArray raw_string;
    raw_string = (currentDevice->text(1)).toLatin1();
    current_device_name = raw_string.data();
        
    // Get the device that the user selected.
    oyConfig_s * device = 0;     
    oyDeviceGet(0, current_device_class, current_device_name, 0, &device);
    
     // NOTE STILL NEEDS WORK
    updateProfileList(device); 
    oyConfig_Release(&device);
}

// Update profile association list every time a user clicks on a device item.
void kmdevices::updateProfileList(oyConfig_s * device)
{        
    int j, device_match;    
    int * rank;
    oyProfile_s * profile = 0;
    const char * profile_name = 0;

    profileAssociationList->clear();

    oyDeviceGetProfile( device, &profile );
    profile_name = oyProfile_GetText(profile, oyNAME_DESCRIPTION);
    
    profileAssociationList->addItem(profile_name);

    
/*   NOTE: Attempt at using oyConfig to load profile from database
              (will fail if un-commented)     */

/*

    oyConfigs_s * db_list = oyConfigs_New(0);
    oyConfig_s * comparison;
    
    oyConfigs_FromDB("//" OY_TYPE_STD, &db_list, 0);

    //oyDeviceGetProfile(configuration, &profile);
    //profile_filename = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
    
    for (j = 0; j < oyConfigs_Count( db_list ); j++)
    {
        comparison = oyConfigs_Get( db_list, j );
        device_match = oyConfig_Compare( device, comparison, rank);

        if(device_match == 0)
        {
            profile_name = oyConfig_FindString( device, "profile_name", 0);
            profile_name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
            profile_name = oyConfig_FindString( comparison, "profile_name", 0);
            if( profile_name )
                profileAssociationList->addItem(profile_name);
        }            
    }
   
  */
}


/*           
                              NOTE
(For right now in KM, 'saving' implies making a change on a profile list
       and clicking on the device again to be prompted to save.)

*/
// Save profile settings to oyranos DB file.
void kmdevices::saveProfileSettings(QString device_name)
{
    // Properly convert QString into C friendly string.
    QByteArray ba;
    ba = device_name.toLatin1();
    current_device_name = ba.data();  

    // Which type of device did the user select?  
    if ( currentDevice->parent() == parent_monitor_item)         
            current_device_class = "monitor";        
    // NOTE Not finished yet.
    else if ( currentDevice->parent() == parent_printer_item)  
    {              
            current_device_class = "printer";     
            return;       
    }
    else if ( currentDevice->parent() == parent_scanner_item)
            current_device_class = "scanner";
             
    oyConfig_s * device = 0;     
    oyDeviceGet(0, current_device_class, current_device_name, 0, &device);

    QListWidgetItem * temp_item;
    QString fileToDescriptionString;
    
    const char * current_profile_filename = 0;        
    int i = 0;

 /*  NOTE: Attempt at using oyConfig to load profile from database
           This doesn't work.
     */
/*
    // Save each device profile.
    for (i = 0; i < profileAssociationList->count(); i++)
    {
         temp_item = profileAssociationList->item(i);

         // Grab filename from list.
         fileToDescriptionString = temp_item->text();

         QByteArray ba;
         ba = (temp_item->text()).toLatin1();
         current_profile_filename = ba.data();

         int error = oyDeviceSetProfile( device, current_profile_filename );
         oyDeviceSetProfile (device, current_profile_filename);
    }

    oyDeviceSetup( device ); */

    return;
}

// Populate "Add Profile" combobox.  Depending on the device selected, the profile list will vary.
void kmdevices::populateDeviceComboBox(icProfileClassSignature deviceSignature)
{    
    int current = 0, size, i;
    oyProfile_s * profile = 0, * temp_profile = 0;
    oyProfiles_s * patterns = 0, * iccs = 0;

    profile = oyProfile_FromSignature( deviceSignature, oySIGNATURE_CLASS, 0 );
    patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

    iccs = oyProfiles_Create( patterns, 0 );
    
    QString getProfileDescription, profilePath;

    size = oyProfiles_Count(iccs);

    deviceProfileComboBox->clear(); 

    for( i = 0; i < size; ++i)
    {
         temp_profile = oyProfiles_Get( iccs, i );
         getProfileDescription = oyProfile_GetText( temp_profile, oyNAME_DESCRIPTION );
         profilePath = oyProfile_GetFileName( temp_profile, 0);

         getProfileDescription.append("\t(");
         getProfileDescription.append(profilePath);
         getProfileDescription.append(")");

         deviceProfileComboBox->addItem(getProfileDescription);
    }
}

// Add a new profile to the list.
void kmdevices::openProfile()
{   
    int parenthesis_index = 0, base_filename_index = 0, str_size = 0, i;        
    QString baseFileName = deviceProfileComboBox->currentText(),
            tempProfile;

    QListWidgetItem * temp_item = new QListWidgetItem;

    QString description = baseFileName;    
    parenthesis_index = baseFileName.indexOf("\t(");   

    // Clean-up full text in the deviceComboBox (we will o>setText(2)nly use the file name to
    //                                          add to the profiles list).
    str_size = baseFileName.size();
    baseFileName.remove(0, parenthesis_index + 2);
    baseFileName.remove(str_size - 2, 1);
    parenthesis_index = baseFileName.indexOf(")");
    baseFileName.remove(parenthesis_index, parenthesis_index + 1);
    base_filename_index = baseFileName.lastIndexOf("/");
    baseFileName.remove(0, base_filename_index + 1); 

    // Error if user adds a duplicate profile.
    for(i = 0; i < profileAssociationList->count(); i++)
    {
          temp_item = profileAssociationList->item(i);
          if(temp_item->text() == baseFileName)
          {
              KMessageBox::error(this,
                  i18n("Profile " + baseFileName.toLocal8Bit() + " is already associated with this device!"),
                  i18n("Cannot add profile to list"));

                  return;
          }
    }
     
     recentlyAddedProfile = baseFileName; 
     profileAssociationList->addItem(recentlyAddedProfile);

     emit changed(true); 
     listModified = true;
}

void kmdevices::removeProfile()
{    
    int item_to_remove_index = profileAssociationList->currentRow();
    QListWidgetItem * temp_item = profileAssociationList->item(item_to_remove_index);
    
    // If user tries to remove the default profile
    if(temp_item->text() == currentDevice->text(2))
    {
         KMessageBox::error(this,
                  i18n("You cannot remove the default profile!"),
                  i18n("Profile remove error."));

         return;
    }

    profileAssociationList->takeItem(item_to_remove_index);
    
    // The list has changed...
    emit changed(true);
    listModified = true;
  
}

// User clicks on a profile item...
void kmdevices::changeProfileItem(QListWidgetItem* selected_profile)
{    
    currentProfileItem = selected_profile;
    
    // If we select a profile, we make sure that 
    // we cannot save the same default profile twice...
    if (currentProfileItem->text() == currentDevice->text(2))
    {
         defaultProfileButton->setEnabled(false);  
         removeButton->setEnabled(false);
    }
    else
    {
         defaultProfileButton->setEnabled(true);  
         removeButton->setEnabled(true);
    }
}

// Set default profile.
void kmdevices::setDefaultItem()
{        
     
     oyProfile_s * profile_name;
     QString description;
    
     // Prompt user before setting profile as the default.
     if(KMessageBox::questionYesNo(this,
        i18n("Do you wish to save this profile as the device default?"),
        i18n("New Default Profile Request"))
           == KMessageBox::No)
     return;  

     QString defaultProfile = currentProfileItem->text();
 
     // Convert filename into description name...
     profile_name = oyProfile_FromFile(defaultProfile.toLocal8Bit(), 0, 0);
     description = oyProfile_GetText(profile_name, oyNAME_DESCRIPTION);
    
     // Set third column of the device list with the profile description.
     currentDevice->setText(1, description);
     
     //saveDefaultProfile(currentDevice->text(0), description);    
     currentDevice->setText(2, defaultProfile);

     // If current device pointer points to a MONITOR item, save default profile to Oyranos.
     if(currentDevice->parent() == parent_monitor_item)
     {          
         oyConfig_s * monitor = 0;
         const char * display_string_name = (currentDevice->text(0)).toLocal8Bit();
         const char * profile = defaultProfile.toLocal8Bit();
    
         oyDeviceGet( OY_TYPE_STD, "monitor", display_string_name, 0, &monitor);
         const char * display_name = oyConfig_FindString(monitor, "display_name", 0);
    
         oySetMonitorProfile ( display_name, profile );         
         oyActivateMonitorProfiles(display_name);
     }
     // If current device pointer points to a PRINTER item, save default profile to Oyranos.
     else if(currentDevice->parent() == parent_printer_item)
     {
         oyConfig_s * printer = 0;
         const char * printer_name = (currentDevice->text(0)).toLocal8Bit();
         const char * profile = defaultProfile.toLocal8Bit();
 
         oyDeviceGet( OY_TYPE_STD, "printer", printer_name, 0, &printer);
         oyDeviceSetProfile( printer, profile);
     }
     // If current device pointer points to a SCANNER item, save default profile to Oyranos.
     else if (currentDevice->parent() == parent_scanner_item)
     {
         oyConfig_s * scanner = 0;
         const char * scanner_name = (currentDevice->text(0)).toLocal8Bit();
         const char * profile = defaultProfile.toLocal8Bit();
 
         oyDeviceGet( OY_TYPE_STD, "scanner", scanner_name, 0, &scanner);
         oyDeviceSetProfile( scanner, profile);         
     }

     defaultProfileButton->setEnabled(false);
     removeButton->setEnabled(false);
}


kmdevices::~kmdevices()
{     
    
}

/*                  
           ************* Oyranos DB Settings *************
*/



// Saves default profile to the list.
void kmdevices::saveDefaultProfile(QString device_name, QString profile)
{
     QString keyString = "DEVICE:";
     keyString.append(device_name);

     KConfigGroup save_default_profile(m_config, keyString);
     QStringList profileList = save_default_profile.readEntry("ASSOCIATED_PROFILES",
                                                           QStringList());

     // If the profileList is empty, then this is the first time loading the device...
     if(profileList.empty())
     {          
         QStringList descriptionList = save_default_profile.readEntry("ASSOCIATED_DESCRIPTIONS",
                                                           QStringList());

         profileList.append(profile);
         descriptionList.append(convertFilenameToDescription(profile));

         save_default_profile.writeEntry("ASSOCIATED_PROFILES", profileList);
         save_default_profile.writeEntry("ASSOCIATED_DESCRIPTIONS", descriptionList);
     }

     save_default_profile.writeEntry("DEFAULT_PROFILE", profile); 
}


// This is so we can obtain a profile name description from a profile file name.
QString kmdevices::convertFilenameToDescription(QString profileFilename)
{    
    QString profileDescriptionName;
    oyProfile_s * profile_name;
    
    profile_name = oyProfile_FromFile( profileFilename.toLocal8Bit(), 0, 0);
    profileDescriptionName = oyProfile_GetText( profile_name, oyNAME_DESCRIPTION );

    return profileDescriptionName;
}
