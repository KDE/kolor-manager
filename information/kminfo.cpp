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


#include "kminfo.h"

#include <KAboutData>
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
#include <QDesktopWidget>
#include <KConfigGroup>
#include <QString>

#include <oyranos/oyranos.h>
#include <oyranos/oyranos_config.h>
#include <oyranos/oyranos_alpha.h>

// Code to provide KDE module functionality for color manager.

K_PLUGIN_FACTORY( kminfoFactory, 
         registerPlugin<kminfo>(); 
         )
K_EXPORT_PLUGIN( kminfoFactory("kminfo") )

using namespace oyranos;     

// Detect iccexamin for integrated 3D Profile support.
bool kminfo::iccExaminIsInstalled(QString &iccExaminPath)
{

# ifdef __WIN32__
     QChar pathSep = QChar::fromLatin1(';');
     const QString iccExamin = QString::fromLocal8Bit("iccexamin.exe");
#elif defined (__APPLE__)
     QChar pathSep = QChar::fromLatin1(':'); 
     const QString iccExamin = QString::fromLocal8Bit("iccexamin.app/Contents/MacOS/ICC Examin");
# else
     QChar pathSep = QChar::fromLatin1(':');  
     const QString iccExamin = QString::fromLocal8Bit("iccexamin");
# endif /* __WIN32__ */

     QString Path = QString::fromLocal8Bit(getenv("PATH"));
     QFileInfo fileinfo;

     bool done = false;
     bool found = false;

     while (!done)
     {
          if (!Path.length() > 0) // still more paths to look at
                 done=true;
          else
          {
              // get next path
              iccExaminPath = Path.left(Path.indexOf(pathSep, 0, Qt::CaseSensitive)) +
              QString::fromLocal8Bit("/") + iccExamin ;
              // remove the path we are about to check from path
              Path = Path.right(Path.length() - Path.indexOf(pathSep, 0, Qt::CaseSensitive) - 1);
              // apend executable name to path
              fileinfo.setFile(iccExaminPath);
              // check to see if it exists
             if (fileinfo.exists())
             {  
                 done = true;
                 found = true;
                
             }
             // check to see if this was last path
             if (Path.length() <= iccExaminPath.length())
                 done = true;
           }
      }

     return found;
}

void kminfo::load()
{

}

void kminfo::save()
{ 
    installedProfilesTree->clear();
    populateInstalledProfileList();  
}

kminfo::kminfo(QWidget *parent, const QVariantList &) :
    KCModule( kminfoFactory::componentData(), parent)
{
       KAboutData* about = new KAboutData(
        "kminfo", 0, ki18n("KMInfo"), 0, KLocalizedString(),
        KAboutData::License_GPL,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    
    setAboutData( about );

    setupUi(this);              // Load Gui.
    
    m_config = KSharedConfig::openConfig("kolor-manager-globals");

    installedProfilesTree->setColumnWidth(0, 350);
    installedProfilesTree->setColumnWidth(1, 150);

    // Save tree list parents to QTreeWidgetItem pointers.
    devicesParentTree = installedProfilesTree->topLevelItem(0);
    editingCsTree = installedProfilesTree->topLevelItem(1);
    assumedCsTree = installedProfilesTree->topLevelItem(2);

    // Save sub-list of "Devices" to QTreeWidgetItem pointers according to row.
    monitorListSubTree = devicesParentTree->child(0);
    printerListSubTree = devicesParentTree->child(1);
    scannerListSubTree = devicesParentTree->child(2);
    //cameraListSubTree = devicesParentTree->child(3);

    // For convenience, we expand colorspace trees.
    installedProfilesTree->expandItem(editingCsTree);
    installedProfilesTree->expandItem(assumedCsTree);

    installedProfilesTree->expandAll();
    
    // Display oyEDITING_XYZ info for now. 
    populateInstalledProfileList();
    profileInfoGroupBox -> setEnabled(false);

    if (iccExaminIsInstalled(iccExaminCommand))
        launchICCExaminButton->show();
    else
        launchICCExaminButton->hide(); 
    
    scrollArea->setWidget(scrollAreaWidgetContents);
    scrollArea->setWidgetResizable(true);

    // Whenever the user clicks on a QTreeWidget child, the description changes.
    connect( installedProfilesTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), 
                this, SLOT(changeProfileTreeItem(QTreeWidgetItem*)));
    
    connect( launchICCExaminButton, SIGNAL(clicked()), this, SLOT(launchICCExamin())); 

}

