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

#ifndef KMSETTINGS_H
#define KMSETTINGS_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>

#include <oyranos.h>

class QPushButton;
class QListWidget;
class QListWidgetItem;

#include "ui_kmsettings.h"     // Gui header.


/*****************************************************************
          Kolor Management Settings Class Definition             *
******************************************************************/

class kmsettings : public KCModule, Ui::kmsettings
{        
    Q_OBJECT

public:    
    kmsettings(QWidget *parent, const QVariantList &);
    ~kmsettings();

    // QString to const char * conversion.
    const char* qStringToChar(QString);

public Q_SLOTS:
     
    // load the settings from the config
    virtual void load();

    // save the current settings
    virtual void save();

// User-defined QT slots.
private slots:
    
    // Refreshes which policy is currently highlighted/selected.
    void selectPolicy(QListWidgetItem*);  

    // Function used when user clicks on "Add Custom Policy".
    void addNewPolicy();

    // Function to remove custom policy.
    void removeCustomPolicy();
 
    // Write changes to the custom Xml File.
    void saveCustomXmlFile();

    // Enables "Apply" button.
    void emitChanged();

    // Button is called to save custom default profile into XML file.
    void saveSettingsToXml();

private:
 
    // Used to fill in "Default Profiles" tab.
    void populateProfiles();

    // Used to fill in "Behavior Settings" tab.
    void populateBehaviorSettings();
   
    // Refresh profile list with changed policy settings.
    void refreshProfileSettings();    

    // Refresh policy displaying
    void refreshPolicySettings();

    // Function that handles read-only instances.
    void setEditableItems(bool);

    // Function to 'create' a new xml file.
    void writeNewXmlFile(QString policyname, QString filename);
  
    // Filter default profiles. 
    void fillProfileComboBoxes(oyPROFILE_e, QComboBox *);

    // Save policies 
    void savePolicy();

    // Load policies
    void loadPolicy();

    // Set up all available widgets that are editable into lists.
    void loadEditableItems();  

    // Store the settings in the Oyranos database
    void saveSettings();

    // Holds all available policy names on the list.
    QStringList installedCustomPolicies;

    // For items that are editable by the user, having them stored
    // in a list allows us to easily detect if "one" item has been changed.
    QList<QComboBox*> editableComboItems;
    QList<QCheckBox*> editableCheckBoxItems;

    // Strings to store policy names.
    QString selected_policy;
    QString default_policy;  

    // String to hold new custom XML filename.
    QString xmlFileName;
   
    // String used to store the path of a recently installed custom policy.
    QString customProfileDirectory;

    // Variable to specify if current policy is a custom one.
    bool isCustom, settingsChanged;

};

#endif 
