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
#include <oyranos/oyranos_config.h>
#include <oyranos/oyranos_icc.h>
#include <oyranos/oyranos_cmm.h>
// #include "oyranos/oyranos_printer.h"

#include <cups/cups.h>
#include <cups/ppd.h>
#include <sane/sane.h>

// Code to provide KDE module functionality for color manager.
K_PLUGIN_FACTORY( kmdevicesFactory, 
         registerPlugin<kmdevices>(); 
         )
K_EXPORT_PLUGIN( kmdevicesFactory("kmdevices") )

using namespace oyranos;

#define SANE_MAX_USERNAME_LEN   128
#define SANE_MAX_PASSWORD_LEN   128

#define KM_CONFIG_FILE "kolor-manager-globals"

typedef void (*SANE_Authorization_Callback)
    (SANE_String_Const resource,
         SANE_Char username[SANE_MAX_USERNAME_LEN],
         SANE_Char password[SANE_MAX_PASSWORD_LEN]);

void kmdevices::load()
{
    
}

void kmdevices::save()
{
     if(listModified == true)
     {
         saveProfileSettings(currentDevice->text(0));
         listModified = false;
     }

     // Refresh KConfig file
     m_config->sync();

     // NOTE oyConfig_s solution to kconfig.
     // oyConfig_SaveToDB(km_config);           
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

    listModified = false;       // Nothing has been modified yet...
  
    setupUi(this);              // Load Gui. 

    // NOTE oyConfig_s solution to kconfig.
    // km_config = oyConfig_New(KM_CONFIG_FILE, 0);

    m_config = KSharedConfig::openConfig(KM_CONFIG_FILE);
 
    addProfileButton->setEnabled(false);
    removeButton->setEnabled(false);
    deviceProfileComboBox->setEnabled(false);    
    defaultProfileButton->setEnabled(false);

    // Set column width of device list.
    deviceList->setColumnWidth(0, 350);
    deviceList->setColumnWidth(1, 175);
    deviceList->setColumnWidth(2, 175);
  
    // Load directories and device listing.
    populateDeviceListing();   
    
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
    detectScanner();    
    detectPrinter();
    detectMonitor();  

    // FIXME Camera backend support needs to be fixed.
    //detectCamera();

    // Refresh KConfig file
    m_config->sync();

    // NOTE oyConfig_s solution to kconfig.
    // oyConfig_SaveToDB(km_config);
}

// Setup monitor items for QtListWidget (Devices)
void kmdevices::detectMonitor()
{     
     // To add monitor device name to list.
     KConfigGroup save_monitor_list(m_config, "DEVICE_LIST");     
     QStringList monitorNameList;

     QDesktopWidget desktop;
     int nScreens = desktop.numScreens();
         
     QIcon monitor_icon(":/resources/monitor.png");    

     char * display;

     if (nScreens > 0)
     {
        // build monitor top-level item.
        parent_monitor_item = new QTreeWidgetItem;
        parent_monitor_item->setText(0, "Monitor");
        deviceList->insertTopLevelItem(0, parent_monitor_item);

        for(int j = 0; j < nScreens; j++)
        {        
            QString monitorItemString;    
            QString monitorProfileString;       

           display = 0;
           char * manufacturer = 0;
           char * model = 0;
           char * serial = 0;
           char * system_port = 0;
           char * display_geometry = 0;
           oyBlob_s * edid = 0;

           char * newProfile;

           QRect rect = desktop.screenGeometry(j);

           // Grab Monitor info       
           display = oyGetDisplayNameFromPosition(display, rect.left(), rect.bottom(), 0);

           oyGetMonitorInfo(display, &manufacturer, &model, &serial, &system_port, 
                                              &display_geometry, &edid, 0);

           // Display Monitor Information in list.
           newProfile = oyGetMonitorProfileNameFromDB(display, 0);
           
           // Activate monitor profile through Oyranos.
           oySetMonitorProfile(display, newProfile);    
           oyActivateMonitorProfiles(display);

           // Setup monitor device designation
           monitorItemString.append(manufacturer);
           monitorItemString.append(" ");
           monitorItemString.append(model);
           monitorItemString.append("-");
           monitorItemString.append(serial);

           // FIXME In Oyranos, there seems to be an "\n" in one of the monitor return strings.
           //       Is it possible to remove this, or is the '\n" necessary?
           monitorItemString.remove("\n");
        
           deviceListPointer = new QTreeWidgetItem(); 
              
           addNewDeviceConfig(monitorItemString);     
           saveDefaultProfile(monitorItemString, newProfile);       
           saveDeviceType(monitorItemString, "MONITOR");

           // Save display information to KConfig            
           saveMonitorLocationInfo (monitorItemString, display);
              
           // Initial "default profile" string.
           monitorProfileString = newProfile;
       
           deviceListPointer->setText(0, monitorItemString);
           deviceListPointer->setIcon(0, monitor_icon);
           deviceListPointer->setText(1, convertFilenameToDescription(monitorProfileString));
           deviceListPointer->setText(2, monitorProfileString);

           parent_monitor_item->addChild(deviceListPointer);  
      
           monitorNameList.insert(0, monitorItemString);
       
        }   
    }
    save_monitor_list.writeEntry("MONITOR", monitorNameList);

/*     NOTE oyConfig_s solution to kconfig.

    int i;
    QString keyString;
    QString keyString_base = "MONITOR_NAME";     
 
    for (i = 0; i < monitorNameList.size(); i++)
    {
        keyString = keyString_base + (char*)(i + 1);
        oyConfig_AddDBData(km_config, keyString.toAscii(), (monitorNameList.takeAt(i)).toAscii(), 6);

    }*/
    
}