void kminfo::launchICCExamin()
{
    QString exec = iccExaminCommand + " " + directoryListingTag->text();
    system(exec.toLocal8Bit());
}

// Populate the tree with detected profile items.
void kminfo::populateInstalledProfileList()
{
     devicesParentTree = installedProfilesTree->topLevelItem(0);
     editingCsTree = installedProfilesTree->topLevelItem(1);
     assumedCsTree = installedProfilesTree->topLevelItem(2);

      // Setup icons for devices parent item.
     QIcon monitor_icon(":/resources/monitor.png");  
     QIcon printer_icon(":/resources/printer1.png");
     QIcon scanner_icon(":/resources/scanner.png");

     // All input/output devices are stored in the "[DEVICE_LIST]" key.
     KConfigGroup installed_devices(m_config, "DEVICE_LIST");
     
     // Load any devices stored in the KConfig file.
     QStringList monitorDevices = installed_devices.readEntry("MONITOR", QStringList());
     QStringList printerDevices = installed_devices.readEntry("PRINTER", QStringList());
     QStringList scannerDevices = installed_devices.readEntry("SCANNER", QStringList());

     // Detect available monitors
     if(!monitorDevices.empty())
         populateDeviceProfiles(monitorDevices, monitorListSubTree, monitor_icon);

     // Detect available printers.
     if(!printerDevices.empty())
         populateDeviceProfiles( printerDevices, printerListSubTree, printer_icon);

     // Detect available scanners.
     if(!scannerDevices.empty())
         populateDeviceProfiles( scannerDevices, scannerListSubTree, scanner_icon);

     const char * g_name = NULL;
     QString name;
            
     // Populate colorspace items.
     oyWidgetTitleGet( (oyWIDGET_e)oyEDITING_RGB, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
         addProfileTreeItem( oyEDITING_RGB, name.fromLatin1(g_name), editingCsTree);
     oyWidgetTitleGet( (oyWIDGET_e)oyEDITING_CMYK, NULL, &g_name, NULL,NULL );
     if (strlen(g_name) > 0)
        addProfileTreeItem( oyEDITING_CMYK, name.fromLatin1(g_name), editingCsTree);
     oyWidgetTitleGet( (oyWIDGET_e)oyEDITING_XYZ, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
         addProfileTreeItem( oyEDITING_XYZ, name.fromLatin1(g_name), editingCsTree );
     oyWidgetTitleGet( (oyWIDGET_e)oyEDITING_LAB, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
         addProfileTreeItem( oyEDITING_LAB, name.fromLatin1(g_name), editingCsTree );
     oyWidgetTitleGet( (oyWIDGET_e)oyASSUMED_RGB, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
        addProfileTreeItem( oyASSUMED_RGB, name.fromLatin1(g_name), assumedCsTree );
     oyWidgetTitleGet( (oyWIDGET_e)oyASSUMED_CMYK, NULL, &g_name, NULL,NULL );
     if (strlen(g_name) > 0)
         addProfileTreeItem( oyASSUMED_CMYK, name.fromLatin1(g_name), assumedCsTree );
     oyWidgetTitleGet( (oyWIDGET_e)oyASSUMED_XYZ, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
        addProfileTreeItem( oyASSUMED_XYZ, name.fromLatin1(g_name), assumedCsTree );
     oyWidgetTitleGet( (oyWIDGET_e)oyASSUMED_LAB, NULL, &g_name, NULL, NULL );
     if (strlen(g_name) > 0)
        addProfileTreeItem( oyASSUMED_LAB, name.fromLatin1(g_name), assumedCsTree );
}

void kminfo::populateDeviceProfiles(QStringList listOfDevices, QTreeWidgetItem * deviceListSubTree,                                                                                                                  
                                                                                     QIcon device_icon)
{
    int i, j;
    QString deviceName = "";

    for (i = 0; i < listOfDevices.count(); i++)
     {         
         QTreeWidgetItem * device_child = new QTreeWidgetItem;
         deviceName = listOfDevices.value(i);
         device_child->setText(0, deviceName);
         device_child->setIcon(0, device_icon);
         deviceListSubTree->addChild(device_child);   

         // Configure current device
         KConfigGroup device_group(m_config, "DEVICE:" + deviceName);
         QStringList deviceProfiles = device_group.readEntry("ASSOCIATED_DESCRIPTIONS", QStringList());
         QStringList deviceFile = device_group.readEntry("ASSOCIATED_PROFILES", QStringList());

         QString fileName, profileName;

         for(j = 0; j < deviceProfiles.count(); j++)
         {
              QTreeWidgetItem * profile_child = new QTreeWidgetItem;
              profileName = deviceProfiles.value(j);
              fileName = deviceFile.value(j);
              profile_child->setText(1, ("Device Profile / " + fileName));
              profile_child->setText(0, profileName);
              device_child->addChild(profile_child);
         }      
     }     
}

// Function to add profile items into the tree listing.
void kminfo::addProfileTreeItem( oyPROFILE_e profile_type, QString description, 
                                        QTreeWidgetItem * parent_item )
{
      oyProfile_s * profile = oyProfile_FromStd( profile_type, 0);
      oyProfileTag_s * tagID = oyProfile_GetTagById( profile, icSigProfileDescriptionTag  );

      int text_n = 0;
      char ** tagText = oyProfileTag_GetText( tagID, &text_n, 0,0, 0, 0 );
      
      // Add new item.
      QTreeWidgetItem * new_child = new QTreeWidgetItem();
      new_child->setText(1, description);
      new_child->setText(0, *tagText);
          
      parent_item->addChild(new_child);    
} 

// Whenever a user clicks on a child in the tree list, the "profile information"
// window is updated.
void kminfo::changeProfileTreeItem(QTreeWidgetItem* currentProfileItem)
{     
      int row_of_parent;
      
      QTreeWidgetItem * matchingParentItem = currentProfileItem->parent(); 

      // Is this a device item profile that was selected?
      QString isValidItemString = currentProfileItem->text(1);

      // If user clicks on an editing/assumed space item, find which one based on index.
      if (matchingParentItem == editingCsTree)
      {
            row_of_parent = editingCsTree->indexOfChild(currentProfileItem);  

            switch(row_of_parent)
            {
                 case (0): populateTagDescriptions(oyEDITING_RGB);
                           break;
                 case (1): populateTagDescriptions(oyEDITING_CMYK);
                           break;
                 case (2): populateTagDescriptions(oyEDITING_XYZ);
                           break;
                 case (3): populateTagDescriptions(oyEDITING_LAB);                                 
            }
            profileInfoGroupBox -> setEnabled(true);
      }    
      else if (matchingParentItem == assumedCsTree)
      {
            row_of_parent = assumedCsTree->indexOfChild(currentProfileItem);

            switch(row_of_parent)
            {
                 case (0): populateTagDescriptions(oyASSUMED_RGB);
                           break;
                 case (1): populateTagDescriptions(oyASSUMED_CMYK);
                           break;
                 case (2): populateTagDescriptions(oyASSUMED_XYZ);
                           break;
                 case (3): populateTagDescriptions(oyASSUMED_LAB);            
 
            }
            profileInfoGroupBox -> setEnabled(true);
       }

       else if(isValidItemString != 0)
       {
              QString grabProfileFilename = currentProfileItem->text(1);
              int str_index = grabProfileFilename.indexOf("/ ");
              
              grabProfileFilename.remove(0, str_index + 2);

              filenameTagLabel->setText(grabProfileFilename);
              
              oyProfile_s * profile;
              profile = oyProfile_FromFile( grabProfileFilename.toLocal8Bit(), 0, 0);
               
              populateDeviceProfileDescriptions(profile, true);    
              profileInfoGroupBox -> setEnabled(true);
        }
        else
        {
             populateDeviceProfileDescriptions(NULL, false);
             profileInfoGroupBox -> setEnabled(false);
	     // set default fram size
             frame -> setMinimumSize(QSize(250,250));
        }
        
   
}

// Refresh profile information list.
void kminfo::populateDeviceProfileDescriptions(oyProfile_s * profile, bool valid)
{
    if (valid)
    {
        // Output Oyranos-specified profile descriptions.
        setTagDescriptions(profile, icSigCopyrightTag, copyrightTagLabel);
        
	setTagDescriptions(profile, icSigDeviceMfgDescTag, mfgTagLabel);
        
	setTagDescriptions(profile, icSigDeviceModelDescTag, modelTagLabel);
        
	setTagDescriptions(profile, icSigProfileDescriptionTag, descriptionTagLabel);

        setDateTag(profile, dateTagLabel);
	
        setCSpaceTag(profile, colorspaceTagLabel);
	
        setIccsTag(profile, iccVerTagLabel);
	
        setPcsTag(profile, pcsTagLabel);
	
        setDeviceClassTag(profile, deviceClassTagLabel);

        QString profilePathName = oyProfile_GetFileName( profile, 0 );
        directoryListingTag->setText(profilePathName);
    }
    else
    {
        // Set default descriptions.
        descriptionTagLabel -> setText(tr("No Profile Selected"));
        copyrightTagLabel -> setText(tr("(Copyright not available)"));
        filenameTagLabel -> setText("");
        mfgTagLabel -> setText("");
        modelTagLabel -> setText("");

        dateTagLabel -> setText("");
        colorspaceTagLabel -> setText("");
        iccVerTagLabel -> setText("");
        pcsTagLabel -> setText("");
        deviceClassTagLabel -> setText("");

        directoryListingTag -> setText("");
    }
  
}

// Refresh profile information list.
void kminfo::populateTagDescriptions(oyDEFAULT_PROFILE current_profile)
{
     oyProfile_s * profile;
     
     QString profilename;
     int sizeofProfilename;
     int indexofProfilename; 

     profile = oyProfile_FromStd( current_profile, 0);
     profilename = oyProfile_GetFileName( profile, 0 );
  
     // Save a copy of the entire pathname.
     QString profile_path = profilename;   

     // Eliminate path from profile name.
     sizeofProfilename = profilename.size();
     indexofProfilename = profilename.lastIndexOf( "/", sizeofProfilename - 1);
     profilename.remove(0, indexofProfilename + 1);
     
     // Output profile description and path separately.
     filenameTagLabel->setText(profilename);
     directoryListingTag->setText(profile_path);

     // Output Oyranos-specified profile descriptions.
     setTagDescriptions(profile, icSigCopyrightTag, copyrightTagLabel);     
     setTagDescriptions(profile, icSigDeviceMfgDescTag, mfgTagLabel);
     setTagDescriptions(profile, icSigDeviceModelDescTag, modelTagLabel);
     setTagDescriptions(profile, icSigProfileDescriptionTag, descriptionTagLabel);
     
     setDateTag(profile, dateTagLabel);
     setCSpaceTag(profile, colorspaceTagLabel);
     setIccsTag(profile, iccVerTagLabel);
     setPcsTag(profile, pcsTagLabel);
     setDeviceClassTag(profile, deviceClassTagLabel);
     
      int longestString = mfgTagLabel->text().length();
      if (modelTagLabel->text().length() > longestString)
	longestString = modelTagLabel->text().length();
      if (descriptionTagLabel->text().length() > longestString)
	longestString = descriptionTagLabel->text().length();
      if (dateTagLabel->text().length() > longestString)
	longestString = dateTagLabel->text().length();
      if (colorspaceTagLabel->text().length() > longestString)
	longestString = colorspaceTagLabel->text().length();
      if (iccVerTagLabel->text().length() > longestString)
	longestString = iccVerTagLabel->text().length();
      if (pcsTagLabel->text().length() > longestString)
	longestString = pcsTagLabel->text().length();
      if (deviceClassTagLabel->text().length() > longestString)
	longestString = deviceClassTagLabel->text().length();;
      if (directoryListingTag->text().length() > longestString)
	longestString = directoryListingTag->text().length();
      
      frame -> setMinimumSize(QSize((longestString + 16) * 8 ,250));
}

void kminfo::setIccsTag(oyProfile_s * profile, QLabel * iccsLabel)
{
    QString field1, field2, field3;

    icSignature vs = oyValueUInt32( oyProfile_GetSignature(profile, oySIGNATURE_VERSION) );
    char * v = (char*)&vs;
    
    field1 = (((int)v[0]));
    field2 = ((int)v[1]/16);
    field3 = ((int)v[1]%16);

    iccsLabel->setText(field1.setNum(((int)v[0])) + "." + field2.setNum(((int)v[1]/16)) + "." +
                               field3.setNum(((int)v[1]%16)));
}

void kminfo::setDeviceClassTag(oyProfile_s * profile, QLabel * devClassLabel)
{
     QString string;
     string = oyICCDeviceClassDescription( (icProfileClassSignature)
                         oyProfile_GetSignature(profile, oySIGNATURE_CLASS) );
     devClassLabel->setText(string.toLocal8Bit());
}

void kminfo::setPcsTag(oyProfile_s * profile, QLabel * pcsLabel)
{
     QString tagString;
     tagString = oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(profile, oySIGNATURE_PCS) );
     pcsLabel->setText("CIE" + tagString.toLocal8Bit());
}

void kminfo::setCSpaceTag(oyProfile_s * profile, QLabel * cSpaceLabel)
{
     QString tagString;
     tagString = oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(profile, oySIGNATURE_COLOUR_SPACE) );
     cSpaceLabel->setText(tagString);
}

void kminfo::setDateTag(oyProfile_s * profile, QLabel * dateLabel)
{
     uint year, month, day; 
     //uint hours, minutes, seconds;
     QString yearString, monthString, dayString;

     year = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_YEAR);
     month = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_MONTH);
     day = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_DAY);
     
     // NOTE (If we need hours/minutes/seconds, uncomment)
      //hours = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_HOURS);
      //minutes = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_MINUTES);
      //seconds = oyProfile_GetSignature(profile, oySIGNATURE_DATETIME_SECONDS);

     dateLabel->setText(monthString.setNum(month) + "/" + dayString.setNum(day) + "/" + yearString.setNum(year));
}

// Function to write signature head, based on profile, tag type, and QT Label.
void kminfo::setTagDescriptions(oyProfile_s * profile_name, icTagSignature tagType, QLabel * tagLabel )
{
     int text_n;
     bool error;
     
     //oyObject_s  object;
     char** tagText = 0;
     oyProfileTag_s * tagID;

     tagID = oyProfile_GetTagById( profile_name, tagType  );
    
     error = !tagID;
     
     if(!error)
         tagText = oyProfileTag_GetText( tagID, &text_n, 0,0, 0, 0 );
     
     if(text_n && tagText && tagText[0])
             tagLabel->setText(*tagText);          
     else
        tagLabel->setText("-----");
}


kminfo::~kminfo()
{

}

