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

#ifndef KMINFO_H
#define KMINFO_H

#include <QtGui/QDialogButtonBox>
#include <KCModule>
#include <KColorScheme>
#include <KPushButton>

#include <oyranos_alpha.h>

class QPushButton;
class QListWidget;
class QListWidgetItem;

#include "ui_kminfo.h"     // Gui header.

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
    
     // Populate tree list items relating to device-specific profiles (printers, monitors, etc.)
     void populateDeviceProfiles( /*QStringList listOfDevices,*/ QTreeWidgetItem * deviceListSubTree /*, QIcon device_icon*/);

     // Populate tag descriptions for device-specific profiles.
     void populateDeviceProfileDescriptions(oyProfile_s * profile, bool valid);
 
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
     oyProfile_s * current_profile;

     KSharedConfigPtr m_config;
 
};

#endif 
