/********************************************************************************
** Form generated from reading ui file 'kminfo.ui'
**
** Created: Mon Aug 18 06:53:10 2008
**      by: Qt User Interface Compiler version 4.4.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KMINFO_H
#define UI_KMINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_kminfo
{
public:
    QGridLayout *gridLayout_3;
    QTreeWidget *installedProfilesTree;
    QGroupBox *profileInfoGroupBox;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *descriptionTagLabel;
    QFrame *line_2;
    QLabel *copyrightTagLabel;
    QSpacerItem *verticalSpacer;
    QPushButton *launchICCExaminButton;
    QGridLayout *gridLayout;
    QLabel *filenameLabel;
    QLabel *filenameTagLabel;
    QLabel *dateTextLabel;
    QLabel *dateTagLabel;
    QLabel *dcTextLabel;
    QLabel *deviceClassTagLabel;
    QLabel *manufacturerLabel;
    QLabel *mfgTagLabel;
    QLabel *modelTexLabel;
    QLabel *modelTagLabel;
    QLabel *iccVersionLabel;
    QLabel *iccVerTagLabel;
    QLabel *colorspaceLabel;
    QLabel *colorspaceTagLabel;
    QLabel *pcsTypeLabel;
    QLabel *pcsTagLabel;
    QLabel *listingHeaderLabel;
    QLabel *directoryListingTag;

    void setupUi(QWidget *kminfo)
    {
    if (kminfo->objectName().isEmpty())
        kminfo->setObjectName(QString::fromUtf8("kminfo"));
    kminfo->resize(1126, 926);
    gridLayout_3 = new QGridLayout(kminfo);
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    installedProfilesTree = new QTreeWidget(kminfo);
    QTreeWidgetItem *__treeItem = new QTreeWidgetItem(installedProfilesTree);
    new QTreeWidgetItem(__treeItem);
    new QTreeWidgetItem(__treeItem);
    new QTreeWidgetItem(__treeItem);
    new QTreeWidgetItem(__treeItem);

    new QTreeWidgetItem(installedProfilesTree);
    new QTreeWidgetItem(installedProfilesTree);

    installedProfilesTree->setObjectName(QString::fromUtf8("installedProfilesTree"));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(installedProfilesTree->sizePolicy().hasHeightForWidth());
    installedProfilesTree->setSizePolicy(sizePolicy);
    installedProfilesTree->setAlternatingRowColors(false);
    installedProfilesTree->setSelectionMode(QAbstractItemView::SingleSelection);
    installedProfilesTree->setUniformRowHeights(false);
    installedProfilesTree->setAllColumnsShowFocus(false);

    gridLayout_3->addWidget(installedProfilesTree, 0, 0, 1, 1);

    profileInfoGroupBox = new QGroupBox(kminfo);
    profileInfoGroupBox->setObjectName(QString::fromUtf8("profileInfoGroupBox"));
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(profileInfoGroupBox->sizePolicy().hasHeightForWidth());
    profileInfoGroupBox->setSizePolicy(sizePolicy1);
    gridLayout_2 = new QGridLayout(profileInfoGroupBox);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    verticalLayout = new QVBoxLayout();
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
    verticalLayout->setContentsMargins(11, -1, -1, 9);
    descriptionTagLabel = new QLabel(profileInfoGroupBox);
    descriptionTagLabel->setObjectName(QString::fromUtf8("descriptionTagLabel"));
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(descriptionTagLabel->sizePolicy().hasHeightForWidth());
    descriptionTagLabel->setSizePolicy(sizePolicy2);
    QFont font;
    font.setPointSize(13);
    font.setBold(true);
    font.setWeight(75);
    descriptionTagLabel->setFont(font);
    descriptionTagLabel->setAlignment(Qt::AlignCenter);
    descriptionTagLabel->setWordWrap(true);

    verticalLayout->addWidget(descriptionTagLabel);

    line_2 = new QFrame(profileInfoGroupBox);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(line_2);

    copyrightTagLabel = new QLabel(profileInfoGroupBox);
    copyrightTagLabel->setObjectName(QString::fromUtf8("copyrightTagLabel"));
    sizePolicy2.setHeightForWidth(copyrightTagLabel->sizePolicy().hasHeightForWidth());
    copyrightTagLabel->setSizePolicy(sizePolicy2);
    copyrightTagLabel->setFrameShadow(QFrame::Plain);
    copyrightTagLabel->setLineWidth(1);
    copyrightTagLabel->setScaledContents(true);
    copyrightTagLabel->setAlignment(Qt::AlignCenter);
    copyrightTagLabel->setWordWrap(true);
    copyrightTagLabel->setMargin(0);

    verticalLayout->addWidget(copyrightTagLabel);

    verticalSpacer = new QSpacerItem(368, 16, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout->addItem(verticalSpacer);

    launchICCExaminButton = new QPushButton(profileInfoGroupBox);
    launchICCExaminButton->setObjectName(QString::fromUtf8("launchICCExaminButton"));

    verticalLayout->addWidget(launchICCExaminButton);

    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    filenameLabel = new QLabel(profileInfoGroupBox);
    filenameLabel->setObjectName(QString::fromUtf8("filenameLabel"));
    QFont font1;
    font1.setBold(true);
    font1.setItalic(false);
    font1.setWeight(75);
    filenameLabel->setFont(font1);
    filenameLabel->setIndent(-5);

    gridLayout->addWidget(filenameLabel, 0, 0, 1, 1);

    filenameTagLabel = new QLabel(profileInfoGroupBox);
    filenameTagLabel->setObjectName(QString::fromUtf8("filenameTagLabel"));
    QFont font2;
    font2.setItalic(false);
    filenameTagLabel->setFont(font2);

    gridLayout->addWidget(filenameTagLabel, 0, 1, 1, 1);

    dateTextLabel = new QLabel(profileInfoGroupBox);
    dateTextLabel->setObjectName(QString::fromUtf8("dateTextLabel"));
    QFont font3;
    font3.setBold(true);
    font3.setItalic(false);
    font3.setUnderline(false);
    font3.setWeight(75);
    dateTextLabel->setFont(font3);

    gridLayout->addWidget(dateTextLabel, 1, 0, 1, 1);

    dateTagLabel = new QLabel(profileInfoGroupBox);
    dateTagLabel->setObjectName(QString::fromUtf8("dateTagLabel"));

    gridLayout->addWidget(dateTagLabel, 1, 1, 1, 1);

    dcTextLabel = new QLabel(profileInfoGroupBox);
    dcTextLabel->setObjectName(QString::fromUtf8("dcTextLabel"));
    QFont font4;
    font4.setBold(true);
    font4.setWeight(75);
    dcTextLabel->setFont(font4);

    gridLayout->addWidget(dcTextLabel, 2, 0, 1, 1);

    deviceClassTagLabel = new QLabel(profileInfoGroupBox);
    deviceClassTagLabel->setObjectName(QString::fromUtf8("deviceClassTagLabel"));

    gridLayout->addWidget(deviceClassTagLabel, 2, 1, 1, 1);

    manufacturerLabel = new QLabel(profileInfoGroupBox);
    manufacturerLabel->setObjectName(QString::fromUtf8("manufacturerLabel"));
    manufacturerLabel->setFont(font4);

    gridLayout->addWidget(manufacturerLabel, 3, 0, 1, 1);

    mfgTagLabel = new QLabel(profileInfoGroupBox);
    mfgTagLabel->setObjectName(QString::fromUtf8("mfgTagLabel"));

    gridLayout->addWidget(mfgTagLabel, 3, 1, 1, 1);

    modelTexLabel = new QLabel(profileInfoGroupBox);
    modelTexLabel->setObjectName(QString::fromUtf8("modelTexLabel"));
    modelTexLabel->setFont(font4);

    gridLayout->addWidget(modelTexLabel, 4, 0, 1, 1);

    modelTagLabel = new QLabel(profileInfoGroupBox);
    modelTagLabel->setObjectName(QString::fromUtf8("modelTagLabel"));

    gridLayout->addWidget(modelTagLabel, 4, 1, 1, 1);

    iccVersionLabel = new QLabel(profileInfoGroupBox);
    iccVersionLabel->setObjectName(QString::fromUtf8("iccVersionLabel"));
    iccVersionLabel->setFont(font4);

    gridLayout->addWidget(iccVersionLabel, 5, 0, 1, 1);

    iccVerTagLabel = new QLabel(profileInfoGroupBox);
    iccVerTagLabel->setObjectName(QString::fromUtf8("iccVerTagLabel"));

    gridLayout->addWidget(iccVerTagLabel, 5, 1, 1, 1);

    colorspaceLabel = new QLabel(profileInfoGroupBox);
    colorspaceLabel->setObjectName(QString::fromUtf8("colorspaceLabel"));
    colorspaceLabel->setFont(font4);

    gridLayout->addWidget(colorspaceLabel, 6, 0, 1, 1);

    colorspaceTagLabel = new QLabel(profileInfoGroupBox);
    colorspaceTagLabel->setObjectName(QString::fromUtf8("colorspaceTagLabel"));

    gridLayout->addWidget(colorspaceTagLabel, 6, 1, 1, 1);

    pcsTypeLabel = new QLabel(profileInfoGroupBox);
    pcsTypeLabel->setObjectName(QString::fromUtf8("pcsTypeLabel"));
    pcsTypeLabel->setFont(font4);

    gridLayout->addWidget(pcsTypeLabel, 7, 0, 1, 1);

    pcsTagLabel = new QLabel(profileInfoGroupBox);
    pcsTagLabel->setObjectName(QString::fromUtf8("pcsTagLabel"));

    gridLayout->addWidget(pcsTagLabel, 7, 1, 1, 1);

    listingHeaderLabel = new QLabel(profileInfoGroupBox);
    listingHeaderLabel->setObjectName(QString::fromUtf8("listingHeaderLabel"));
    QFont font5;
    font5.setBold(true);
    font5.setUnderline(false);
    font5.setWeight(75);
    listingHeaderLabel->setFont(font5);

    gridLayout->addWidget(listingHeaderLabel, 8, 0, 1, 1);

    directoryListingTag = new QLabel(profileInfoGroupBox);
    directoryListingTag->setObjectName(QString::fromUtf8("directoryListingTag"));
    QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(directoryListingTag->sizePolicy().hasHeightForWidth());
    directoryListingTag->setSizePolicy(sizePolicy3);
    QFont font6;
    font6.setBold(false);
    font6.setItalic(false);
    font6.setWeight(50);
    directoryListingTag->setFont(font6);
    directoryListingTag->setTextFormat(Qt::AutoText);
    directoryListingTag->setScaledContents(true);
    directoryListingTag->setWordWrap(true);

    gridLayout->addWidget(directoryListingTag, 8, 1, 1, 1);


    verticalLayout->addLayout(gridLayout);


    gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);


    gridLayout_3->addWidget(profileInfoGroupBox, 0, 1, 1, 1);


    retranslateUi(kminfo);

    QMetaObject::connectSlotsByName(kminfo);
    } // setupUi

    void retranslateUi(QWidget *kminfo)
    {
    kminfo->setWindowTitle(QApplication::translate("kminfo", "Form", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->headerItem()->setText(0, QApplication::translate("kminfo", "Installed Profiles", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->headerItem()->setText(1, QApplication::translate("kminfo", "Description", 0, QApplication::UnicodeUTF8));

    const bool __sortingEnabled = installedProfilesTree->isSortingEnabled();
    installedProfilesTree->setSortingEnabled(false);
    installedProfilesTree->topLevelItem(0)->setText(0, QApplication::translate("kminfo", "Devices", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(0)->child(0)->setText(0, QApplication::translate("kminfo", "Monitors", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(0)->child(1)->setText(0, QApplication::translate("kminfo", "Printers", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(0)->child(2)->setText(0, QApplication::translate("kminfo", "Scanners", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(0)->child(3)->setText(0, QApplication::translate("kminfo", "Digital Camera", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(1)->setText(0, QApplication::translate("kminfo", "Editing Space", 0, QApplication::UnicodeUTF8));
    installedProfilesTree->topLevelItem(2)->setText(0, QApplication::translate("kminfo", "Assumed Space", 0, QApplication::UnicodeUTF8));

    installedProfilesTree->setSortingEnabled(__sortingEnabled);
    profileInfoGroupBox->setTitle(QApplication::translate("kminfo", "Profile Information", 0, QApplication::UnicodeUTF8));
    descriptionTagLabel->setText(QApplication::translate("kminfo", "ICCDescriptionLabel", 0, QApplication::UnicodeUTF8));
    copyrightTagLabel->setText(QApplication::translate("kminfo", "(Copyright not available)", 0, QApplication::UnicodeUTF8));
    launchICCExaminButton->setText(QApplication::translate("kminfo", "Analyze Profile Data", 0, QApplication::UnicodeUTF8));
    filenameLabel->setText(QApplication::translate("kminfo", "File Name:", 0, QApplication::UnicodeUTF8));
    filenameTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    dateTextLabel->setText(QApplication::translate("kminfo", "Date Created:", 0, QApplication::UnicodeUTF8));
    dateTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    dcTextLabel->setText(QApplication::translate("kminfo", "Device Class", 0, QApplication::UnicodeUTF8));
    deviceClassTagLabel->setText(QApplication::translate("kminfo", "\"FIXME\"", 0, QApplication::UnicodeUTF8));
    manufacturerLabel->setText(QApplication::translate("kminfo", "Manufacturer:", 0, QApplication::UnicodeUTF8));
    mfgTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    modelTexLabel->setText(QApplication::translate("kminfo", "Model:", 0, QApplication::UnicodeUTF8));
    modelTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    iccVersionLabel->setText(QApplication::translate("kminfo", "ICC Version:", 0, QApplication::UnicodeUTF8));
    iccVerTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    colorspaceLabel->setText(QApplication::translate("kminfo", "Color Space:", 0, QApplication::UnicodeUTF8));
    colorspaceTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    pcsTypeLabel->setText(QApplication::translate("kminfo", "PCS Type:", 0, QApplication::UnicodeUTF8));
    pcsTagLabel->setText(QApplication::translate("kminfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    listingHeaderLabel->setText(QApplication::translate("kminfo", "Profile Pathname:", 0, QApplication::UnicodeUTF8));
    directoryListingTag->setText(QApplication::translate("kminfo", "Directory Listng", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(kminfo);
    } // retranslateUi

};

namespace Ui {
    class kminfo: public Ui_kminfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KMINFO_H
