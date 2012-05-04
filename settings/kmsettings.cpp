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

#include <KDebug>
#include <KAboutData>
#include <KComponentData>
#include <KGenericFactory>
//#include <KConfigGroup>
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
#include <QDataStream>
#include <KSharedConfig>

#include <oyranos.h>
#include <oyranos_config.h>
#include <alpha/oyranos_alpha.h>
#include <locale.h>

// The number of policies that are available in KM by default.
// NOTE These are listed *before* the user-defined policies.
const int internalPolicyCount = 4;

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
    settingsChanged = false;
}

// Register the panel and initialise fro Oyranos.
kmsettings::kmsettings(QWidget *parent, const QVariantList &) :
    KCModule( kmsettingsFactory::componentData(), parent)
{ 
   
   KAboutData* about = new KAboutData(
        "kmsettings", 0, ki18n("KMSettings"), 0, KLocalizedString(),
        KAboutData::License_BSD,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("2008-2009 Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    about->addAuthor( ki18n("2010 Kai-Uwe Behrmann"), KLocalizedString(),
                      "ku.b@gmx.de"  );

    setAboutData( about );

   setupUi(this);              // Load Gui.

   removePolicyButton->setEnabled(false);

   loadEditableItems();        // Store all setting widgets into a convenient list structure.

   populateProfiles();         // Load all Oyranos-specified profiles
                               // into each combobox in the "Default Profiles" tab.


   settingsChanged = false;

  /* i18n */
   QString qs;
   int i;
   int count = 0,
       current = 0,
       flags = 0;
   const char    * tooltip = NULL;
   const char   ** names = NULL;
   const char    * label = NULL;
   oyWIDGET_TYPE_e type = oyWIDGETTYPE_START;

  // Get widget strings.
  type = oyWidgetTitleGet( oyWIDGET_POLICY, NULL, &label, &tooltip, &flags );
  qs = QString::fromLocal8Bit(label);
  policySettingsBox->setTitle(qs);
  kmsettingsTab->setTabText(0,qs);
  qs = QString::fromLocal8Bit( tooltip );
  policyLabel->setText(qs);

  type = oyWidgetTitleGet( oyWIDGET_GROUP_DEFAULT_PROFILES, NULL, &label, &tooltip, &flags );
  Q_UNUSED(type);
  qs = QString::fromLocal8Bit(label);
  kmsettingsTab->setTabText(1,qs);
  
  type = oyWidgetTitleGet( oyWIDGET_GROUP_BEHAVIOUR, NULL, &label, &tooltip, &flags );
  Q_UNUSED(type);
  qs = QString::fromLocal8Bit(label,-1);
  kmsettingsTab->setTabText(2,qs);

#define SET_OY_PROFILE_WIDGET( widget ) \
  oyWidgetTitleGet( oyWIDGET_##widget, NULL, &label, &tooltip, &flags ); \
  qs = QString::fromLocal8Bit(label); \
  label_##widget->setText( qs ); \
  qs = QString::fromLocal8Bit( tooltip ); \
  label_##widget->setToolTip( qs ); \
  combo_##widget->setToolTip( qs );

#define SET_OY_BOX_WIDGET( widget ) \
  oyWidgetTitleGet( oyWIDGET_##widget, NULL, &label, &tooltip, &flags ); \
  qs = QString::fromLocal8Bit(label); \
  box_##widget->setTitle( qs ); \
  qs = QString::fromLocal8Bit( tooltip ); \
  box_##widget->setToolTip( qs );

#define SET_OY_COMBO_WIDGET( widget ) \
  oyWidgetTitleGet( oyWIDGET_##widget, NULL, &label, &tooltip, &flags ); \
  oyOptionChoicesGet( oyWIDGET_##widget, &count, &names, &current ); \
  qs = QString::fromLocal8Bit( tooltip ); \
  combo_##widget->setToolTip( qs ); \
  combo_##widget->clear(); \
  for(i = 0; i < count; ++i) \
  { \
    qs = QString::fromLocal8Bit( names[i] ); \
    combo_##widget->addItem( qs ); \
  }

#define SET_OY_CHECK_WIDGET( widget ) \
  oyWidgetTitleGet( oyWIDGET_##widget, NULL, &label, &tooltip, &flags ); \
  qs = QString::fromLocal8Bit( label ); \
  check_##widget->setText( qs ); \
  qs = QString::fromLocal8Bit( tooltip ); \
  check_##widget->setToolTip( qs );

#define SET_OY_LABEL_WIDGET( widget ) \
  oyWidgetTitleGet( oyWIDGET_##widget, NULL, &label, &tooltip, &flags ); \
  qs = QString::fromLocal8Bit(label); \
  label_##widget->setText( qs ); \
  qs = QString::fromLocal8Bit( tooltip ); \
  label_##widget->setToolTip( qs );

  SET_OY_PROFILE_WIDGET( EDITING_RGB );
  SET_OY_PROFILE_WIDGET( EDITING_CMYK );
  SET_OY_PROFILE_WIDGET( EDITING_LAB );
  SET_OY_PROFILE_WIDGET( EDITING_XYZ );
  SET_OY_PROFILE_WIDGET( EDITING_GRAY );
  SET_OY_PROFILE_WIDGET( ASSUMED_RGB );
  SET_OY_PROFILE_WIDGET( ASSUMED_CMYK );
  SET_OY_PROFILE_WIDGET( ASSUMED_LAB );
  SET_OY_PROFILE_WIDGET( ASSUMED_XYZ );
  SET_OY_PROFILE_WIDGET( ASSUMED_GRAY );
  SET_OY_PROFILE_WIDGET( ASSUMED_WEB );
  SET_OY_PROFILE_WIDGET( PROFILE_PROOF );
  SET_OY_BOX_WIDGET( GROUP_BEHAVIOUR_RENDERING );
  SET_OY_BOX_WIDGET( GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS );
  SET_OY_BOX_WIDGET( GROUP_BEHAVIOUR_MISSMATCH );
  SET_OY_BOX_WIDGET( GROUP_BEHAVIOUR_PROOF );
  SET_OY_COMBO_WIDGET( ACTION_UNTAGGED_ASSIGN );
  SET_OY_LABEL_WIDGET( ACTION_UNTAGGED_ASSIGN );
  SET_OY_COMBO_WIDGET( ACTION_OPEN_MISMATCH_RGB );
  SET_OY_LABEL_WIDGET( ACTION_OPEN_MISMATCH_RGB );
  SET_OY_COMBO_WIDGET( ACTION_OPEN_MISMATCH_CMYK );
  SET_OY_LABEL_WIDGET( ACTION_OPEN_MISMATCH_CMYK );
  SET_OY_COMBO_WIDGET( MIXED_MOD_DOCUMENTS_PRINT );
  SET_OY_LABEL_WIDGET( MIXED_MOD_DOCUMENTS_PRINT );
  SET_OY_COMBO_WIDGET( MIXED_MOD_DOCUMENTS_SCREEN );
  SET_OY_LABEL_WIDGET( MIXED_MOD_DOCUMENTS_SCREEN );
  SET_OY_COMBO_WIDGET( RENDERING_INTENT );
  SET_OY_COMBO_WIDGET( RENDERING_INTENT_PROOF );
  SET_OY_LABEL_WIDGET( RENDERING_INTENT_PROOF );
  SET_OY_CHECK_WIDGET( RENDERING_BPC );
  SET_OY_CHECK_WIDGET( PROOF_SOFT );
  SET_OY_CHECK_WIDGET( PROOF_HARD );
  SET_OY_CHECK_WIDGET( RENDERING_GAMUT_WARNING );

   // Load behavior settings and display current default policy.
   populateBehaviorSettings();
   refreshProfileSettings();         // Refresh comboboxes in "Default Profiles"
   refreshPolicySettings();

   // We want to be able to observe changes of single settings and display
   // the actual used policy. So track them.
   // QT-related SIGNAL/SLOT functions, such as button presses and clicking
   // on a particular item.
   connect(policySettingsList, SIGNAL(currentRowChanged(int)), 
        this, SLOT(selectPolicy(int)));   
   connect(addNewPolicyButton, SIGNAL(clicked()), this, SLOT(addNewPolicy()));
   connect(removePolicyButton, SIGNAL(clicked()), this, SLOT(removeCustomPolicy()));

   int k = 0, n = editableComboItems.size();
   // When a user clicks on any combo box, the "Apply" button will be enabled.
   for(k = 0; k < n; ++k)
    connect(editableComboItems.value(k), SIGNAL(activated(int)), this, SLOT(emitChanged()));

   n = editableCheckBoxItems.size();
   // When a user clicks on a radio box, the "Apply" button will be enabled.
   for(k = 0; k < n; ++k)
    connect(editableCheckBoxItems.value(k), SIGNAL(clicked()), this, SLOT(emitChanged()));    


}


// Load editable comboboxe and checkbox items into a list
//  (this is convenient to detect each settings change by the user).
void kmsettings::loadEditableItems()
{      
     editableComboItems.push_front(combo_EDITING_RGB);
     editableComboItems.push_front(combo_EDITING_CMYK);   
     editableComboItems.push_front(combo_EDITING_XYZ);
     editableComboItems.push_front(combo_EDITING_LAB);
     editableComboItems.push_front(combo_ASSUMED_RGB);
     editableComboItems.push_front(combo_ASSUMED_CMYK);   
     editableComboItems.push_front(combo_ASSUMED_XYZ);
     editableComboItems.push_front(combo_ASSUMED_LAB);

     editableComboItems.push_front(combo_EDITING_GRAY);
     editableComboItems.push_front(combo_ASSUMED_GRAY);   
     
     editableComboItems.push_front(combo_RENDERING_INTENT);
     editableComboItems.push_front(combo_ACTION_UNTAGGED_ASSIGN);    
     editableComboItems.push_front(combo_ACTION_OPEN_MISMATCH_RGB);
     editableComboItems.push_front(combo_ACTION_OPEN_MISMATCH_CMYK);
     editableComboItems.push_front(combo_RENDERING_INTENT_PROOF);    
     editableComboItems.push_front(combo_MIXED_MOD_DOCUMENTS_SCREEN);
     editableComboItems.push_front(combo_MIXED_MOD_DOCUMENTS_PRINT);     
     editableComboItems.push_front(combo_PROFILE_PROOF);

     editableCheckBoxItems.push_front(check_PROOF_SOFT);
     editableCheckBoxItems.push_front(check_PROOF_HARD);
     editableCheckBoxItems.push_front(check_RENDERING_BPC);
     editableCheckBoxItems.push_front(check_RENDERING_GAMUT_WARNING);
     
}

//  Fill the combo boxes in the "Default Profiles" tab with Oyranos-installed profiles.
void kmsettings::populateProfiles()
{
    // Fill comboboxes with each 'filtered' profile.
    fillProfileComboBoxes(oyEDITING_RGB, combo_EDITING_RGB);
    fillProfileComboBoxes(oyEDITING_CMYK, combo_EDITING_CMYK);
    fillProfileComboBoxes(oyEDITING_LAB, combo_EDITING_LAB);
    fillProfileComboBoxes(oyEDITING_XYZ, combo_EDITING_XYZ);
    fillProfileComboBoxes(oyEDITING_GRAY, combo_EDITING_GRAY);

    fillProfileComboBoxes(oyASSUMED_RGB, combo_ASSUMED_RGB);
    fillProfileComboBoxes(oyASSUMED_CMYK, combo_ASSUMED_CMYK);
    fillProfileComboBoxes(oyASSUMED_LAB, combo_ASSUMED_LAB);
    fillProfileComboBoxes(oyASSUMED_XYZ, combo_ASSUMED_XYZ);
    fillProfileComboBoxes(oyASSUMED_GRAY, combo_ASSUMED_GRAY);  

    fillProfileComboBoxes(oyPROFILE_PROOF, combo_PROFILE_PROOF);
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
    for( i = 0; i < size; ++i)
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
     combo_RENDERING_INTENT->setCurrentIndex(behavior_setting);

//  Populate Mismatch Handling Settings
   
     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN);
     combo_ACTION_UNTAGGED_ASSIGN->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB);
     combo_ACTION_OPEN_MISMATCH_RGB->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK);
     combo_ACTION_OPEN_MISMATCH_CMYK->setCurrentIndex(behavior_setting);

//  Set up Proofing Settings  
  
     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_RENDERING_INTENT_PROOF);
     combo_RENDERING_INTENT_PROOF->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_RENDERING_BPC);
     if(behavior_setting == 1)
          check_RENDERING_BPC->setChecked(true);
     else 
          check_RENDERING_BPC->setChecked(false);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_RENDERING_GAMUT_WARNING);
     if(behavior_setting == 1)
          check_RENDERING_GAMUT_WARNING->setChecked(true);
     else 
          check_RENDERING_GAMUT_WARNING->setChecked(false);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_PROOF_SOFT);
     if(behavior_setting == 1)
          check_PROOF_SOFT->setChecked(true);
     else 
          check_PROOF_SOFT->setChecked(false);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_PROOF_HARD);

     if(behavior_setting == 1)
          check_PROOF_HARD->setChecked(true);
     else 
          check_PROOF_HARD->setChecked(false);

