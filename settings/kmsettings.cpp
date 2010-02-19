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


#include "kmsettings.h"

#include <KAboutData>
#include <KComponentData>
#include <KGenericFactory>
#include <KConfigGroup>
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
#include <QInputDialog>
#include <KConfig>
#include <QDataStream>
#include <KSharedConfig>

#include <oyranos.h>
#include <oyranos_config.h>
#include <oyranos_alpha.h>

// Code to provide KDE module functionality for Kolor Management.
K_PLUGIN_FACTORY( kmsettingsFactory, 
         registerPlugin<kmsettings>(); 
         )
K_EXPORT_PLUGIN( kmsettingsFactory("kmsettings") )


void kmsettings::load()
{                  
     loadPolicy();
}

void kmsettings::save()
{
    saveCustomXmlFile();
    KConfigGroup default_policy_save(m_config, "Custom_Policies");

    // Save the default policy for next session.
    default_policy_save.writeEntry("Default_Policy", default_policy);
    m_config->sync();

    settingsChanged = false;
}

kmsettings::kmsettings(QWidget *parent, const QVariantList &) :
    KCModule( kmsettingsFactory::componentData(), parent)
{ 
   
   KAboutData* about = new KAboutData(
        "kmsettings", 0, ki18n("KMSettings"), 0, KLocalizedString(),
        KAboutData::License_GPL,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    
    setAboutData( about );
    
   // Set up shared configuration pointer, and save configuration data to
   // the 'kolor-manager-globals' file.
   m_config = KSharedConfig::openConfig("kolor-manager-globals");
 
   setupUi(this);              // Load Gui.

   setPolicyButton->setEnabled(false);
   removePolicyButton->setEnabled(false);

   loadEditableItems();        // Store all setting widgets into a convenient list structure.

   setupDirectory();           // Load XDG directory.

   populateProfiles();         // Load all Oyranos-specified profiles
                               // into each combobox in the "Default Profiles" tab.

   // Load saved default profile.
   KConfigGroup default_policy_load(m_config, "Custom_Policies");   
   selected_policy = default_policy_load.readEntry("Default_Policy", QString() );

   setPolicy();
 
   settingsChanged = false;

   // Load behavior settings and display current default policy.
   populateBehaviorSettings();

   // QT-related SIGNAL/SLOT functions, such as button presses and clicking
   // on a particular item.
   connect(policySettingsList, SIGNAL(itemClicked(QListWidgetItem*)), 
        this, SLOT(selectPolicy(QListWidgetItem*)));   
   connect(setPolicyButton, SIGNAL(clicked()), this, SLOT(setPolicy()));
   connect(addNewPolicyButton, SIGNAL(clicked()), this, SLOT(addNewPolicy()));
   connect(removePolicyButton, SIGNAL(clicked()), this, SLOT(removeCustomPolicy()));

   int k = 0;   
   // When a user clicks on any combo box, the "Apply" button will be enabled.
   for(k = 0; k < editableComboItems.size(); k++)
    connect(editableComboItems.value(k), SIGNAL(activated(int)), this, SLOT(emitChanged()));

   // When a user clicks on a radio box, the "Apply" button will be enabled.
   for(k = 0; k < editableCheckBoxItems.size(); k++)
    connect(editableCheckBoxItems.value(k), SIGNAL(clicked()), this, SLOT(emitChanged()));    
}

// Load XML directory for saving and loading policies.
void kmsettings::setupDirectory()
{
     char * ifXDGDirExists = getenv("XDG_CONFIG_HOME");
 
     // Check to see if user system has the environmental variable set up.
     if(ifXDGDirExists)
        customProfileDirectory = getenv("XDG_CONFIG_HOME");     // Set up directory to save xml files.
     else 
     {  
        customProfileDirectory = getenv("HOME");
        customProfileDirectory.append("/.config/color/settings/");
     }

     // Set up 'default' policy directories.
     office_policy_path = customProfileDirectory;
     office_policy_path.append("office.policy.xml");

     designer_policy_path = customProfileDirectory;
     designer_policy_path.append("designer.policy.xml");

     printer_policy_path = customProfileDirectory;
     printer_policy_path.append("prepress.policy.xml");

     photographer_policy_path = customProfileDirectory;
     photographer_policy_path.append("photographer.policy.xml");
}

// Load editable comboboxe and checkbox items into a list
//  (this is convenient to detect each settings change by the user).
void kmsettings::loadEditableItems()
{      
     editableComboItems.push_front(editRgbCombo);
     editableComboItems.push_front(editCmykCombo);   
     editableComboItems.push_front(editXyzCombo);
     editableComboItems.push_front(editLabCombo);
     editableComboItems.push_front(assumeRgbCombo);
     editableComboItems.push_front(assumeCmykCombo);   
     editableComboItems.push_front(assumeXyzCombo);
     editableComboItems.push_front(assumeLabCombo);

     editableComboItems.push_front(editGrayCombo);
     editableComboItems.push_front(assumeGrayCombo);   
     
     editableComboItems.push_front(defaultRIntentCombo);
     editableComboItems.push_front(ifNoProfileCombo);    
     editableComboItems.push_front(ifRgbMismatchCombo);
     editableComboItems.push_front(ifCmykMismatchCombo);
     editableComboItems.push_front(proofRIntentCombo);    
     editableComboItems.push_front(monitorMixedCombo);
     editableComboItems.push_front(proofColorProfileCombo);
     editableComboItems.push_front(printerMixedCombo);     

     editableCheckBoxItems.push_front(softProofCheckbox);
     editableCheckBoxItems.push_front(hardProofCheckbox);
     
}

//  Fill the combo boxes in the "Default Profiles" tab with Oyranos-installed profiles.
void kmsettings::populateProfiles()
{
    // Fill comboboxes with each 'filtered' profile.
    fillProfileComboBoxes(oyEDITING_RGB, editRgbCombo);
    fillProfileComboBoxes(oyEDITING_CMYK, editCmykCombo);
    fillProfileComboBoxes(oyEDITING_LAB, editLabCombo);
    fillProfileComboBoxes(oyEDITING_XYZ, editXyzCombo);
    
    // FIXME Not sure if gray works?
    fillProfileComboBoxes(oyEDITING_GRAY, editGrayCombo);

    fillProfileComboBoxes(oyASSUMED_RGB, assumeRgbCombo);
    fillProfileComboBoxes(oyASSUMED_CMYK, assumeCmykCombo);
    fillProfileComboBoxes(oyASSUMED_LAB, assumeLabCombo);
    fillProfileComboBoxes(oyASSUMED_XYZ, assumeXyzCombo);
    
    // FIXME Not sure if gray works?
    fillProfileComboBoxes(oyASSUMED_GRAY, assumeGrayCombo);  

    fillProfileComboBoxes(oyASSUMED_RGB, proofColorProfileCombo);
}

// Filter comboboxes in 'Default Profiles' with appropriate profiles.
void kmsettings::fillProfileComboBoxes(oyPROFILE_e profile_type, QComboBox * profile_combobox)
{
    int current = 0, size, i,
                  sizeofProfilename,
                  indexofProfilename;
    oyProfile_s * temp_prof = 0;
    oyProfiles_s * iccs = 0;

    QString profile_text, profile_filename;

    iccs = oyProfiles_ForStd( profile_type, &current, 0 );

    size = oyProfiles_Count(iccs);
    for( i = 0; i < size; i++)
    {
       temp_prof = oyProfiles_Get( iccs, i );
       profile_text = oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION );
       profile_filename = oyProfile_GetFileName(temp_prof, 0);

       sizeofProfilename =  profile_filename.size();
       indexofProfilename = profile_filename.lastIndexOf( "/", sizeofProfilename - 1);
       profile_filename.remove(0, indexofProfilename + 1);

       profile_combobox->insertItem(i, profile_filename, 0);

       oyProfile_Release( &temp_prof );
    }
}


