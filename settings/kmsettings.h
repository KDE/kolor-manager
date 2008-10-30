#ifndef KMSETTINGS_H
#define KMSETTINGS_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>

#include <oyranos/oyranos.h>

class QPushButton;
class QListWidget;
class QListWidgetItem;

#include "ui_kmsettings.h"     // Gui header.

using namespace oyranos;

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

    // Convert policy name to Oyranos-readable XML file pathname. 
    const char* policyNameToXmlPath(QString);

    // Convert policy name to Oyranos-readable XML name. 
    const char* policyNameToXmlFile(QString);

public Q_SLOTS:
     
    // load the settings from the config
    virtual void load();

    // save the current settings
    virtual void save();

// User-defined QT slots.
private slots:
    
    // Refreshes which policy is currently highlighted/selected.
    void selectPolicy(QListWidgetItem*);  

    // Function used when user
    void setPolicy();
    
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
    
    // Load XDG_CONFIG_HOME directory.
    void setupDirectory();
    
    // Set up all available widgets that are editable into lists.
    void loadEditableItems();  

    // Holds all available policy names on the list.
    QStringList installedCustomPolicies;

    // For items that are editable by the user, having them stored
    // in a list allows us to easily detect if "one" item has been changed.
    QList<QComboBox*> editableComboItems;
    QList<QRadioButton*> editableCheckBoxItems;
       
    // Strings to store policy names.
    QString selected_policy;
    QString default_policy;  

    // Strings to locate standard policy files.
    QString office_policy_path;
    QString designer_policy_path;
    QString printer_policy_path;
    QString photographer_policy_path;

    // String to hold new custom XML filename.
    QString xmlFileName;
   
    // String used to store the path of a recently installed custom policy.
    QString customProfileDirectory;

    // Variable to specify if current policy is a custom one.
    bool isCustom, settingsChanged;

    // KConfig pointer (save various settings within KDE)
    KSharedConfigPtr m_config;
};

#endif 
