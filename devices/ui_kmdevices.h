/********************************************************************************
** Form generated from reading ui file 'kmdevices.ui'
**
** Created: Sun Aug 17 22:44:38 2008
**      by: Qt User Interface Compiler version 4.4.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KMDEVICES_H
#define UI_KMDEVICES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_kmdevices
{
public:
    QGridLayout *gridLayout;
    QGroupBox *deviceGroupBox;
    QGridLayout *gridLayout_3;
    QTreeWidget *deviceList;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QFrame *line;
    QListWidget *profileAssociationList;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QComboBox *deviceProfileComboBox;
    QPushButton *addProfileButton;
    QPushButton *removeButton;
    QPushButton *defaultProfileButton;

    void setupUi(QWidget *kmdevices)
    {
    if (kmdevices->objectName().isEmpty())
        kmdevices->setObjectName(QString::fromUtf8("kmdevices"));
    kmdevices->resize(797, 647);
    gridLayout = new QGridLayout(kmdevices);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    deviceGroupBox = new QGroupBox(kmdevices);
    deviceGroupBox->setObjectName(QString::fromUtf8("deviceGroupBox"));
    gridLayout_3 = new QGridLayout(deviceGroupBox);
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    deviceList = new QTreeWidget(deviceGroupBox);
    deviceList->setObjectName(QString::fromUtf8("deviceList"));
    deviceList->setIconSize(QSize(32, 32));
    deviceList->setUniformRowHeights(false);
    deviceList->setAllColumnsShowFocus(false);
    deviceList->setHeaderHidden(false);

    gridLayout_3->addWidget(deviceList, 1, 0, 1, 1);


    gridLayout->addWidget(deviceGroupBox, 0, 1, 1, 1);

    verticalSpacer_2 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Minimum);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);

    groupBox_2 = new QGroupBox(kmdevices);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_2 = new QGridLayout(groupBox_2);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    line = new QFrame(groupBox_2);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    gridLayout_2->addWidget(line, 6, 0, 1, 1);

    profileAssociationList = new QListWidget(groupBox_2);
    profileAssociationList->setObjectName(QString::fromUtf8("profileAssociationList"));

    gridLayout_2->addWidget(profileAssociationList, 0, 0, 1, 1);

    groupBox = new QGroupBox(groupBox_2);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    formLayout = new QFormLayout(groupBox);
    formLayout->setObjectName(QString::fromUtf8("formLayout"));
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    deviceProfileComboBox = new QComboBox(groupBox);
    deviceProfileComboBox->setObjectName(QString::fromUtf8("deviceProfileComboBox"));

    formLayout->setWidget(1, QFormLayout::FieldRole, deviceProfileComboBox);

    addProfileButton = new QPushButton(groupBox);
    addProfileButton->setObjectName(QString::fromUtf8("addProfileButton"));
    addProfileButton->setEnabled(true);
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8("../../../../../../../usr/share/icons/crystalsvg/16x16/actions/list-add.png")), QIcon::Normal, QIcon::On);
    addProfileButton->setIcon(icon);

    formLayout->setWidget(1, QFormLayout::LabelRole, addProfileButton);


    gridLayout_2->addWidget(groupBox, 5, 0, 1, 1);

    removeButton = new QPushButton(groupBox_2);
    removeButton->setObjectName(QString::fromUtf8("removeButton"));
    QIcon icon1;
    icon1.addPixmap(QPixmap(QString::fromUtf8("../../../../../../../usr/share/icons/crystalsvg/16x16/actions/list-remove.png")), QIcon::Normal, QIcon::On);
    removeButton->setIcon(icon1);

    gridLayout_2->addWidget(removeButton, 2, 0, 1, 1);

    defaultProfileButton = new QPushButton(groupBox_2);
    defaultProfileButton->setObjectName(QString::fromUtf8("defaultProfileButton"));

    gridLayout_2->addWidget(defaultProfileButton, 1, 0, 1, 1);


    gridLayout->addWidget(groupBox_2, 3, 1, 2, 1);

    deviceGroupBox->raise();
    groupBox_2->raise();

    retranslateUi(kmdevices);

    QMetaObject::connectSlotsByName(kmdevices);
    } // setupUi

    void retranslateUi(QWidget *kmdevices)
    {
    kmdevices->setWindowTitle(QApplication::translate("kmdevices", "Form", 0, QApplication::UnicodeUTF8));
    deviceGroupBox->setTitle(QApplication::translate("kmdevices", "Detected Devices", 0, QApplication::UnicodeUTF8));
    deviceList->headerItem()->setText(0, QApplication::translate("kmdevices", "Device List", 0, QApplication::UnicodeUTF8));
    deviceList->headerItem()->setText(1, QApplication::translate("kmdevices", "Profile Description", 0, QApplication::UnicodeUTF8));
    deviceList->headerItem()->setText(2, QApplication::translate("kmdevices", "Profile Filename", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("kmdevices", "Profiles Associated with Device", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("kmdevices", "Available DeviceProfiles", 0, QApplication::UnicodeUTF8));
    addProfileButton->setText(QApplication::translate("kmdevices", "Add Profile", 0, QApplication::UnicodeUTF8));
    removeButton->setText(QApplication::translate("kmdevices", "Remove Profile", 0, QApplication::UnicodeUTF8));
    defaultProfileButton->setText(QApplication::translate("kmdevices", "Set Profile as Device Default", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(kmdevices);
    } // retranslateUi

};

namespace Ui {
    class kmdevices: public Ui_kmdevices {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KMDEVICES_H
