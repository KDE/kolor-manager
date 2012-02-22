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
#include <QMouseEvent>
#include <QDesktopWidget>
#include <KSharedConfigPtr>
#include <KAboutData>

#include <oyranos.h>
#include <oyranos_icc.h>
#include <alpha/oyranos_alpha.h>
#include <alpha/oyranos_cmm.h>

#define CONFIG_REGISTRATION ("//" OY_TYPE_STD "/config/command")

// Code to provide KDE module functionality for color manager.
K_PLUGIN_FACTORY( kmdevicesFactory, 
         registerPlugin<kmdevices>(); 
         )
K_EXPORT_PLUGIN( kmdevicesFactory("kmdevices") )


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
        KAboutData::License_BSD,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("2008-2009 Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    about->addAuthor( ki18n("2010 Kai-Uwe Behrmann"), KLocalizedString(),
                      "ku.b@gmx.de"  );
    
    setAboutData( about );

    current_device_name = 0;
    current_device_class = 0;

    listModified = false;       // avoid action on signals

    setupUi(this);              // Load Gui. 
 
    // Disable all buttons
    deviceProfileComboBox->setEnabled(false);    

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
    connect( deviceList, SIGNAL(itemClicked( QTreeWidgetItem*, int)),
             this, SLOT( changeDeviceItem( QTreeWidgetItem*)) );
    connect( relatedDeviceCheckBox, SIGNAL(stateChanged( int )),
             this, SLOT( changeDeviceItem( int )) );
    connect( deviceProfileComboBox, SIGNAL(activated(int)),
             this, SLOT( openProfile(int)) );
    connect( profileAssociationList, SIGNAL( itemDoubleClicked( QListWidgetItem* )),
             this, SLOT( profileListDoubleClicked( QListWidgetItem * )) );
}

// small helper to obtain a profile from a device
int kmDeviceGetProfile( oyConfig_s * device, oyProfile_s ** profile )
{
  oyOptions_s * options = 0;
  oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
  int error = oyDeviceGetProfile( device, options, profile );
  oyOptions_Release( &options );
  return error;
}


// Populate devices and profiles.
void kmdevices::populateDeviceListing()
{
  // TODO Work out a solution to use raw/camera stuff.
  detectRaw();  

  uint32_t count = 0, i = 1,
         * rank_list = 0;
  char ** texts = 0;
  int error = 0;

  while(i && getenv("DEBUG"))
    sleep(1);

  // get all configuration filters
  oyConfigDomainList( "//"OY_TYPE_STD"/config.device.icc_profile",
                      &texts, &count, &rank_list ,0 );

  for (i = 0; i < count; ++i)
  {
    error = detectDevices( texts[i] );
  }
}