// Populate each setting in "Behavior Settings".
void kmsettings::populateBehaviorSettings()
{ 
     int behavior_setting;

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_RENDERING_INTENT);
     defaultRIntentCombo->setCurrentIndex(behavior_setting);

//  Populate Mismatch Handling Settings
   
     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN);
     ifNoProfileCombo->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB);
     ifRgbMismatchCombo->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK);
     ifCmykMismatchCombo->setCurrentIndex(behavior_setting);

//  Set up Proofing Settings  
  
     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_RENDERING_INTENT_PROOF);
     proofRIntentCombo->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_PROOF_SOFT);
     if(behavior_setting == 1)
          softProofCheckbox->setChecked(true);
     else 
          softProofCheckbox->setChecked(false);
      
     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_PROOF_HARD);

     if(behavior_setting == 1)
          hardProofCheckbox->setChecked(true);
     else 
          hardProofCheckbox->setChecked(false);

// Set up Mixed Color Settings

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN);
     monitorMixedCombo->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT);
     printerMixedCombo->setCurrentIndex(behavior_setting);     
}

// Last "clicked on" policy by the user.
void kmsettings::selectPolicy(QListWidgetItem* selectedPolicyItem)
{
     setPolicyButton->setEnabled(true);

    // If user makes a settings change, and then clicks on a different policy...
    if (settingsChanged == true && isCustom == true)   
    {
        if(KMessageBox::questionYesNo(this,
        i18n("The policy settings for '" + selected_policy.toLocal8Bit() + "' have changed. \nDo you wish to save them?"),
        i18n("Policy settings have been modified...")) == KMessageBox::Yes)
            
            save();

            settingsChanged = false;
            changed(false);
     }

     selected_policy = selectedPolicyItem->text();

     if (selected_policy == "[Home / Office]"  ||
         selected_policy == "[Graphic Design]" ||
         selected_policy == "[Printer]"        ||
         selected_policy == "[Photography]"       )
     {
         isCustom = false;
         removePolicyButton->setEnabled(false);
     }
     
     else
     {
         isCustom = true;             // This is a custom policy.
         
         removePolicyButton->setEnabled(true);
          
         oyPolicySet(policyNameToXmlFile(selectedPolicyItem->text()), 
                              policyNameToXmlPath(selectedPolicyItem->text()));
     }

     if (selected_policy == "[Home / Office]")
         oyPolicySet("office.policy.xml", office_policy_path.toLocal8Bit());
     else if (selected_policy == "[Graphic Design]")
         oyPolicySet("designer.policy.xml", designer_policy_path.toLocal8Bit());
     else if (selected_policy == "[Printer]")
         oyPolicySet("prepress.policy.xml", printer_policy_path.toLocal8Bit());
     else if (selected_policy == "[Photography]")
         oyPolicySet("photographer.policy.xml", photographer_policy_path.toLocal8Bit());
   
     // Make sure the user doesn't delete the current policy settings!
     if(default_policy == selectedPolicyItem->text())
         removePolicyButton->setEnabled(false);

     populateBehaviorSettings();       // Refresh settings in "Behavior Settings"
     refreshProfileSettings();         // Refresh comboboxes in "Default Profiles"

     setEditableItems(isCustom);
}