// Set up Mixed Color Settings

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN);
     combo_MIXED_MOD_DOCUMENTS_SCREEN->setCurrentIndex(behavior_setting);

     behavior_setting = oyGetBehaviour(oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT);
     combo_MIXED_MOD_DOCUMENTS_PRINT->setCurrentIndex(behavior_setting);     
}

// Last "clicked on" policy by the user.
void kmsettings::selectPolicy(int rowIndex)
{
    QListWidgetItem * selectedPolicyItem = policySettingsList->item(rowIndex);
       
    // If user makes a settings change, and then clicks on a different policy...
    if (settingsChanged == true && isCustom == true)   
    {
        if(KMessageBox::questionYesNo(this,
        i18n("The policy settings for '%1' have changed.\nDo you wish to save them?", selected_policy),
        i18n("Policy settings have been modified.")) == KMessageBox::Yes)
            
            save();

            settingsChanged = false;
            changed(false);
     }

     selected_policy = selectedPolicyItem->text();

     char * full_name = 0;
     std::string t = selected_policy.toStdString();
     oyPolicyFileNameGet( t.c_str(), &full_name, malloc );
     t.clear();
     QFile file( full_name );
     if(file.permissions() & QFile::WriteOwner)
     {
         isCustom = true;             // This is a custom policy.
         
         removePolicyButton->setEnabled(true);
         t = selectedPolicyItem->text().toStdString();
         oyPolicySet( t.c_str(), 0 );
     }
     if(full_name) free( full_name );
     
     // Do not remove "built-in" policies ('Office', 'Photographer', etc.)
     if (rowIndex < internalPolicyCount)
      removePolicyButton->setEnabled(false);
     else
      removePolicyButton->setEnabled(true);

     populateBehaviorSettings();       // Refresh settings in "Behavior Settings"
     refreshProfileSettings();         // Refresh comboboxes in "Default Profiles"
     refreshPolicySettings();

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
     profileSearchIndex = combo_EDITING_RGB->findText( xmlToString, Qt::MatchExactly);
     combo_EDITING_RGB->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyEDITING_CMYK, 0);
     profileSearchIndex = combo_EDITING_CMYK->findText( xmlToString, Qt::MatchExactly);
     combo_EDITING_CMYK->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyEDITING_XYZ, 0);
     profileSearchIndex = combo_EDITING_XYZ->findText( xmlToString, Qt::MatchExactly);
     combo_EDITING_XYZ->setCurrentIndex(profileSearchIndex);
    
     xmlToString = oyGetDefaultProfileName (oyEDITING_LAB, 0);
     profileSearchIndex = combo_EDITING_LAB->findText( xmlToString, Qt::MatchExactly);
     combo_EDITING_LAB->setCurrentIndex(profileSearchIndex);

     xmlToString = oyGetDefaultProfileName (oyEDITING_GRAY, 0);
     profileSearchIndex = combo_EDITING_GRAY->findText( xmlToString, Qt::MatchExactly);
     combo_EDITING_GRAY->setCurrentIndex(profileSearchIndex);  
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_RGB, 0);
     profileSearchIndex = combo_ASSUMED_RGB->findText( xmlToString, Qt::MatchExactly);
     combo_ASSUMED_RGB->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_CMYK, 0);
     profileSearchIndex = combo_EDITING_CMYK->findText( xmlToString, Qt::MatchExactly);
     combo_ASSUMED_CMYK->setCurrentIndex(profileSearchIndex);
     
     xmlToString = oyGetDefaultProfileName (oyASSUMED_XYZ, 0);
     profileSearchIndex = combo_EDITING_XYZ->findText( xmlToString, Qt::MatchExactly);
     combo_ASSUMED_XYZ->setCurrentIndex(profileSearchIndex);
    
     xmlToString = oyGetDefaultProfileName (oyASSUMED_LAB, 0);
     profileSearchIndex = combo_ASSUMED_LAB->findText( xmlToString, Qt::MatchExactly);
     combo_ASSUMED_LAB->setCurrentIndex(profileSearchIndex);  

     xmlToString = oyGetDefaultProfileName (oyASSUMED_GRAY, 0);
     profileSearchIndex = combo_ASSUMED_GRAY->findText( xmlToString, Qt::MatchExactly);
     combo_ASSUMED_GRAY->setCurrentIndex(profileSearchIndex);  

     xmlToString = oyGetDefaultProfileName (oyPROFILE_PROOF, 0);
     profileSearchIndex = combo_PROFILE_PROOF->findText( xmlToString, Qt::MatchExactly);
     combo_PROFILE_PROOF->setCurrentIndex(profileSearchIndex);    
}