// NEW General function to detect and retrieve devices via the Oyranos CMM backend.
int kmdevices::detectDevices(const char * device_type)
{ 
    int error = 0;
    oyConfigs_s * device_list = 0;
    oyOptions_s * options = oyOptions_New(0);
    oyConfDomain_s * d = oyConfDomain_FromReg( device_type, 0 );
    const char * reg_app = strrchr(device_type,'/')+1;
    const char * device_class = oyConfDomain_GetText( d, "device_class",
                                                      oyNAME_NICK );

    oyOptions_SetFromText(&options, "//" OY_TYPE_STD "/config/command", 
                          "properties", OY_CREATE_NEW);

    oyDevicesGet( OY_TYPE_STD, reg_app, 0, &device_list); 
        
    int j = 0, 
        device_num = oyConfigs_Count(device_list);

    // Must have at least one device detected to add to the list.
    if(device_num > 0)
    {
        // Set up Kolor Manager gui "logistics" for a specified device.
        QTreeWidgetItem * parent_item = new QTreeWidgetItem;
        parent_item->setText(0, oyConfDomain_GetText( d, "device_class",
                                                      oyNAME_NAME ));
        QVariant v( device_class );
        parent_item->setData( 0, Qt::UserRole, v );
        deviceList->insertTopLevelItem(0, parent_item);

        QIcon device_icon;
        QSize icon_size(30, 30);
        QString icon;
        icon = QString(":/resources/") + device_class + ".png";
        device_icon.addFile( icon.toLower(), icon_size , QIcon::Normal, QIcon::On);

        // Traverse through the available devices 
        for (j = 0; j < device_num; ++j)
        {
            QString deviceItemString, deviceProfileDescription;

            oyConfig_s * device = oyConfigs_Get(device_list, j);
            error = oyDeviceBackendCall(device, options);
 
            const char * device_manufacturer = 0;
            const char * device_model = 0;
            const char * device_serial = 0;
            char * device_designation = 0;

            const char * profile_filename = 0;
            oyProfile_s * profile = 0;

            device_manufacturer = oyConfig_FindString( device,"manufacturer",0);
            device_model = oyConfig_FindString( device, "model", 0);
            device_serial = oyConfig_FindString( device, "serial", 0);

            // Get device designation.
            error = oyDeviceGetInfo(device, oyNAME_NICK, 0, &device_designation, malloc);
 
            // A printer will only take a "device model"
            if (strcmp(device_class,"printer") != 0)
            {
                deviceItemString.append(device_manufacturer);
                deviceItemString.append(" ");
            }

            deviceItemString.append(device_model);
            deviceItemString.append(" ");
            deviceItemString.append(device_serial);

            error = kmDeviceGetProfile(device, &profile);
            profile_filename = oyProfile_GetFileName(profile, 0);
 
            deviceListPointer = new QTreeWidgetItem();

            if (profile_filename == NULL)
            {
                deviceProfileDescription = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
                if(!deviceProfileDescription.count())
                  deviceProfileDescription = "(No Profile Installed!)";
                profile_filename = "------";
            }
            else
                deviceProfileDescription = convertFilenameToDescription(profile_filename);
 
            deviceListPointer->setIcon(0, device_icon);
            deviceListPointer->setText(DEVICE_DESCRIPTION, deviceItemString);
            deviceListPointer->setText(DEVICE_NAME, device_designation);
            deviceListPointer->setText(PROFILE_DESCRIPTION, deviceProfileDescription);   
            deviceListPointer->setText(PROFILE_FILENAME, profile_filename);
        
            parent_item->addChild(deviceListPointer);

            oyConfig_Release(&device);
        }
        
     }
     else
        error = -1;

     oyOptions_Release ( &options );
     oyConfigs_Release ( &device_list );
    oyConfDomain_Release( &d );
 
     return error;
}

// Display an item that will open a dialog to use camera functionality.
void kmdevices::detectRaw()
{
}


//        ** SIGNAL/SLOT Related Functions **  
// Hitting the "Show only device related ICC profiles" button 
// relatedDeviceCheckBox.
void kmdevices::changeDeviceItem(int /*state*/)
{     
  changeDeviceItem( currentDevice );
} 

// When the user clicks on an item in the devices tree list.
void kmdevices::changeDeviceItem(QTreeWidgetItem * selected_device)
{     
    // Don't count top parent items as a "selected device".
    if (selected_device->parent() == deviceListPointer)
    {         
         deviceProfileComboBox->setEnabled(false);
        
         return;
    }

    // The user modifies the list, but clicks away from the selected device item.
    listModified = false;

    // If we click on a device item, the current device is stored and options are available.
    deviceProfileComboBox->setEnabled(true);

    currentDevice = selected_device;
 
    // Convert QString to proper C string.
    QByteArray raw_string;
    raw_string = (currentDevice->text(DEVICE_NAME)).toLatin1();
    setCurrentDeviceName(raw_string.data());
        
    char * device_class = 0;
    if(selected_device && selected_device->parent())
    {
      QVariant v = selected_device->parent()->data( 0, Qt::UserRole );
      QString qs_device_class = v.toString();
      QByteArray raw_string;
      raw_string = qs_device_class.toLatin1();
      device_class = strdup(raw_string.data());
    }

    // Change "Available Device Profiles" combobox to device-related profiles.
    if ( device_class )
    {  
      oyConfDomain_s * d = oyConfDomain_FromReg( device_class, 0 );
      const char * icc_profile_class = oyConfDomain_GetText( d,
                                             "icc_profile_class", oyNAME_NICK );
      setCurrentDeviceClass(device_class);

          if(icc_profile_class && strcmp(icc_profile_class,"display") == 0)
        populateDeviceComboBox(icSigDisplayClass);
      else if(icc_profile_class && strcmp(icc_profile_class,"output") == 0)
        populateDeviceComboBox(icSigOutputClass);
      else if(icc_profile_class && strcmp(icc_profile_class,"input") == 0)
        populateDeviceComboBox(icSigInputClass);
      profileAssociationList->clear();

      oyConfDomain_Release( &d );
      free(device_class); device_class = 0;
    }

    // Get the device that the user selected.
    oyConfig_s * device = 0;
    device = getCurrentDevice();

    updateProfileList(device); 
    oyConfig_Release(&device);
}