void kmdevices::detectPrinter()
{
   KConfigGroup save_printer_info(m_config, "DEVICE_LIST");
   QStringList printerNameList;

   QString initialProfileName, printerItemString, printerProfileString;
   QIcon printer_icon(":/resources/printer1.png");
 
   int numDests = 0;

   cups_dest_t *dests = NULL;

   numDests = cupsGetDests(&dests);

 /* NOTE Alternative backend code to retrieve the CUPS info. 

   char *** list = allocFunc(sizeof(char**));
   int numDests = oyPrGetDevices(list, allocFunc);

*/

   if( numDests > 0)
   {
        parent_printer_item = new QTreeWidgetItem;
        parent_printer_item->setText(0, "Printers");
        deviceList->insertTopLevelItem(0, parent_printer_item);
        
        for (int j = 0; j < numDests; j++)
        {
            printerItemString = dests[j].name;

            /* NOTE Backend method:
      
            char * printer_name;
            char * manufacturer = 0;
            char * model = 0;
            char * serial = 0;
                
            oyConfig_s * cups_device = oyConfig_New (CMM_PRINTER_REG, 0);    
            oyOptions_s * cups_options = oyPrSaveCUPSOptions ( *(*list + j), &cups_device);

            char * newProfile = oyPrPPDGetProfileName ( cups_options );

            // TODO Work on backend to activate profiles here.

            oyOption_s * printer_info = 0;

            printer_name = oyConfig_Find(km_config, "instrument_name", 0);
            manufacturer = oyConfig_Find(km_config, "Manufacturer", 0);
            model = oyConfig_Find(km_config, "Model", 0);
            serial = oyConfig_Find(km_config, "Serial", 0);

            // Setup printer device designation
            printerItemString.append(manufacturer);
            printerItemString.append(" ");
            printerItemString.append(model);
            printerItemString.append("-");
            printerItemString.append(serial);

            // Set for initial "default profile".
            printerProfileString = newProfile
                                                                            */

            // NOTE The following is not needed for Oyranos backend.
/*
            const char * printer_filename;  
            printer_filename = cupsGetPPD(dests[j].name);
            ppd_file_t * ppd;
            ppd = ppdOpenFile(printer_filename);                            */

            addNewDeviceConfig(printerItemString);
            //saveDefaultProfile(printerItemString, printerProfileString);                
            saveDeviceType(printerItemString, "PRINTER");
            
            /* NOTE Or use: saveDeviceType(printerItemString, printer_name);


                                                                            */
            deviceListPointer = new QTreeWidgetItem();

            deviceListPointer->setIcon(0, printer_icon);
            deviceListPointer->setText(0, printerItemString);
            deviceListPointer->setText(1, convertFilenameToDescription(printerProfileString));   
            deviceListPointer->setText(2, printerProfileString);
        
            parent_printer_item->addChild(deviceListPointer);
     
            printerNameList.insert(0, printerItemString);
       }
  }

     cupsFreeDests(numDests, dests);   
     save_printer_info.writeEntry("PRINTER", printerNameList);

/*      NOTE oyConfig_s solution to kconfig.

    int i;
    QString keyString;
    QString keyString_base = "PRINTER_NAME";     
 
    for (i = 0; i < printerNameList.size(); i++)
    {
        keyString = keyString_base + (char*)(i + 1);
        oyConfig_AddDBData(km_config, keyString.toAscii(), (printerNameList.takeAt(i)).toAscii(), 6);

    }*/
    

}