// Show the actual policy name after changes applied to Oyranos options.
void kmsettings::refreshPolicySettings()
{
   // Load policy.
   int count = 0, current = -1;
   const char ** names = 0;
   oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );
   if(names && count && current >= 0)
   {
     selected_policy = names[current];
     // Set user selected policy as system default.
     currentPolicyLabel->setText(selected_policy);      // Update default policy label.
     kDebug() << "actual policy: %s\n" << names[current];
   } else
     currentPolicyLabel->setText("----");
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

     QCheckBox * checkbox;
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
   bool ok;
   
   // New policy name dialog.
   xmlFileName = QInputDialog::getText(this, i18n("Add new policy"), 
                      i18n("Please enter a name for the new policy:"), 
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
                  i18n("The policy name '%1' already exists.", xmlFileName),
                  i18n("Unable to add policy."));
                
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
   refreshPolicySettings();
}

void kmsettings::removeCustomPolicy()
{
     // Remove policy from list file.
     QListWidgetItem * deleted_item = policySettingsList->takeItem(policySettingsList->currentRow());
     savePolicy();

     // Remove actual Xml file from directory.
     char * full_name = 0;
     std::string t = deleted_item->text().toStdString();
     oyPolicyFileNameGet( t.c_str(),&full_name,malloc);
     QFile file( full_name );
     file.remove();
     if(full_name) free( full_name );
}