// Update profile association list every time a user clicks on a device item.
void kmdevices::updateProfileList(oyConfig_s * device)
{
    int i, j;
    oyProfile_s * profile = 0;
    const char * profile_name = 0;
    oyConfigs_s * db_list = 0,
                * matches = 0;
    oyConfig_s * config;
    const char * pattern[][2] = {{"device_name",0},
                             {"manufacturer",0},
                             {"model",0},
                             {"serial",0},
                             {0,0}};

    if(!device) return;

    profileAssociationList->clear();

    kmDeviceGetProfile( device, &profile );
    profile_name = oyProfile_GetText(profile, oyNAME_DESCRIPTION);

    j = 0;
    while(pattern[j][0])
    {
      pattern[j][1] = oyConfig_FindString( device, pattern[j][0], 0);

      ++j;
    }

    oyConfigs_FromDB( device->registration, &db_list, 0 );
    oyConfigs_SelectSimiliars( db_list, pattern, &matches );

    int n = oyConfigs_Count( matches );
    for(i = 0; i < n; ++i)
    {
      config = oyConfigs_Get( matches, i );
      
      profile_name = oyConfig_FindString( config, "profile_name", 0);
      if( profile_name )
        profileAssociationList->addItem( profile_name );

      oyConfig_Release( &config );
    }

  oyConfigs_Release( &matches );
  oyConfigs_Release( &db_list );
}



// Populate "Assign Profile" combobox.  Depending on the device selected, the profile list will vary.
void kmdevices::populateDeviceComboBox(icProfileClassSignature deviceSignature)
{
    int size, i, current = -1, current_tmp = 0, pos = 0;
    oyProfile_s * profile = 0, * temp_profile = 0;
    oyProfiles_s * patterns = 0, * iccs = 0;
    oyConfig_s * device = getCurrentDevice();
    const char * profile_file_name = 0;

    profile = oyProfile_FromSignature( deviceSignature, oySIGNATURE_CLASS, 0 );
    patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

    iccs = oyProfiles_Create( patterns, 0 );
    oyProfiles_Release( &patterns );
 
    QString getProfileDescription;

    size = oyProfiles_Count(iccs);
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(iccs) *
                                             sizeof(int32_t) );
    oyProfiles_DeviceRank( iccs, device, rank_list );

    size = oyProfiles_Count(iccs);

    deviceProfileComboBox->clear();

    kmDeviceGetProfile( device, &profile ); /* reget profile */
    profile_file_name = oyProfile_GetFileName( profile, 0 );

    Qt::CheckState show_only_device_related = 
                                            relatedDeviceCheckBox->checkState();
    int empty_added = -1;

    for( i = 0; i < size; ++i)
    {
      const char * temp_profile_file_name;
         temp_profile = oyProfiles_Get( iccs, i );
         getProfileDescription = oyProfile_GetText( temp_profile, oyNAME_DESCRIPTION );
         temp_profile_file_name = oyProfile_GetFileName( temp_profile, 0);
 
         current_tmp = -1;

         if(profile_file_name && temp_profile_file_name &&
            strcmp( profile_file_name, temp_profile_file_name ) == 0)
           current_tmp = pos;

         if(current == -1 && current_tmp != -1)
           current = current_tmp;

         if(empty_added == -1 &&
            rank_list[i] < 1)
         {
           deviceProfileComboBox->addItem("");
           empty_added = pos;
           if(current != -1 &&
              current == pos)
             ++current;
           ++pos;
         }

         if(show_only_device_related == Qt::Unchecked ||
            rank_list[i] > 0 ||
            current_tmp != -1)
         {
           getProfileDescription.append("\t(");
           getProfileDescription.append(temp_profile_file_name);
           getProfileDescription.append(")");
           deviceProfileComboBox->addItem(getProfileDescription);
           ++pos;
         }
      oyProfile_Release( &temp_profile );
    }
  if(empty_added == -1)
  {
    deviceProfileComboBox->addItem("");
    ++pos;
    if(current == -1 && current_tmp != -1)
      current = pos;
  }
  oyConfig_Release( &device );
  oyProfile_Release( &profile );
  oyProfiles_Release( &iccs );

  deviceProfileComboBox->setCurrentIndex( current );
}