//  Refresh profile combo boxes with profiles associated with the current policy.
void kmsettings::refreshProfileSettings()
{
     int profileSearchIndex;  // Index to match profile stored in a combobox.

     QString xmlToString;     // String used to grab each default colorspace
                              // profile from Oyranos.

     /* Each item will grab the default profile, and match it with an available profile
        under each combo box in "Default Profiles".  The combobox will then display
        the default profile.                                                     */
     xmlToString = oyGetDefaultProfileName (oyEDITING_RGB, 0); 
     profileSearchIndex = editRgbCombo->findText( xmlToString, Qt::MatchExactly);
     editRgbCombo->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyEDITING_CMYK, 0);
     profileSearchIndex = editCmykCombo->findText( xmlToString, Qt::MatchExactly);
     editCmykCombo->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyEDITING_XYZ, 0);
     profileSearchIndex = editXyzCombo->findText( xmlToString, Qt::MatchExactly);
     editXyzCombo->setCurrentIndex(profileSearchIndex);
    
     xmlToString = oyGetDefaultProfileName (oyEDITING_LAB, 0);
     profileSearchIndex = editLabCombo->findText( xmlToString, Qt::MatchExactly);
     editLabCombo->setCurrentIndex(profileSearchIndex);

     xmlToString = oyGetDefaultProfileName (oyEDITING_GRAY, 0);
     profileSearchIndex = editGrayCombo->findText( xmlToString, Qt::MatchExactly);
     editGrayCombo->setCurrentIndex(profileSearchIndex);  
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_RGB, 0);
     profileSearchIndex = assumeRgbCombo->findText( xmlToString, Qt::MatchExactly);
     assumeRgbCombo->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_CMYK, 0);
     profileSearchIndex = editCmykCombo->findText( xmlToString, Qt::MatchExactly);
     assumeCmykCombo->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_XYZ, 0);
     profileSearchIndex = editXyzCombo->findText( xmlToString, Qt::MatchExactly);
     assumeXyzCombo->setCurrentIndex(profileSearchIndex);
    
     xmlToString = oyGetDefaultProfileName (oyASSUMED_LAB, 0);
     profileSearchIndex = assumeLabCombo->findText( xmlToString, Qt::MatchExactly);
     assumeLabCombo->setCurrentIndex(profileSearchIndex);  

     xmlToString = oyGetDefaultProfileName (oyASSUMED_GRAY, 0);
     profileSearchIndex = assumeGrayCombo->findText( xmlToString, Qt::MatchExactly);
     assumeGrayCombo->setCurrentIndex(profileSearchIndex);  

     xmlToString = oyGetDefaultProfileName (oyPROFILE_PROOF, 0);
     profileSearchIndex = proofColorProfileCombo->findText( xmlToString, Qt::MatchExactly);
     proofColorProfileCombo->setCurrentIndex(profileSearchIndex);    
}