void kmsettings::saveSettingsToXml()
{     
   saveCustomXmlFile();
   refreshPolicySettings();
}


void kmsettings::saveSettings()
{ 
    std::string t;
    int behaviorSetting;    
   
    t = combo_EDITING_RGB->currentText().toStdString();
    oySetDefaultProfile(oyEDITING_RGB, t.c_str()); t.clear();
 
    t = combo_EDITING_CMYK->currentText().toStdString();
    oySetDefaultProfile(oyEDITING_CMYK, t.c_str()); t.clear();

    t = combo_EDITING_XYZ->currentText().toStdString();
    oySetDefaultProfile(oyEDITING_XYZ, t.c_str()); t.clear();

    t = combo_EDITING_LAB->currentText().toStdString();
    oySetDefaultProfile(oyEDITING_LAB, t.c_str()); t.clear();

    t = combo_EDITING_GRAY->currentText().toStdString();
    oySetDefaultProfile(oyEDITING_GRAY, t.c_str()); t.clear();

    t = combo_ASSUMED_RGB->currentText().toStdString();
    oySetDefaultProfile(oyASSUMED_RGB, t.c_str()); t.clear();

    t = combo_ASSUMED_CMYK->currentText().toStdString();
    oySetDefaultProfile(oyASSUMED_CMYK, t.c_str()); t.clear();

    t = combo_ASSUMED_LAB->currentText().toStdString();
    oySetDefaultProfile(oyASSUMED_LAB, t.c_str()); t.clear();

    t = combo_ASSUMED_XYZ->currentText().toStdString();
    oySetDefaultProfile(oyASSUMED_XYZ, t.c_str()); t.clear();

    t = combo_ASSUMED_GRAY->currentText().toStdString();
    oySetDefaultProfile(oyASSUMED_GRAY, t.c_str()); t.clear();

    t = combo_PROFILE_PROOF->currentText().toStdString();
    oySetDefaultProfile(oyPROFILE_PROOF, t.c_str()); t.clear();

    //----------------------------------------------------------------

    behaviorSetting = combo_RENDERING_INTENT->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_RENDERING_INTENT, behaviorSetting ); t.clear();

    behaviorSetting = combo_ACTION_UNTAGGED_ASSIGN->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN, behaviorSetting ); t.clear();

    behaviorSetting = combo_ACTION_OPEN_MISMATCH_RGB->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB , behaviorSetting ); t.clear();
  
    behaviorSetting = combo_ACTION_OPEN_MISMATCH_CMYK->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK , behaviorSetting ); t.clear();

    behaviorSetting = combo_RENDERING_INTENT_PROOF->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_RENDERING_INTENT_PROOF , behaviorSetting ); t.clear();

    behaviorSetting = combo_MIXED_MOD_DOCUMENTS_SCREEN->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN , behaviorSetting ); t.clear();

    behaviorSetting = combo_MIXED_MOD_DOCUMENTS_PRINT->currentIndex();
    oySetBehaviour ( oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT , behaviorSetting ); t.clear();

    if (check_RENDERING_BPC->isChecked())
        oySetBehaviour ( oyBEHAVIOUR_RENDERING_BPC , 1 );
    else
        oySetBehaviour ( oyBEHAVIOUR_RENDERING_BPC , 0 );

    if (check_RENDERING_GAMUT_WARNING->isChecked())
        oySetBehaviour ( oyBEHAVIOUR_RENDERING_GAMUT_WARNING , 1 );
    else
        oySetBehaviour ( oyBEHAVIOUR_RENDERING_GAMUT_WARNING , 0 );

    if (check_PROOF_SOFT->isChecked())
        oySetBehaviour ( oyBEHAVIOUR_PROOF_SOFT , 1 );
    else
        oySetBehaviour ( oyBEHAVIOUR_PROOF_SOFT , 0 );

    if (check_PROOF_HARD->isChecked())
        oySetBehaviour( oyBEHAVIOUR_PROOF_HARD , 1 );
    else 
        oySetBehaviour(oyBEHAVIOUR_PROOF_HARD  , 0 );
}