// Add a new profile to the list.
void kmdevices::openProfile(int /*index*/)
{
    int parenthesis_index = 0, base_filename_index = 0, str_size = 0, i;        
    QString baseFileName = deviceProfileComboBox->currentText();

    QListWidgetItem * temp_item = new QListWidgetItem;

    parenthesis_index = baseFileName.indexOf("\t(");   

    // Clean-up full text in the deviceComboBox (we will only use the file name to
    //                                          add to the profiles list).
    str_size = baseFileName.size();
    baseFileName.remove(0, parenthesis_index + 2);
    baseFileName.remove(str_size - 2, 1);
    parenthesis_index = baseFileName.indexOf(")");
    baseFileName.remove(parenthesis_index, parenthesis_index + 1);
    base_filename_index = baseFileName.lastIndexOf("/");
    baseFileName.remove(0, base_filename_index + 1); 

    // Error if user adds a duplicate profile.
    for(i = 0; i < profileAssociationList->count(); ++i)
    {
          temp_item = profileAssociationList->item(i);
          if(temp_item->text() == baseFileName)
          {
              std::string t = baseFileName.toStdString();
              KMessageBox::error(this,
                  i18n("Profile %1 is already associated with this device.", baseFileName),
                  i18n("Cannot add profile to list"));

                  return;
          }
    }

    emit changed(true); 
    listModified = true;

    assingProfile( baseFileName );

    // Convert QString to proper C string.
    QByteArray raw_string;
    raw_string = (currentDevice->text(DEVICE_NAME)).toLatin1();
    setCurrentDeviceName(raw_string.data());
        
    // Get the device that the user selected.
    oyConfig_s * device = 0;     
    device = getCurrentDevice();
 
    updateProfileList(device); 
    oyConfig_Release(&device);
}

void kmdevices::profileListDoubleClicked( QListWidgetItem * item )
{
  int i,j,n, k,k_n;
  oyConfigs_s * db_list = 0,
              * matches = 0;
  oyConfig_s * config = 0;
  const char * pattern[][2] = {{"device_name",0},
                               {"manufacturer",0},
                               {"model",0},
                               {"serial",0},
                               {0,0}};
  char * t;
  oyConfig_s * device = getCurrentDevice();
  oyOptions_s * options = 0;
  oyOption_s * o = 0;

  j = 0;
  while(pattern[j][0])
  {
    pattern[j][1] = oyConfig_FindString( device, pattern[j][0], 0);
    ++j;
  }

  oyConfigs_FromDB( device->registration, &db_list, 0 );
  oyConfigs_SelectSimiliars( db_list, pattern, &matches );

  n = profileAssociationList->count();
  for(i = 0; i < n; ++i)
  {
    if(profileAssociationList->item( i ) == item)
    {
      QString d;
      config = oyConfigs_Get( matches, i );
      
      k_n = oyConfig_Count( config );
      for(k = 0; k < k_n; ++k)
      {
        o = oyConfig_Get( config, k );
        t = oyOption_GetValueText( o, malloc );
        if(t)
        {
            d.append( strrchr( oyOption_GetRegistration(o), '/' ) + 1 );
            d.append(":\t");
            d.append( t );
            d.append("\n");
          free( t );
        }
      }

      KMessageBox::information( this, i18n("Oyranos DB entry:\n%1", d) );

      oyConfig_Release( &config );
    }
  }

  oyConfig_Release( &device );
  oyConfigs_Release( &db_list );
  oyConfigs_Release( &matches );
  oyOptions_Release( &options );
}