// Set Policy as default.
void kmsettings::setPolicy()
{
     isCustom = false;    // For now, we assume the user is using a default policy.        

     // Set user selected policy as system default.
     currentPolicyLabel->setText(selected_policy);      // Update default policy label.
     default_policy = selected_policy;    

     // Check to see if the policy is one of the four 'defaults' or a custom-made one.
     if (default_policy == "[Home / Office]")
         oyPolicySet("office.policy.xml", office_policy_path.toLocal8Bit());
     else if (default_policy == "[Graphic Design]")
         oyPolicySet("designer.policy.xml", designer_policy_path.toLocal8Bit());
     else if (default_policy == "[Printer]")
         oyPolicySet("prepress.policy.xml", printer_policy_path.toLocal8Bit());
     else if (default_policy == "[Photography]")
         oyPolicySet("photographer.policy.xml", photographer_policy_path.toLocal8Bit());
     else 
     {
           // Refresh custom profile.
           oyPolicySet(policyNameToXmlFile(selected_policy), 
                              policyNameToXmlPath(selected_policy));

            //isCustom = true;      // This is a custom policy.
      }             

     save();

     // Once a new policy is obtained, the UI needs to update the settings.
     populateBehaviorSettings();       // Refresh settings in "Behavior Settings"
     refreshProfileSettings();         // Refresh comboboxes in "Default Profiles"

     setEditableItems(isCustom);
}

// This function controls whether or not the settings are read-only.
void kmsettings::setEditableItems(bool itemStatus)
{   
     QComboBox * combobox;
     for (int i = 0; i < editableComboItems.size(); i++)
     {
         combobox = editableComboItems.value(i);
         combobox->setEnabled(itemStatus);
     }

     QRadioButton * checkbox;
     for (int i = 0; i < editableCheckBoxItems.size(); i++)
     {
         checkbox = editableCheckBoxItems.value(i);
         checkbox->setEnabled(itemStatus);
     }
}

/*
   When the user wants to add a custom policy, 
   the program will prompt user for new policy name 
   and add to the installed policies list.      */

void kmsettings::addNewPolicy()
{   
   QString newPolicyString;
   bool ok;
   
   // New policy name dialog.
   xmlFileName = QInputDialog::getText(this, tr("Add new policy"), 
                      tr("Please enter a name for the new policy: "), 
                      QLineEdit::Normal, QDir::home().dirName(), &ok);
 
   if(ok)   
   {
       int i;
       QListWidgetItem * temp_item;

       for(i = 0; i < policySettingsList->count(); i++)
       {
            temp_item = policySettingsList->item(i);
            if (xmlFileName == temp_item->text())
            {
                KMessageBox::error(this,
                  i18n("The policy name '" + xmlFileName.toLocal8Bit() + "' already exists!"),
                  i18n("Unable to add policy..."));
                
                  return;
            }
       }    
       
       policySettingsList->addItem(xmlFileName);     // Add policy name to list.
   }
   else 
      return;
  
   savePolicy();

   // Create a "blank" XML file based on entered name.
   selected_policy = xmlFileName;
   saveCustomXmlFile();
}

void kmsettings::removeCustomPolicy()
{        
     // Remove policy from list and KConfig file.  
     QListWidgetItem * deleted_item = policySettingsList->takeItem(policySettingsList->currentRow());
     savePolicy();

     // Remove actual Xml file from directory.
     QFile file(policyNameToXmlPath(deleted_item->text()));
     file.remove();     
}

void kmsettings::saveSettingsToXml()
{     
     saveCustomXmlFile();
}

