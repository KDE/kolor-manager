#ifndef KMINFO_H
#define KMINFO_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>

#include <oyranos/oyranos_alpha.h>

class QPushButton;
class QListWidget;
class QListWidgetItem;

#include "ui_kminfo.h"     // Gui header.

using namespace oyranos;

/******************************************************************
          KM Information Class Definition                         *
*******************************************************************/


// "kminfo" - Class to view profile information.
class kminfo : public KCModule, Ui::kminfo
{        
    Q_OBJECT

public:    
    kminfo(QWidget *parent, const QVariantList &);
    ~kminfo();   

QString iccExaminCommand;
public Q_SLOTS:

    // load the settings from the config
    virtual void load();

    // save the current settings
    virtual void save();


// User-defined QT slots.
private slots:
     
     // Function to change profile information description
     void changeProfileTreeItem(QTreeWidgetItem*);
  
     // If "Analyze profile" button is un-hidden, launch iccexamin.
     void launchICCExamin();

private:
     
     // Build profile listing tree.
     void populateInstalledProfileList();

     // Add an item to the tree.
     void addProfileTreeItem( oyPROFILE_e, QString description, QTreeWidgetItem * parent_item );
    
     // Populate "Profile Information" box (using Oyranos profile information)
     void populateTagDescriptions(oyDEFAULT_PROFILE);

     // Populate tree list items relating to device-specific profiles (printers, monitors, etc.)
     void populateDeviceProfiles( QStringList listOfDevices, QTreeWidgetItem * deviceListSubTree, QIcon device_icon);

     // Populate tag descriptions for device-specific profiles.
     void populateDeviceProfileDescriptions(oyProfile_s * profile);
 
     // Function to write tag descriptions to individual labels
     void setTagDescriptions(oyProfile_s *, icTagSignature, QLabel *);
 
     // The following provide additonal tags to be displayed.
     void setPcsTag(oyProfile_s * profile, QLabel * pcsLabel);
     void setCSpaceTag(oyProfile_s * profile, QLabel * cSpaceLabel);
     void setIccsTag(oyProfile_s * profile, QLabel * iccsLabel);
     void setDeviceClassTag(oyProfile_s * profile, QLabel * devClassLabel);

     // Function to write date tag to a label.
     void setDateTag(oyProfile_s *, QLabel *);
    
     // Check for iccexamin functionality on user system.
     bool iccExaminIsInstalled(QString &iccExaminPath);

     
     // Pointers to current QTreeWidget parents (Devices, Editing Space, Assumed Space)
     QTreeWidgetItem * assumedCsTree;
     QTreeWidgetItem * editingCsTree;
     QTreeWidgetItem * devicesParentTree;

     // Pointers to device-specific (parent) items on list
     QTreeWidgetItem * monitorListSubTree;
     QTreeWidgetItem * printerListSubTree;
     QTreeWidgetItem * scannerListSubTree;
     QTreeWidgetItem * cameraListSubTree;

     KSharedConfigPtr m_config;
 
};

#endif 