// Create a new file that's currently stored in the customProfileDirectory QString.
void kmsettings::saveCustomXmlFile()
{ 
    saveSettings();
    std::string t = selected_policy.toStdString();
    oyPolicySaveActual(oyGROUP_ALL, t.c_str()); 
}

// Used to enable the "Apply" button.
void kmsettings::emitChanged()
{
     // Ungrey "Apply" button. 
     settingsChanged = true;
     emit changed(true);     

     saveSettings();
     refreshPolicySettings();
}

// Function to save/refresh installed policies in system.
void kmsettings::savePolicy()
{
     QString tempProfile;
     QStringList policyList;

     if ( policySettingsList->count() >= internalPolicyCount)
     {
         QListWidgetItem * temp_item;

         for (int i = internalPolicyCount; i < policySettingsList->count(); i++)
         {
              temp_item = policySettingsList->item(i);
              tempProfile = temp_item->text();

              policyList.insert(0, tempProfile);
         }
     }
}

// Function to load the installed policies in system.
void kmsettings::loadPolicy()
{
  const char ** names = NULL;
  int count = 0, i, current = -1;

  policySettingsList->clear();

  // NOTE Built-in policies are loaded first.
  oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );
  for(i = 0; i < count; ++i)
    policySettingsList->addItem( names[i] );
}

kmsettings::~kmsettings()
{
     
}