// Convert policy name to Oyranos-readable XML file pathname. 
const char* kmsettings::policyNameToXmlPath(QString policyName)
{
     QString fullXmlPath = customProfileDirectory;
     fullXmlPath.append(policyName);
     fullXmlPath.append(".xml");

     return fullXmlPath.toLocal8Bit();
}

// Convert policy name to Oyranos-readable XML name. 
const char* kmsettings::policyNameToXmlFile(QString policyName)
{
      policyName.append(".xml");
      return policyName.toLocal8Bit();
}

// Create a new file that's currently stored in the customProfileDirectory QString.

void kmsettings::saveCustomXmlFile()
{ 
    QString stringToXml;    
    int behaviorSetting;    
   
    stringToXml = editRgbCombo->currentText();
    oySetDefaultProfile(oyEDITING_RGB, stringToXml.toLocal8Bit()); 
 
    stringToXml = editCmykCombo->currentText();
    oySetDefaultProfile(oyEDITING_CMYK, stringToXml.toLocal8Bit());

    stringToXml = editXyzCombo->currentText();
    oySetDefaultProfile(oyEDITING_XYZ, stringToXml.toLocal8Bit());

    stringToXml = editLabCombo->currentText();
    oySetDefaultProfile(oyEDITING_LAB, stringToXml.toLocal8Bit());

    stringToXml = assumeRgbCombo->currentText();
    oySetDefaultProfile(oyASSUMED_RGB, stringToXml.toLocal8Bit());

    stringToXml = assumeCmykCombo->currentText();
    oySetDefaultProfile(oyASSUMED_CMYK, stringToXml.toLocal8Bit());

    stringToXml = assumeLabCombo->currentText();
    oySetDefaultProfile(oyASSUMED_LAB, stringToXml.toLocal8Bit());

    stringToXml = assumeXyzCombo->currentText();
    oySetDefaultProfile(oyASSUMED_XYZ, stringToXml.toLocal8Bit());

    //----------------------------------------------------------------

    behaviorSetting = defaultRIntentCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_RENDERING_INTENT, behaviorSetting );

    behaviorSetting = ifNoProfileCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN, behaviorSetting );

    behaviorSetting = ifRgbMismatchCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB , behaviorSetting );
  
    behaviorSetting = ifCmykMismatchCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK , behaviorSetting );

    behaviorSetting = proofRIntentCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_RENDERING_INTENT_PROOF , behaviorSetting );

    behaviorSetting = monitorMixedCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN , behaviorSetting );

    behaviorSetting = printerMixedCombo->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT , behaviorSetting );

    if (softProofCheckbox->isChecked())
        oySetBehaviour ( oyBEHAVIOUR_PROOF_SOFT , 1 );
    else
        oySetBehaviour ( oyBEHAVIOUR_PROOF_SOFT , 0 );

    if (hardProofCheckbox->isChecked())
        oySetBehaviour( oyBEHAVIOUR_PROOF_HARD , 1 );
    else 
        oySetBehaviour(oyBEHAVIOUR_PROOF_HARD  , 0 );

    oyPolicySaveActual(oyGROUP_ALL, selected_policy.toLocal8Bit()); 

}

// Used to enable the "Apply" button.
void kmsettings::emitChanged()
{
     // Ungrey "Apply" button. 
     settingsChanged = true;
     emit changed(true);     
}

// Function to save/refresh installed policies in system.
void kmsettings::savePolicy()
{
     KConfigGroup policyGroup(m_config, "Custom_Policies");

     QString tempProfile;
     QStringList policyList;

     policyGroup.deleteEntry("Custom_Policies_Installed");

     if ( policySettingsList->count() >= 4)
     {   
         QListWidgetItem * temp_item;

         for (int i = 4; i < policySettingsList->count(); i++)
         {
              
              temp_item = policySettingsList->item(i);
              tempProfile = temp_item->text();

              policyList.insert(0, tempProfile);
         }
     }

     policyGroup.writeEntry("Custom_Policies_Installed", policyList);
}

// Function to load the installed policies in system.
void kmsettings::loadPolicy()
{
     KConfigGroup installedCustomPolicies(m_config, "Custom_Policies");
     QStringList policyList = installedCustomPolicies.readEntry("Custom_Policies_Installed", QStringList());

     if (policyList.empty())
         return;
     else
     {
         QString tempProfileName;

         for (int i = 0; i < policyList.size(); i++)
         {       
              tempProfileName = policyList.value(i);
              policySettingsList->addItem(tempProfileName);        
         }
     }
     
}

kmsettings::~kmsettings()
{
     
}