oyConfig_s * kmdevices::getCurrentDevice( void )
{
  oyConfig_s * device = 0;
  int error= 0;

  oyOptions_s * options = 0;
  oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command", 
                         "properties", OY_CREATE_NEW );
  oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
  if(current_device_class && current_device_name)
    error = oyDeviceGet( OY_TYPE_STD, current_device_class, current_device_name,
                         options, &device );
 
  /* clear */
  oyOptions_Release( &options );

  return device;
}

  void kmsleep(double sekunden)
  {
#          if defined(__GCC__) || defined(__APPLE__)
             timespec ts;
             double ganz;
             double rest = modf(sekunden, &ganz);
             ts.tv_sec = (time_t)ganz;
             ts.tv_nsec = (time_t)(rest * 1000000000);
             //DBG_PROG_V( sekunden<<" "<<ts.tv_sec<<" "<<ganz<<" "<<rest )
             nanosleep(&ts, 0);
#          else
#            if defined( WIN32 ) 
               Sleep((DWORD)(sekunden*(double)CLOCKS_PER_SEC));
#            else
               usleep((time_t)(sekunden*(double)CLOCKS_PER_SEC));
#            endif
#          endif
  }


// Set default profile.
void kmdevices::assingProfile( QString & profile_name )
{        
     oyProfile_s * profile;
     QString description;

     // If current device pointer points to a MONITOR item, save default profile to Oyranos.

     {
         int error= 0;
         oyConfig_s * device = getCurrentDevice();
         std::string t = profile_name.toStdString();
         const char * profilename = t.c_str();
         char * pn = strdup(profilename);

         /* store a existing profile in DB */
         if(strlen(pn))
           error = oyDeviceSetProfile ( device, pn );
         error = oyDeviceUnset( device ); /* unset the device */
         /* completely unset the actual profile from DB */
         if(!strlen(pn))
         {
           error = oyConfig_EraseFromDB( device );
           oyConfig_Release( &device );
           /* reopen the device to forget about the "profile_name" key */
           device = getCurrentDevice();
         }
         error = oyDeviceSetup( device ); /* reinitialise */
         /* compiz needs some time to exchange the profiles,
            immediately we would get the old colour server profile */
         kmsleep(0.3);
         error = kmDeviceGetProfile( device, &profile ); /* reget profile */

         /* clear */
         oyConfig_Release( &device );
         if(pn) free(pn); pn = 0;
     }

     // Convert profile into description name...
     description = oyProfile_GetText( profile, oyNAME_DESCRIPTION);

     if(!description.count())
       description = "(No Profile Installed!)";

     // Set third column of the device list with the profile description.
     currentDevice->setText(PROFILE_DESCRIPTION, description);
 
     if(!profile_name.count())
       profile_name = "------";
     currentDevice->setText(PROFILE_FILENAME, profile_name);
}


kmdevices::~kmdevices()
{     
    
}

/*                  
           ************* Oyranos DB Settings *************
*/



// This is so we can obtain a profile name description from a profile file name.
QString kmdevices::convertFilenameToDescription(QString profileFilename)
{    
    QString profileDescriptionName;
    oyProfile_s * profile_name;
    
    std::string t = profileFilename.toStdString();
    profile_name = oyProfile_FromFile( t.c_str(), 0, 0);
    profileDescriptionName = oyProfile_GetText( profile_name, oyNAME_DESCRIPTION );

    return profileDescriptionName;
}