// Inital scanner support.

void kmdevices::detectScanner()
{
    QString scannerItemString = "";
    QString scannerProfileString = "";

    SANE_Status stat;
    SANE_Int version_code;   

    const SANE_Device **scanner_device_list;

    stat = sane_init(&version_code, NULL);

    stat = sane_get_devices(&scanner_device_list, SANE_FALSE);

    if(scanner_device_list[0] == NULL)
    {
        sane_exit();
        return;
    }    

    if (stat == SANE_STATUS_GOOD)
    {
        int j = 0;
       
        KConfigGroup save_scanner_info(m_config, "DEVICE_LIST");
        QStringList scannerNameList;

        QIcon scanner_icon(":/resources/scanner.png");

        parent_scanner_item = new QTreeWidgetItem;
        parent_scanner_item->setText(0, "Scanners");
        deviceList->insertTopLevelItem(0, parent_scanner_item);

        while(scanner_device_list[j] != NULL)
        {           
            scannerItemString = scanner_device_list[j]->vendor;
            scannerItemString.append(" ");
            scannerItemString.append(scanner_device_list[j]->model);
            scannerItemString.append(" ");
            scannerItemString.append(scanner_device_list[j]->type);
            scannerItemString.append(" ");

            scannerItemString.append(scanner_device_list[j]->name);

            addNewDeviceConfig(scannerItemString);
            saveDeviceType(scannerItemString, "SCANNER");
            
            // FIXME Need method to grab profiles directly from scanner.
           
            deviceListPointer = new QTreeWidgetItem();

            deviceListPointer->setIcon(0, scanner_icon);
            deviceListPointer->setText(0, scannerItemString);
            deviceListPointer->setText(1, convertFilenameToDescription(scannerProfileString));   
            deviceListPointer->setText(2, scannerProfileString);

            parent_scanner_item->addChild(deviceListPointer);

           scannerNameList.insert(0, scannerItemString);
           j++;

       }
        
        save_scanner_info.writeEntry("SCANNER", scannerNameList);
    }

   sane_exit();

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

//        ** SIGNAL/SLOT Related Functions **  
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

// When the user clicks on an item in the devices tree list.
void kmdevices::changeDeviceItem(QTreeWidgetItem * selected_device)
{ 
    // Don't count top parent items as a "selected device".
    if (selected_device == parent_monitor_item ||
        selected_device == parent_printer_item ||
        selected_device == parent_scanner_item )
    {
         addProfileButton->setEnabled(false);                   
         removeButton->setEnabled(false);         
         deviceProfileComboBox->setEnabled(false);
         defaultProfileButton->setEnabled(false);
        
         return;
    }

    if (listModified)   
    {
        if(KMessageBox::questionYesNo(this,
        i18n("The associated profiles list has changed. \nDo you wish to save it?"),
        i18n("Save device-associated profiles."))
           == KMessageBox::Yes)

            saveProfileSettings(currentDevice->text(0));

            listModified = false;
     }

    // If we find a device, the current device is stored and options are available.
    addProfileButton->setEnabled(true);
    removeButton->setEnabled(true);
    
    deviceProfileComboBox->setEnabled(true);
    defaultProfileButton->setEnabled(true);

    // Change "Available Device Profiles" combobox to device-related profiles.
    if ( selected_device->parent() == parent_monitor_item)        
             populateDeviceComboBox(icSigDisplayClass);
    else if ( selected_device->parent() == parent_printer_item)
            populateDeviceComboBox(icSigOutputClass);
    
     // TODO Does icSigInputClass return any profile?
    else if ( selected_device->parent() == parent_scanner_item)
             populateDeviceComboBox(icSigInputClass);  
    else if ( selected_device->parent() == parent_camera_item)
             populateDeviceComboBox(icSigInputClass);  

    currentDevice = selected_device;      
    updateProfileList(); 
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

// Update profile association list.
void kmdevices::updateProfileList()
{
    int j;
    QStringList savedProfileList;
    QString keyString = "DEVICE:";

    // Read from KConfig file about available Associated Profiles (filenames).
    keyString.append(currentDevice->text(0));    
    KConfigGroup loadProfiles(m_config, keyString);
    savedProfileList = loadProfiles.readEntry("ASSOCIATED_PROFILES", QStringList());
    
    profileAssociationList->clear();

    // Refresh profile listing.
    for(j = 0; j < savedProfileList.size(); j++)
        profileAssociationList->addItem(savedProfileList.value(j));
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

// Set default profile...
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

    // Save default profile to KConfig.
    saveDefaultProfile(currentDevice->text(0), description); 
   
    currentDevice->setText(2, defaultProfile);

     // If current device pointer points to a MONITOR item, save default profile to Oyranos.
     if(currentDevice->parent() == parent_monitor_item)
     {          
         const char * display_name = loadMonitorLocationInfo (currentDevice->text(0)); 
         const char * default_profile = defaultProfile.toLocal8Bit();

         oySetMonitorProfile ( display_name, default_profile );         
         const char * activated_profile_name = loadMonitorLocationInfo (currentDevice->text(0));
         oyActivateMonitorProfiles(activated_profile_name);
     }
       
    // TODO write code to activate printer, scanner, etc. ...

    
    defaultProfileButton->setEnabled(false);
    removeButton->setEnabled(false);
}


kmdevices::~kmdevices()
{     

}

/*                  
           ************* KConfig functions for kmdevices *************
*/

// Add a new device (initializer)
void kmdevices::addNewDeviceConfig(QString device_name)
{                 
     QString keyString = "DEVICE:";
     keyString.append(device_name);
     KConfigGroup device_config(m_config, keyString);
}

// Save profile settings to KConfig file.
void kmdevices::saveProfileSettings(QString device_name)
{
    // (In KConfig File...) 
    //  DEVICE:'device_name'
    QString keyString = "DEVICE:";
    keyString.append(device_name);
    KConfigGroup save_device_profiles(m_config, keyString);

    save_device_profiles.deleteEntry("ASSOCIATED_PROFILES");
    save_device_profiles.deleteEntry("ASSOCIATED_DESCRIPTIONS");

    QStringList updatedProfiles;
    QStringList updatedDescriptions;

    QListWidgetItem * temp_item;

    QString fileToDescriptionString;
        
    int i;
    for (i = 0; i < profileAssociationList->count(); i++)
    {
         temp_item = profileAssociationList->item(i);

         // Grab filename from list.
         fileToDescriptionString = temp_item->text();
         
         updatedDescriptions.insert(i, convertFilenameToDescription(fileToDescriptionString));
         updatedProfiles.insert(i, temp_item->text());
    }
    
    save_device_profiles.writeEntry("ASSOCIATED_PROFILES", updatedProfiles);
    save_device_profiles.writeEntry("ASSOCIATED_DESCRIPTIONS", updatedDescriptions);
  
/*      NOTE oyConfig_s solution to kconfig.
  
     QString fileToDescriptionString;
     QListWidgetItem * temp_item;
    
     const char * description_string = 0;
     const char * profile_string = 0;

     QString keyString_ap;
     QString keyString_ap_base = device_name;
     keyString_ap_base.append("-ASSOCIATED_PROFILE");

     QString keyString_ad;
     QString keyString_ad_base = device_name;
     keyString_ad_base.append("-ASSOCIATED_DESCRIPTION");

     QString keyString_apCount = device_name;
     keyString_apCount.append("-PROFILE_COUNT");

     char * profile_count = (char *)(profileAssociationList->count());
     oyConfig_AddDBData(km_config, keyString_apCount.toAscii(), profile_count, 6);
              
    int i;
    for (i = 0; i < profileAssociationList->count(); i++)
    {
         keyString_ap = keyString_ap_base + (char*)(i + 1);
         keyString_ad = keyString_ad_base + (char*)(i + 1);
         
         temp_item = profileAssociationList->item(i);

         // Grab filename from list.
         fileToDescriptionString = temp_item->text();
         fileToDescriptionString = convertFilenameToDescription(fileToDescriptionString);
         description_string = fileToDescriptionString.toAscii();
         
         oyConfig_AddDBData(km_config, keyString_ap.toAscii(), description_string, 6);
         oyConfig_AddDBData(km_config, keyString_ad.toAscii(), (temp_item->text()).toAscii(), 6);
    }

    oyConfig_SaveToDB(km_config);*/

    
}


// Add selected profile to the ASSOCIATED_PROFILES listing in the KConfig file.
void kmdevices::addDeviceProfile(QString device_name, QString profile)
{
     QString keyString = "DEVICE:";
     keyString.append(device_name);
     KConfigGroup add_profile(m_config, keyString);
     
     QStringList temp_profilelist = add_profile.readEntry("ASSOCIATED_PROFILES", QStringList());
     QStringList descriptionlist = add_profile.readEntry("ASSOCIATED_DESCRIPTIONS", QStringList());
 
     if(!isProfileDuplicate(profile, temp_profilelist))
         return;
     
     temp_profilelist.append(profile);  
     descriptionlist.append(convertFilenameToDescription(profile));
          
     add_profile.writeEntry("ASSOCIATED_PROFILES", temp_profilelist);
     add_profile.writeEntry("ASSOCIATED_DESCRIPTIONS", descriptionlist);


   /*          NOTE oyConfig_s solution to kconfig.

     QString keyString_ap_base = device_name;
     keyString_ap_base.append("-ASSOCIATED_PROFILE");

     QString keyString_ad_base = device_name;
     keyString_ad_base.append("-ASSOCIATED_DESCRIPTION");

     oyConfig_AddDBData(km_config, keyString_ap_base.toAscii(), profile.toAscii(), 6);

     const char * fn_description = (convertFilenameToDescription(profile)).toAscii();

     oyConfig_AddDBData(km_config, keyString_ad_base.toAscii(), fn_description, 6);

     QString keyString_apCount = device_name;
     keyString_apCount.append("-PROFILE_COUNT");

     QString profile_count = oyConfig_FindString(km_config, keyString_apCount.toAscii(), 0);
   */  

}

// A check to see if the profile already exists in the profile list (resolves duplication)
bool kmdevices::isProfileDuplicate(QString profile, QStringList profileList)
{
    int i;
        
     for (i = 0; i < profileList.count(); i++)
     {
          if(profileList[i] == profile)    
              return true;
     }

     return false;
}

// In the initial configuration, this saves the type of device (scanner, monitor, printer...)
void kmdevices::saveDeviceType(QString device_name, QString deviceType)
{
     QString keyString = "DEVICE:";
     keyString.append(device_name);
     KConfigGroup save_device_type(m_config, keyString);
     save_device_type.writeEntry("Type", deviceType);

/*        NOTE oyConfig_s solution to kconfig.

     QString keyString = device_name;
     keyString.append("-DEVICE_TYPE");  
     oyConfig_AddDBData(km_config, keyString.toAscii(), deviceType.toAscii(), 6);*/
}

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

/*    NOTE oyConfig_s solution to kconfig.

     int i;
     QString keyString = device_name;
     keyString.append("-DEFAULT_PROFILE");

     oyConfig_AddDBData(km_config, keyString.toAscii(), profile.toAscii(), 6);*/
}

// Function to save Oyranos display information.
void kmdevices::saveMonitorLocationInfo (QString device_name, const char * display)
{
     QString keyString = "DEVICE:";
     keyString.append(device_name);
     KConfigGroup save_display(m_config, keyString);

     QString displayQString;

     save_display.writeEntry("Screen_Position", display);

/*    NOTE oyConfig_s solution to kconfig.
    QString keyString = device_name;
    keyString.append("-SCREEN_POSITION");    

    oyConfig_AddDBData(km_config, keyString.toAscii(), display, 6);*/
}

const char * kmdevices::loadMonitorLocationInfo(QString device_name)
{
     QString keyString = "DEVICE:";
     keyString.append(device_name);
     KConfigGroup save_display(m_config, keyString);

     QString displayQString = save_display.readEntry("Screen_Position", QString() );
  
     return displayQString.toLocal8Bit();

/*    NOTE oyConfig_s solution to kconfig.

     QString keyString = device_name;
     keyString.append("-SCREEN_POSITION");

     const char * displayQString = oyConfig_FindString(km_config, keyString.toAscii(), 0);

     return displayQString; */

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
