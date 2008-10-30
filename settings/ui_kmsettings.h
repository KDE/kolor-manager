/********************************************************************************
** Form generated from reading ui file 'kmsettings.ui'
**
** Created: Thu Aug 7 04:33:49 2008
**      by: Qt User Interface Compiler version 4.4.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KMSETTINGS_H
#define UI_KMSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_kmsettings
{
public:
    QGridLayout *gridLayout_7;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QSplitter *splitter;
    QTabWidget *kmsettingsTab;
    QWidget *policySettingsTab;
    QGridLayout *gridLayout_6;
    QGroupBox *currentPolicyBox;
    QGridLayout *gridLayout_5;
    QLabel *currentPolicyLabel;
    QGroupBox *policySetttingsBox;
    QGridLayout *gridLayout_8;
    QListWidget *policySettingsList;
    QPushButton *setPolicyButton;
    QLabel *policyLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *removePolicyButton;
    QPushButton *addNewPolicyButton;
    QFrame *line_2;
    QSpacerItem *verticalSpacer_2;
    QWidget *defaultProfilesTab;
    QGridLayout *gridLayout_12;
    QGroupBox *defaultProfilesBox;
    QGridLayout *gridLayout_11;
    QLabel *label_2;
    QComboBox *editRgbCombo;
    QLabel *label_3;
    QComboBox *editCmykCombo;
    QLabel *label_5;
    QComboBox *editXyzCombo;
    QLabel *label_6;
    QComboBox *editLabCombo;
    QComboBox *editGrayCombo;
    QLabel *label_9;
    QLabel *label;
    QSpacerItem *verticalSpacer_4;
    QGroupBox *assumedSourceGBox;
    QGridLayout *gridLayout_17;
    QLabel *label_7;
    QComboBox *assumeRgbCombo;
    QLabel *label_8;
    QComboBox *assumeCmykCombo;
    QLabel *label_10;
    QComboBox *assumeXyzCombo;
    QLabel *label_11;
    QComboBox *assumeLabCombo;
    QLabel *webSourceLabel;
    QComboBox *editRgbCombo_7;
    QLabel *assumedGrayLabel;
    QComboBox *assumeGrayCombo;
    QFrame *line;
    QWidget *tab;
    QGridLayout *gridLayout_10;
    QGridLayout *gridLayout_9;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_14;
    QGridLayout *gridLayout;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QComboBox *ifNoProfileCombo;
    QComboBox *ifRgbMismatchCombo;
    QComboBox *ifCmykMismatchCombo;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_16;
    QGridLayout *gridLayout_4;
    QLabel *label_17;
    QComboBox *monitorMixedCombo;
    QLabel *label_18;
    QComboBox *printerMixedCombo;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_15;
    QGridLayout *gridLayout_3;
    QLabel *label_15;
    QGridLayout *gridLayout_2;
    QComboBox *proofColorProfileCombo;
    QComboBox *proofRIntentCombo;
    QRadioButton *hardProofCheckbox;
    QRadioButton *softProofCheckbox;
    QLabel *label_16;
    QGroupBox *renderingSettingsBox;
    QGridLayout *gridLayout_13;
    QHBoxLayout *horizontalLayout;
    QComboBox *defaultRIntentCombo;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *kmsettings)
    {
    if (kmsettings->objectName().isEmpty())
        kmsettings->setObjectName(QString::fromUtf8("kmsettings"));
    kmsettings->resize(823, 619);
    gridLayout_7 = new QGridLayout(kmsettings);
    gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
    horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

    gridLayout_7->addItem(horizontalSpacer, 1, 2, 1, 1);

    verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout_7->addItem(verticalSpacer, 2, 1, 1, 1);

    splitter = new QSplitter(kmsettings);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);

    gridLayout_7->addWidget(splitter, 0, 1, 1, 1);

    kmsettingsTab = new QTabWidget(kmsettings);
    kmsettingsTab->setObjectName(QString::fromUtf8("kmsettingsTab"));
    policySettingsTab = new QWidget();
    policySettingsTab->setObjectName(QString::fromUtf8("policySettingsTab"));
    gridLayout_6 = new QGridLayout(policySettingsTab);
    gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
    currentPolicyBox = new QGroupBox(policySettingsTab);
    currentPolicyBox->setObjectName(QString::fromUtf8("currentPolicyBox"));
    currentPolicyBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    gridLayout_5 = new QGridLayout(currentPolicyBox);
    gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
    currentPolicyLabel = new QLabel(currentPolicyBox);
    currentPolicyLabel->setObjectName(QString::fromUtf8("currentPolicyLabel"));
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    currentPolicyLabel->setFont(font);
    currentPolicyLabel->setAlignment(Qt::AlignCenter);

    gridLayout_5->addWidget(currentPolicyLabel, 1, 0, 1, 1);


    gridLayout_6->addWidget(currentPolicyBox, 1, 0, 1, 1);

    policySetttingsBox = new QGroupBox(policySettingsTab);
    policySetttingsBox->setObjectName(QString::fromUtf8("policySetttingsBox"));
    policySetttingsBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    gridLayout_8 = new QGridLayout(policySetttingsBox);
    gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
    policySettingsList = new QListWidget(policySetttingsBox);
    new QListWidgetItem(policySettingsList);
    new QListWidgetItem(policySettingsList);
    new QListWidgetItem(policySettingsList);
    new QListWidgetItem(policySettingsList);
    policySettingsList->setObjectName(QString::fromUtf8("policySettingsList"));
    QFont font1;
    font1.setBold(false);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(50);
    policySettingsList->setFont(font1);
    policySettingsList->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    policySettingsList->setAlternatingRowColors(false);
    policySettingsList->setIconSize(QSize(16, 16));
    policySettingsList->setSortingEnabled(false);

    gridLayout_8->addWidget(policySettingsList, 1, 1, 1, 1);

    setPolicyButton = new QPushButton(policySetttingsBox);
    setPolicyButton->setObjectName(QString::fromUtf8("setPolicyButton"));

    gridLayout_8->addWidget(setPolicyButton, 2, 1, 1, 1);

    policyLabel = new QLabel(policySetttingsBox);
    policyLabel->setObjectName(QString::fromUtf8("policyLabel"));

    gridLayout_8->addWidget(policyLabel, 0, 1, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    gridLayout_8->addItem(horizontalSpacer_2, 1, 2, 1, 1);

    removePolicyButton = new QPushButton(policySetttingsBox);
    removePolicyButton->setObjectName(QString::fromUtf8("removePolicyButton"));

    gridLayout_8->addWidget(removePolicyButton, 5, 1, 1, 1);

    addNewPolicyButton = new QPushButton(policySetttingsBox);
    addNewPolicyButton->setObjectName(QString::fromUtf8("addNewPolicyButton"));

    gridLayout_8->addWidget(addNewPolicyButton, 4, 1, 1, 1);

    line_2 = new QFrame(policySetttingsBox);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    gridLayout_8->addWidget(line_2, 3, 1, 1, 1);


    gridLayout_6->addWidget(policySetttingsBox, 3, 0, 1, 1);

    verticalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout_6->addItem(verticalSpacer_2, 5, 0, 1, 1);

    kmsettingsTab->addTab(policySettingsTab, QString());
    defaultProfilesTab = new QWidget();
    defaultProfilesTab->setObjectName(QString::fromUtf8("defaultProfilesTab"));
    gridLayout_12 = new QGridLayout(defaultProfilesTab);
    gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
    defaultProfilesBox = new QGroupBox(defaultProfilesTab);
    defaultProfilesBox->setObjectName(QString::fromUtf8("defaultProfilesBox"));
    gridLayout_11 = new QGridLayout(defaultProfilesBox);
    gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
    label_2 = new QLabel(defaultProfilesBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout_11->addWidget(label_2, 2, 0, 1, 1);

    editRgbCombo = new QComboBox(defaultProfilesBox);
    editRgbCombo->setObjectName(QString::fromUtf8("editRgbCombo"));
    editRgbCombo->setEnabled(true);

    gridLayout_11->addWidget(editRgbCombo, 2, 1, 1, 1);

    label_3 = new QLabel(defaultProfilesBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout_11->addWidget(label_3, 3, 0, 1, 1);

    editCmykCombo = new QComboBox(defaultProfilesBox);
    editCmykCombo->setObjectName(QString::fromUtf8("editCmykCombo"));
    editCmykCombo->setEnabled(true);

    gridLayout_11->addWidget(editCmykCombo, 3, 1, 1, 1);

    label_5 = new QLabel(defaultProfilesBox);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout_11->addWidget(label_5, 4, 0, 1, 1);

    editXyzCombo = new QComboBox(defaultProfilesBox);
    editXyzCombo->setObjectName(QString::fromUtf8("editXyzCombo"));
    editXyzCombo->setEnabled(true);

    gridLayout_11->addWidget(editXyzCombo, 4, 1, 1, 1);

    label_6 = new QLabel(defaultProfilesBox);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    gridLayout_11->addWidget(label_6, 5, 0, 1, 1);

    editLabCombo = new QComboBox(defaultProfilesBox);
    editLabCombo->setObjectName(QString::fromUtf8("editLabCombo"));
    editLabCombo->setEnabled(true);

    gridLayout_11->addWidget(editLabCombo, 5, 1, 1, 1);

    editGrayCombo = new QComboBox(defaultProfilesBox);
    editGrayCombo->setObjectName(QString::fromUtf8("editGrayCombo"));

    gridLayout_11->addWidget(editGrayCombo, 6, 1, 1, 1);

    label_9 = new QLabel(defaultProfilesBox);
    label_9->setObjectName(QString::fromUtf8("label_9"));

    gridLayout_11->addWidget(label_9, 6, 0, 1, 1);


    gridLayout_12->addWidget(defaultProfilesBox, 3, 0, 1, 1);

    label = new QLabel(defaultProfilesTab);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout_12->addWidget(label, 0, 0, 1, 1);

    verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout_12->addItem(verticalSpacer_4, 2, 0, 1, 1);

    assumedSourceGBox = new QGroupBox(defaultProfilesTab);
    assumedSourceGBox->setObjectName(QString::fromUtf8("assumedSourceGBox"));
    gridLayout_17 = new QGridLayout(assumedSourceGBox);
    gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
    label_7 = new QLabel(assumedSourceGBox);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    gridLayout_17->addWidget(label_7, 0, 0, 1, 1);

    assumeRgbCombo = new QComboBox(assumedSourceGBox);
    assumeRgbCombo->setObjectName(QString::fromUtf8("assumeRgbCombo"));
    assumeRgbCombo->setEnabled(true);

    gridLayout_17->addWidget(assumeRgbCombo, 0, 1, 1, 1);

    label_8 = new QLabel(assumedSourceGBox);
    label_8->setObjectName(QString::fromUtf8("label_8"));

    gridLayout_17->addWidget(label_8, 1, 0, 1, 1);

    assumeCmykCombo = new QComboBox(assumedSourceGBox);
    assumeCmykCombo->setObjectName(QString::fromUtf8("assumeCmykCombo"));
    assumeCmykCombo->setEnabled(true);

    gridLayout_17->addWidget(assumeCmykCombo, 1, 1, 1, 1);

    label_10 = new QLabel(assumedSourceGBox);
    label_10->setObjectName(QString::fromUtf8("label_10"));

    gridLayout_17->addWidget(label_10, 2, 0, 1, 1);

    assumeXyzCombo = new QComboBox(assumedSourceGBox);
    assumeXyzCombo->setObjectName(QString::fromUtf8("assumeXyzCombo"));
    assumeXyzCombo->setEnabled(true);

    gridLayout_17->addWidget(assumeXyzCombo, 2, 1, 1, 1);

    label_11 = new QLabel(assumedSourceGBox);
    label_11->setObjectName(QString::fromUtf8("label_11"));

    gridLayout_17->addWidget(label_11, 3, 0, 1, 1);

    assumeLabCombo = new QComboBox(assumedSourceGBox);
    assumeLabCombo->setObjectName(QString::fromUtf8("assumeLabCombo"));
    assumeLabCombo->setEnabled(true);

    gridLayout_17->addWidget(assumeLabCombo, 3, 1, 1, 1);

    webSourceLabel = new QLabel(assumedSourceGBox);
    webSourceLabel->setObjectName(QString::fromUtf8("webSourceLabel"));

    gridLayout_17->addWidget(webSourceLabel, 6, 0, 1, 1);

    editRgbCombo_7 = new QComboBox(assumedSourceGBox);
    editRgbCombo_7->setObjectName(QString::fromUtf8("editRgbCombo_7"));
    editRgbCombo_7->setEnabled(false);

    gridLayout_17->addWidget(editRgbCombo_7, 6, 1, 1, 1);

    assumedGrayLabel = new QLabel(assumedSourceGBox);
    assumedGrayLabel->setObjectName(QString::fromUtf8("assumedGrayLabel"));

    gridLayout_17->addWidget(assumedGrayLabel, 4, 0, 1, 1);

    assumeGrayCombo = new QComboBox(assumedSourceGBox);
    assumeGrayCombo->setObjectName(QString::fromUtf8("assumeGrayCombo"));

    gridLayout_17->addWidget(assumeGrayCombo, 4, 1, 1, 1);

    line = new QFrame(assumedSourceGBox);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    gridLayout_17->addWidget(line, 5, 0, 1, 2);


    gridLayout_12->addWidget(assumedSourceGBox, 4, 0, 1, 1);

    kmsettingsTab->addTab(defaultProfilesTab, QString());
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    gridLayout_10 = new QGridLayout(tab);
    gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
    gridLayout_9 = new QGridLayout();
    gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
    groupBox = new QGroupBox(tab);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout_14 = new QGridLayout(groupBox);
    gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_12 = new QLabel(groupBox);
    label_12->setObjectName(QString::fromUtf8("label_12"));

    gridLayout->addWidget(label_12, 0, 0, 1, 1);

    label_13 = new QLabel(groupBox);
    label_13->setObjectName(QString::fromUtf8("label_13"));

    gridLayout->addWidget(label_13, 0, 1, 1, 1);

    label_14 = new QLabel(groupBox);
    label_14->setObjectName(QString::fromUtf8("label_14"));

    gridLayout->addWidget(label_14, 0, 2, 1, 1);

    ifNoProfileCombo = new QComboBox(groupBox);
    ifNoProfileCombo->setObjectName(QString::fromUtf8("ifNoProfileCombo"));

    gridLayout->addWidget(ifNoProfileCombo, 1, 0, 1, 1);

    ifRgbMismatchCombo = new QComboBox(groupBox);
    ifRgbMismatchCombo->setObjectName(QString::fromUtf8("ifRgbMismatchCombo"));

    gridLayout->addWidget(ifRgbMismatchCombo, 1, 1, 1, 1);

    ifCmykMismatchCombo = new QComboBox(groupBox);
    ifCmykMismatchCombo->setObjectName(QString::fromUtf8("ifCmykMismatchCombo"));

    gridLayout->addWidget(ifCmykMismatchCombo, 1, 2, 1, 1);


    gridLayout_14->addLayout(gridLayout, 0, 0, 1, 1);


    gridLayout_9->addWidget(groupBox, 2, 0, 1, 1);

    groupBox_3 = new QGroupBox(tab);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    gridLayout_16 = new QGridLayout(groupBox_3);
    gridLayout_16->setObjectName(QString::fromUtf8("gridLayout_16"));
    gridLayout_4 = new QGridLayout();
    gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
    label_17 = new QLabel(groupBox_3);
    label_17->setObjectName(QString::fromUtf8("label_17"));
    label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout_4->addWidget(label_17, 0, 0, 1, 1);

    monitorMixedCombo = new QComboBox(groupBox_3);
    monitorMixedCombo->setObjectName(QString::fromUtf8("monitorMixedCombo"));

    gridLayout_4->addWidget(monitorMixedCombo, 0, 1, 1, 1);

    label_18 = new QLabel(groupBox_3);
    label_18->setObjectName(QString::fromUtf8("label_18"));
    label_18->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout_4->addWidget(label_18, 1, 0, 1, 1);

    printerMixedCombo = new QComboBox(groupBox_3);
    printerMixedCombo->setObjectName(QString::fromUtf8("printerMixedCombo"));

    gridLayout_4->addWidget(printerMixedCombo, 1, 1, 1, 1);


    gridLayout_16->addLayout(gridLayout_4, 0, 0, 1, 1);


    gridLayout_9->addWidget(groupBox_3, 4, 0, 1, 1);

    groupBox_2 = new QGroupBox(tab);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_15 = new QGridLayout(groupBox_2);
    gridLayout_15->setObjectName(QString::fromUtf8("gridLayout_15"));
    gridLayout_3 = new QGridLayout();
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    label_15 = new QLabel(groupBox_2);
    label_15->setObjectName(QString::fromUtf8("label_15"));

    gridLayout_3->addWidget(label_15, 0, 0, 1, 1);

    gridLayout_2 = new QGridLayout();
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    proofColorProfileCombo = new QComboBox(groupBox_2);
    proofColorProfileCombo->setObjectName(QString::fromUtf8("proofColorProfileCombo"));

    gridLayout_2->addWidget(proofColorProfileCombo, 0, 0, 1, 1);

    proofRIntentCombo = new QComboBox(groupBox_2);
    proofRIntentCombo->setObjectName(QString::fromUtf8("proofRIntentCombo"));

    gridLayout_2->addWidget(proofRIntentCombo, 0, 1, 1, 1);

    hardProofCheckbox = new QRadioButton(groupBox_2);
    hardProofCheckbox->setObjectName(QString::fromUtf8("hardProofCheckbox"));
    hardProofCheckbox->setAutoExclusive(true);

    gridLayout_2->addWidget(hardProofCheckbox, 1, 1, 1, 1);

    softProofCheckbox = new QRadioButton(groupBox_2);
    softProofCheckbox->setObjectName(QString::fromUtf8("softProofCheckbox"));
    softProofCheckbox->setAutoExclusive(true);

    gridLayout_2->addWidget(softProofCheckbox, 1, 0, 1, 1);


    gridLayout_3->addLayout(gridLayout_2, 1, 0, 1, 2);

    label_16 = new QLabel(groupBox_2);
    label_16->setObjectName(QString::fromUtf8("label_16"));

    gridLayout_3->addWidget(label_16, 0, 1, 1, 1);


    gridLayout_15->addLayout(gridLayout_3, 0, 0, 1, 1);


    gridLayout_9->addWidget(groupBox_2, 3, 0, 1, 1);

    renderingSettingsBox = new QGroupBox(tab);
    renderingSettingsBox->setObjectName(QString::fromUtf8("renderingSettingsBox"));
    gridLayout_13 = new QGridLayout(renderingSettingsBox);
    gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    defaultRIntentCombo = new QComboBox(renderingSettingsBox);
    defaultRIntentCombo->setObjectName(QString::fromUtf8("defaultRIntentCombo"));
    defaultRIntentCombo->setAcceptDrops(false);

    horizontalLayout->addWidget(defaultRIntentCombo);


    gridLayout_13->addLayout(horizontalLayout, 0, 0, 1, 1);


    gridLayout_9->addWidget(renderingSettingsBox, 0, 0, 1, 1);


    gridLayout_10->addLayout(gridLayout_9, 0, 0, 1, 1);

    verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout_10->addItem(verticalSpacer_3, 1, 0, 1, 1);

    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    gridLayout_10->addItem(horizontalSpacer_3, 0, 1, 1, 1);

    kmsettingsTab->addTab(tab, QString());

    gridLayout_7->addWidget(kmsettingsTab, 1, 1, 1, 1);


    retranslateUi(kmsettings);

    kmsettingsTab->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(kmsettings);
    } // setupUi

    void retranslateUi(QWidget *kmsettings)
    {
    kmsettings->setWindowTitle(QApplication::translate("kmsettings", "Form", 0, QApplication::UnicodeUTF8));
    currentPolicyBox->setTitle(QApplication::translate("kmsettings", "Current Policy", 0, QApplication::UnicodeUTF8));
    currentPolicyLabel->setText(QApplication::translate("kmsettings", "Home / Office", 0, QApplication::UnicodeUTF8));
    policySetttingsBox->setTitle(QApplication::translate("kmsettings", "Policy Settings", 0, QApplication::UnicodeUTF8));

    const bool __sortingEnabled = policySettingsList->isSortingEnabled();
    policySettingsList->setSortingEnabled(false);
    policySettingsList->item(0)->setText(QApplication::translate("kmsettings", "[Graphic Design]", 0, QApplication::UnicodeUTF8));
    policySettingsList->item(1)->setText(QApplication::translate("kmsettings", "[Home / Office]", 0, QApplication::UnicodeUTF8));
    policySettingsList->item(2)->setText(QApplication::translate("kmsettings", "[Photography]", 0, QApplication::UnicodeUTF8));
    policySettingsList->item(3)->setText(QApplication::translate("kmsettings", "[Printer]", 0, QApplication::UnicodeUTF8));

    policySettingsList->setSortingEnabled(__sortingEnabled);
    setPolicyButton->setText(QApplication::translate("kmsettings", "Set Policy as Default", 0, QApplication::UnicodeUTF8));
    policyLabel->setText(QApplication::translate("kmsettings", "Change the global color policy:", 0, QApplication::UnicodeUTF8));
    removePolicyButton->setText(QApplication::translate("kmsettings", "Remove Custom Policy", 0, QApplication::UnicodeUTF8));
    addNewPolicyButton->setText(QApplication::translate("kmsettings", "Add Custom Policy", 0, QApplication::UnicodeUTF8));
    kmsettingsTab->setTabText(kmsettingsTab->indexOf(policySettingsTab), QApplication::translate("kmsettings", "Policy Settings", 0, QApplication::UnicodeUTF8));
    defaultProfilesBox->setTitle(QApplication::translate("kmsettings", "Editing Profile Defaults", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("kmsettings", "Editing RGB", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("kmsettings", "Editing CMYK", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("kmsettings", "Editing XYZ", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("kmsettings", "Editing Lab", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("kmsettings", "Editing Gray", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("kmsettings", "Choose an appropriate profile for the following color space settings:", 0, QApplication::UnicodeUTF8));
    assumedSourceGBox->setTitle(QApplication::translate("kmsettings", "Assumed Profile Default", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("kmsettings", "Assumed RGB Source", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("kmsettings", "Assumed CMYK Source", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("kmsettings", "Assumed XYZ Source", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("kmsettings", "Assumed Lab Source", 0, QApplication::UnicodeUTF8));
    webSourceLabel->setText(QApplication::translate("kmsettings", "Web Source", 0, QApplication::UnicodeUTF8));
    editRgbCombo_7->clear();
    editRgbCombo_7->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "sRGB.icc (default)", 0, QApplication::UnicodeUTF8)
    );
    assumedGrayLabel->setText(QApplication::translate("kmsettings", "Assumed Gray Source", 0, QApplication::UnicodeUTF8));
    kmsettingsTab->setTabText(kmsettingsTab->indexOf(defaultProfilesTab), QApplication::translate("kmsettings", "Default Profiles", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("kmsettings", "Mismatch Handling", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("kmsettings", "For Non-existent Profile", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("kmsettings", "For RGB Mismatch", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("kmsettings", "For CMYK Mismatch", 0, QApplication::UnicodeUTF8));
    ifNoProfileCombo->clear();
    ifNoProfileCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Don't Assign Profile", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Assign Assumed Profile", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Prompt User", 0, QApplication::UnicodeUTF8)
    );
    ifRgbMismatchCombo->clear();
    ifRgbMismatchCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Preserve Numbers", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Automatically Convert", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Prompt User", 0, QApplication::UnicodeUTF8)
    );
    ifCmykMismatchCombo->clear();
    ifCmykMismatchCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Preserve Numbers", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Automatically Convert", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Prompt User", 0, QApplication::UnicodeUTF8)
    );
    groupBox_3->setTitle(QApplication::translate("kmsettings", "Mixed Color Space Settings", 0, QApplication::UnicodeUTF8));
    label_17->setText(QApplication::translate("kmsettings", "For Screen", 0, QApplication::UnicodeUTF8));
    monitorMixedCombo->clear();
    monitorMixedCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Preserve Numbers", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Convert to Editing RGB Space", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Convert to Web ", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Prompt User", 0, QApplication::UnicodeUTF8)
    );
    label_18->setText(QApplication::translate("kmsettings", "Printer", 0, QApplication::UnicodeUTF8));
    printerMixedCombo->clear();
    printerMixedCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Preserve Numbers", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Convert to Editing CMYK Space", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Convert to Untagged CMYK", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Prompt User", 0, QApplication::UnicodeUTF8)
    );
    groupBox_2->setTitle(QApplication::translate("kmsettings", "Proofing", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("kmsettings", "Proofing Colorspace", 0, QApplication::UnicodeUTF8));
    proofRIntentCombo->clear();
    proofRIntentCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Relative Colormetric", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Absolute Colormetric", 0, QApplication::UnicodeUTF8)
    );
    hardProofCheckbox->setText(QApplication::translate("kmsettings", "Hard Proof ", 0, QApplication::UnicodeUTF8));
    softProofCheckbox->setText(QApplication::translate("kmsettings", "Soft Proof", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("kmsettings", "Proofing Rendering Intent", 0, QApplication::UnicodeUTF8));
    renderingSettingsBox->setTitle(QApplication::translate("kmsettings", "Rendering Settings", 0, QApplication::UnicodeUTF8));
    defaultRIntentCombo->clear();
    defaultRIntentCombo->insertItems(0, QStringList()
     << QApplication::translate("kmsettings", "Perceptual", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Relative Colormetric", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Saturation", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("kmsettings", "Absolute Colormetric", 0, QApplication::UnicodeUTF8)
    );

#ifndef QT_NO_TOOLTIP
    defaultRIntentCombo->setToolTip(QString());
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_STATUSTIP
    defaultRIntentCombo->setStatusTip(QString());
#endif // QT_NO_STATUSTIP


#ifndef QT_NO_WHATSTHIS
    defaultRIntentCombo->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    kmsettingsTab->setTabText(kmsettingsTab->indexOf(tab), QApplication::translate("kmsettings", "Behavior Settings", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(kmsettings);
    } // retranslateUi

};

namespace Ui {
    class kmsettings: public Ui_kmsettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KMSETTINGS_H
