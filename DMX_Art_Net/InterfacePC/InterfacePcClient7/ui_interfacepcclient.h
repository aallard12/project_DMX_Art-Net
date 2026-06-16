/********************************************************************************
** Form generated from reading UI file 'interfacepcclient.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERFACEPCCLIENT_H
#define UI_INTERFACEPCCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InterfacePcClient
{
public:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QFrame *header;
    QHBoxLayout *headerLayout;
    QLabel *mainTitle;
    QSpacerItem *hSpacer;
    QPushButton *btnGoToList;
    QPushButton *btnAddMain;
    QPushButton *btnGoToScenes;
    QPushButton *btnGoToLive;
    QPushButton *btnGoToConnect;
    QStackedWidget *stackedWidget;
    QWidget *connectPage;
    QLabel *label_5;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEditNomUser;
    QLineEdit *lineEditMDP;
    QPushButton *pushButtonSaveUser;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_4;
    QListWidget *listWidgetUser;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonEditUser;
    QPushButton *pushButtonDeleteUser;
    QWidget *listPage;
    QHBoxLayout *listPageLayout;
    QWidget *widget;
    QGridLayout *gridLayout;
    QComboBox *filterUniversCombo;
    QScrollArea *listScroll;
    QWidget *scrollContent;
    QWidget *gridLayoutWidget;
    QGridLayout *equipmentsGrid;
    QLabel *filterLabel;
    QFrame *sidebar;
    QVBoxLayout *sidebarLayout;
    QLabel *uniTitle;
    QListWidget *uiUniversList;
    QHBoxLayout *uniActionsLayout;
    QPushButton *btnEditUnivers;
    QPushButton *btnDeleteUnivers;
    QPushButton *btnAddUnivers;
    QWidget *formPage;
    QVBoxLayout *formPageLayout;
    QScrollArea *formScroll;
    QWidget *formScrollContent;
    QVBoxLayout *formLayout;
    QLabel *formTitle;
    QGridLayout *infoGrid;
    QLineEdit *startAddressEdit;
    QLineEdit *nameEdit;
    QComboBox *universCombo;
    QPushButton *pushButtonCouleur;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *pushButtonImport;
    QLabel *channelCountLabel;
    QVBoxLayout *channelsFormLayout;
    QPushButton *btnAddChannelGlobal;
    QHBoxLayout *bottomLayout;
    QSpacerItem *s;
    QPushButton *btnCancel;
    QPushButton *btnSave;
    QWidget *scenesPage;
    QVBoxLayout *scenesPageLayout;
    QVBoxLayout *scenesTopLayout;
    QHBoxLayout *scenesH1;
    QLabel *scenesTitle;
    QSpacerItem *s2;
    QComboBox *scenesUniversCombo;
    QPushButton *btnResetSliders;
    QHBoxLayout *scenesH2;
    QComboBox *scenesCombo;
    QPushButton *btnRenameScene;
    QPushButton *btnDeleteScene;
    QPushButton *btnSaveScene;
    QScrollArea *slidersScroll;
    QWidget *slidersContainer;
    QGridLayout *slidersGrid;
    QWidget *livePage;
    QVBoxLayout *liveMainLayout;
    QHBoxLayout *liveTitleLayout;
    QLabel *liveTitle;
    QSpacerItem *liveTitleSpacer;
    QLabel *liveFilterLabel;
    QComboBox *liveUniversCombo;
    QHBoxLayout *tcpLayout;
    QLineEdit *lineEditIP;
    QSpinBox *spinBoxPort;
    QPushButton *btnConnectTCP;
    QListWidget *liveScenesList;
    QPushButton *btnLaunchLiveScene;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *InterfacePcClient)
    {
        if (InterfacePcClient->objectName().isEmpty())
            InterfacePcClient->setObjectName("InterfacePcClient");
        InterfacePcClient->resize(1314, 811);
        InterfacePcClient->setStyleSheet(QString::fromUtf8("QMainWindow { background-color: #2b2b2b; }"));
        centralWidget = new QWidget(InterfacePcClient);
        centralWidget->setObjectName("centralWidget");
        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        header = new QFrame(centralWidget);
        header->setObjectName("header");
        header->setStyleSheet(QString::fromUtf8("QFrame#header { background-color: #1e1e1e; border-bottom: 1px solid #000; }"));
        headerLayout = new QHBoxLayout(header);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(20, 15, 20, 15);
        mainTitle = new QLabel(header);
        mainTitle->setObjectName("mainTitle");
        mainTitle->setStyleSheet(QString::fromUtf8("color: #ffffff; font-size: 20px; font-weight: bold; font-family: 'Segoe UI';"));

        headerLayout->addWidget(mainTitle);

        hSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(hSpacer);

        btnGoToList = new QPushButton(header);
        btnGoToList->setObjectName("btnGoToList");
        btnGoToList->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #107c7c; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        headerLayout->addWidget(btnGoToList);

        btnAddMain = new QPushButton(header);
        btnAddMain->setObjectName("btnAddMain");
        btnAddMain->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #20b2aa; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        headerLayout->addWidget(btnAddMain);

        btnGoToScenes = new QPushButton(header);
        btnGoToScenes->setObjectName("btnGoToScenes");
        btnGoToScenes->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #008b8b; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        headerLayout->addWidget(btnGoToScenes);

        btnGoToLive = new QPushButton(header);
        btnGoToLive->setObjectName("btnGoToLive");
        btnGoToLive->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #005f5f; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        headerLayout->addWidget(btnGoToLive);

        btnGoToConnect = new QPushButton(header);
        btnGoToConnect->setObjectName("btnGoToConnect");
        btnGoToConnect->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #4F7575; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        headerLayout->addWidget(btnGoToConnect);


        mainLayout->addWidget(header);

        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName("stackedWidget");
        connectPage = new QWidget();
        connectPage->setObjectName("connectPage");
        label_5 = new QLabel(connectPage);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(30, 30, 171, 41));
        label_5->setStyleSheet(QString::fromUtf8("color: #107c7c; font-size: 30px; font-weight: bold;"));
        layoutWidget = new QWidget(connectPage);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(210, 180, 831, 371));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        lineEditNomUser = new QLineEdit(layoutWidget);
        lineEditNomUser->setObjectName("lineEditNomUser");
        lineEditNomUser->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        verticalLayout->addWidget(lineEditNomUser);

        lineEditMDP = new QLineEdit(layoutWidget);
        lineEditMDP->setObjectName("lineEditMDP");
        lineEditMDP->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        verticalLayout->addWidget(lineEditMDP);

        pushButtonSaveUser = new QPushButton(layoutWidget);
        pushButtonSaveUser->setObjectName("pushButtonSaveUser");
        pushButtonSaveUser->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #35AB35; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        verticalLayout->addWidget(pushButtonSaveUser);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");
        label_4->setStyleSheet(QString::fromUtf8("color: #107c7c; font-size: 16px; font-weight: bold;"));

        verticalLayout_3->addWidget(label_4);

        listWidgetUser = new QListWidget(layoutWidget);
        listWidgetUser->setObjectName("listWidgetUser");
        listWidgetUser->setStyleSheet(QString::fromUtf8("QListWidget { background-color: #383838; border: 1px solid #555; color: #ffffff; border-radius: 4px; } QListWidget::item { padding: 10px; border-bottom: 1px solid #444; } QListWidget::item:selected { background-color: #107c7c; color: white; }"));

        verticalLayout_3->addWidget(listWidgetUser);


        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        pushButtonEditUser = new QPushButton(layoutWidget);
        pushButtonEditUser->setObjectName("pushButtonEditUser");
        pushButtonEditUser->setEnabled(false);
        pushButtonEditUser->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #35AB35; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        verticalLayout_2->addWidget(pushButtonEditUser);

        pushButtonDeleteUser = new QPushButton(layoutWidget);
        pushButtonDeleteUser->setObjectName("pushButtonDeleteUser");
        pushButtonDeleteUser->setEnabled(false);
        pushButtonDeleteUser->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #D91A09; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #149999; }"));

        verticalLayout_2->addWidget(pushButtonDeleteUser);


        horizontalLayout->addLayout(verticalLayout_2);

        stackedWidget->addWidget(connectPage);
        listPage = new QWidget();
        listPage->setObjectName("listPage");
        listPageLayout = new QHBoxLayout(listPage);
        listPageLayout->setContentsMargins(0, 0, 0, 0);
        listPageLayout->setObjectName("listPageLayout");
        widget = new QWidget(listPage);
        widget->setObjectName("widget");
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        filterUniversCombo = new QComboBox(widget);
        filterUniversCombo->setObjectName("filterUniversCombo");
        filterUniversCombo->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 6px; border-radius: 4px;"));

        gridLayout->addWidget(filterUniversCombo, 1, 1, 1, 1);

        listScroll = new QScrollArea(widget);
        listScroll->setObjectName("listScroll");
        listScroll->setMinimumSize(QSize(597, 635));
        listScroll->setFrameShape(QFrame::Shape::NoFrame);
        listScroll->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName("scrollContent");
        scrollContent->setGeometry(QRect(0, 0, 1022, 635));
        gridLayoutWidget = new QWidget(scrollContent);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(0, 0, 1021, 631));
        equipmentsGrid = new QGridLayout(gridLayoutWidget);
        equipmentsGrid->setSpacing(15);
        equipmentsGrid->setObjectName("equipmentsGrid");
        equipmentsGrid->setContentsMargins(0, 0, 0, 0);
        listScroll->setWidget(scrollContent);

        gridLayout->addWidget(listScroll, 3, 1, 1, 1);

        filterLabel = new QLabel(widget);
        filterLabel->setObjectName("filterLabel");
        filterLabel->setStyleSheet(QString::fromUtf8("color: #aaaaaa; font-size: 12px; font-weight: normal;"));

        gridLayout->addWidget(filterLabel, 0, 1, 1, 1);

        sidebar = new QFrame(widget);
        sidebar->setObjectName("sidebar");
        sidebar->setEnabled(true);
        sidebar->setMinimumSize(QSize(50, 635));
        sidebar->setMaximumSize(QSize(250, 16777215));
        sidebar->setStyleSheet(QString::fromUtf8("QFrame#sidebar { background-color: #222222; border-right: 1px solid #111; } QLabel { color: #107c7c; font-size: 16px; font-weight: bold; }"));
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setObjectName("sidebarLayout");
        uniTitle = new QLabel(sidebar);
        uniTitle->setObjectName("uniTitle");

        sidebarLayout->addWidget(uniTitle);

        uiUniversList = new QListWidget(sidebar);
        uiUniversList->setObjectName("uiUniversList");
        uiUniversList->setStyleSheet(QString::fromUtf8("QListWidget { background-color: #383838; border: 1px solid #555; color: #ffffff; border-radius: 4px; } QListWidget::item { padding: 10px; border-bottom: 1px solid #444; } QListWidget::item:selected { background-color: #107c7c; color: white; }"));

        sidebarLayout->addWidget(uiUniversList);

        uniActionsLayout = new QHBoxLayout();
        uniActionsLayout->setObjectName("uniActionsLayout");
        btnEditUnivers = new QPushButton(sidebar);
        btnEditUnivers->setObjectName("btnEditUnivers");
        btnEditUnivers->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #555555; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } QPushButton:hover { background-color: #666666; }"));

        uniActionsLayout->addWidget(btnEditUnivers);

        btnDeleteUnivers = new QPushButton(sidebar);
        btnDeleteUnivers->setObjectName("btnDeleteUnivers");
        btnDeleteUnivers->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #d32f2f; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } QPushButton:hover { background-color: #f44336; }"));

        uniActionsLayout->addWidget(btnDeleteUnivers);


        sidebarLayout->addLayout(uniActionsLayout);

        btnAddUnivers = new QPushButton(sidebar);
        btnAddUnivers->setObjectName("btnAddUnivers");
        btnAddUnivers->setStyleSheet(QString::fromUtf8("QPushButton { background-color: transparent; color: #107c7c; border: 1px dashed #107c7c; padding: 12px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: rgba(16, 124, 124, 0.1); }"));

        sidebarLayout->addWidget(btnAddUnivers);


        gridLayout->addWidget(sidebar, 0, 0, 4, 1);


        listPageLayout->addWidget(widget);

        stackedWidget->addWidget(listPage);
        formPage = new QWidget();
        formPage->setObjectName("formPage");
        formPageLayout = new QVBoxLayout(formPage);
        formPageLayout->setContentsMargins(20, 20, 20, 20);
        formPageLayout->setObjectName("formPageLayout");
        formScroll = new QScrollArea(formPage);
        formScroll->setObjectName("formScroll");
        formScroll->setWidgetResizable(true);
        formScrollContent = new QWidget();
        formScrollContent->setObjectName("formScrollContent");
        formScrollContent->setGeometry(QRect(0, 0, 1254, 668));
        formScrollContent->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        formLayout = new QVBoxLayout(formScrollContent);
        formLayout->setObjectName("formLayout");
        formTitle = new QLabel(formScrollContent);
        formTitle->setObjectName("formTitle");
        formTitle->setMinimumSize(QSize(0, 0));
        formTitle->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setBold(true);
        formTitle->setFont(font);
        formTitle->setStyleSheet(QString::fromUtf8("color: #107c7c; font-size: 16px; font-weight: bold;"));
        formTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout->addWidget(formTitle);

        infoGrid = new QGridLayout();
        infoGrid->setObjectName("infoGrid");
        startAddressEdit = new QLineEdit(formScrollContent);
        startAddressEdit->setObjectName("startAddressEdit");
        startAddressEdit->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        infoGrid->addWidget(startAddressEdit, 1, 2, 1, 1);

        nameEdit = new QLineEdit(formScrollContent);
        nameEdit->setObjectName("nameEdit");
        nameEdit->setMinimumSize(QSize(0, 30));
        nameEdit->setMaximumSize(QSize(16777215, 150));
        nameEdit->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        infoGrid->addWidget(nameEdit, 1, 0, 1, 1);

        universCombo = new QComboBox(formScrollContent);
        universCombo->setObjectName("universCombo");
        universCombo->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        infoGrid->addWidget(universCombo, 1, 1, 1, 1);

        pushButtonCouleur = new QPushButton(formScrollContent);
        pushButtonCouleur->setObjectName("pushButtonCouleur");
        pushButtonCouleur->setStyleSheet(QString::fromUtf8("QPushButton {background-color: \"#000000\" ;color: white; border-radius: 4px; padding: 8px; font-weight: bold;}"));

        infoGrid->addWidget(pushButtonCouleur, 1, 3, 1, 1);

        label = new QLabel(formScrollContent);
        label->setObjectName("label");
        label->setMinimumSize(QSize(0, 0));
        label->setMaximumSize(QSize(16777215, 50));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        infoGrid->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(formScrollContent);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        infoGrid->addWidget(label_2, 0, 2, 1, 1);

        label_3 = new QLabel(formScrollContent);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        infoGrid->addWidget(label_3, 0, 1, 1, 1);

        pushButtonImport = new QPushButton(formScrollContent);
        pushButtonImport->setObjectName("pushButtonImport");
        pushButtonImport->setStyleSheet(QString::fromUtf8("QPushButton {background-color: \"#26A269\" ;color: black; border-radius: 4px; padding: 8px; font-weight: bold;}"));

        infoGrid->addWidget(pushButtonImport, 0, 3, 1, 1);


        formLayout->addLayout(infoGrid);

        channelCountLabel = new QLabel(formScrollContent);
        channelCountLabel->setObjectName("channelCountLabel");
        channelCountLabel->setMaximumSize(QSize(16777215, 50));
        channelCountLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; font-size: 13px; color: #aaaaaa;"));

        formLayout->addWidget(channelCountLabel);

        channelsFormLayout = new QVBoxLayout();
        channelsFormLayout->setSpacing(6);
        channelsFormLayout->setObjectName("channelsFormLayout");

        formLayout->addLayout(channelsFormLayout);

        btnAddChannelGlobal = new QPushButton(formScrollContent);
        btnAddChannelGlobal->setObjectName("btnAddChannelGlobal");
        btnAddChannelGlobal->setStyleSheet(QString::fromUtf8("QPushButton { background-color: transparent; color: #107c7c; border: 1px dashed #107c7c; padding: 12px; border-radius: 4px; font-weight: bold; }"));

        formLayout->addWidget(btnAddChannelGlobal);

        bottomLayout = new QHBoxLayout();
        bottomLayout->setObjectName("bottomLayout");
        s = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        bottomLayout->addItem(s);

        btnCancel = new QPushButton(formScrollContent);
        btnCancel->setObjectName("btnCancel");
        btnCancel->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #555555; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; }"));

        bottomLayout->addWidget(btnCancel);

        btnSave = new QPushButton(formScrollContent);
        btnSave->setObjectName("btnSave");
        btnSave->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #388e3c; color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; }"));

        bottomLayout->addWidget(btnSave);


        formLayout->addLayout(bottomLayout);

        formScroll->setWidget(formScrollContent);

        formPageLayout->addWidget(formScroll);

        stackedWidget->addWidget(formPage);
        scenesPage = new QWidget();
        scenesPage->setObjectName("scenesPage");
        scenesPageLayout = new QVBoxLayout(scenesPage);
        scenesPageLayout->setContentsMargins(20, 20, 20, 20);
        scenesPageLayout->setObjectName("scenesPageLayout");
        scenesTopLayout = new QVBoxLayout();
        scenesTopLayout->setObjectName("scenesTopLayout");
        scenesH1 = new QHBoxLayout();
        scenesH1->setObjectName("scenesH1");
        scenesTitle = new QLabel(scenesPage);
        scenesTitle->setObjectName("scenesTitle");
        scenesTitle->setStyleSheet(QString::fromUtf8("color: #107c7c; font-size: 16px; font-weight: bold;"));

        scenesH1->addWidget(scenesTitle);

        s2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        scenesH1->addItem(s2);

        scenesUniversCombo = new QComboBox(scenesPage);
        scenesUniversCombo->setObjectName("scenesUniversCombo");
        scenesUniversCombo->setStyleSheet(QString::fromUtf8("background-color: #383838; color: white; padding: 5px; border-radius: 4px;"));

        scenesH1->addWidget(scenesUniversCombo);

        btnResetSliders = new QPushButton(scenesPage);
        btnResetSliders->setObjectName("btnResetSliders");
        btnResetSliders->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #555555; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold;}"));

        scenesH1->addWidget(btnResetSliders);


        scenesTopLayout->addLayout(scenesH1);

        scenesH2 = new QHBoxLayout();
        scenesH2->setObjectName("scenesH2");
        scenesCombo = new QComboBox(scenesPage);
        scenesCombo->setObjectName("scenesCombo");
        scenesCombo->setStyleSheet(QString::fromUtf8("background-color: #383838; color: white; padding: 8px; border-radius: 4px;"));

        scenesH2->addWidget(scenesCombo);

        btnRenameScene = new QPushButton(scenesPage);
        btnRenameScene->setObjectName("btnRenameScene");
        btnRenameScene->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #555555; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }"));

        scenesH2->addWidget(btnRenameScene);

        btnDeleteScene = new QPushButton(scenesPage);
        btnDeleteScene->setObjectName("btnDeleteScene");
        btnDeleteScene->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #d32f2f; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }"));

        scenesH2->addWidget(btnDeleteScene);

        btnSaveScene = new QPushButton(scenesPage);
        btnSaveScene->setObjectName("btnSaveScene");
        btnSaveScene->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #388e3c; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }"));

        scenesH2->addWidget(btnSaveScene);


        scenesTopLayout->addLayout(scenesH2);


        scenesPageLayout->addLayout(scenesTopLayout);

        slidersScroll = new QScrollArea(scenesPage);
        slidersScroll->setObjectName("slidersScroll");
        slidersScroll->setWidgetResizable(true);
        slidersContainer = new QWidget();
        slidersContainer->setObjectName("slidersContainer");
        slidersContainer->setGeometry(QRect(0, 0, 1254, 586));
        slidersGrid = new QGridLayout(slidersContainer);
        slidersGrid->setObjectName("slidersGrid");
        slidersScroll->setWidget(slidersContainer);

        scenesPageLayout->addWidget(slidersScroll);

        stackedWidget->addWidget(scenesPage);
        livePage = new QWidget();
        livePage->setObjectName("livePage");
        liveMainLayout = new QVBoxLayout(livePage);
        liveMainLayout->setContentsMargins(20, 20, 20, 20);
        liveMainLayout->setObjectName("liveMainLayout");
        liveTitleLayout = new QHBoxLayout();
        liveTitleLayout->setObjectName("liveTitleLayout");
        liveTitle = new QLabel(livePage);
        liveTitle->setObjectName("liveTitle");
        liveTitle->setStyleSheet(QString::fromUtf8("color: #107c7c; font-size: 16px; font-weight: bold;"));

        liveTitleLayout->addWidget(liveTitle);

        liveTitleSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        liveTitleLayout->addItem(liveTitleSpacer);

        liveFilterLabel = new QLabel(livePage);
        liveFilterLabel->setObjectName("liveFilterLabel");
        liveFilterLabel->setStyleSheet(QString::fromUtf8("color: #aaaaaa; font-size: 12px; font-weight: normal;"));

        liveTitleLayout->addWidget(liveFilterLabel);

        liveUniversCombo = new QComboBox(livePage);
        liveUniversCombo->setObjectName("liveUniversCombo");
        liveUniversCombo->setMinimumSize(QSize(200, 0));
        liveUniversCombo->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 6px; border-radius: 4px;"));

        liveTitleLayout->addWidget(liveUniversCombo);


        liveMainLayout->addLayout(liveTitleLayout);

        tcpLayout = new QHBoxLayout();
        tcpLayout->setObjectName("tcpLayout");
        lineEditIP = new QLineEdit(livePage);
        lineEditIP->setObjectName("lineEditIP");
        lineEditIP->setStyleSheet(QString::fromUtf8("background-color: #383838; color: white; padding: 8px; border-radius: 4px;"));

        tcpLayout->addWidget(lineEditIP);

        spinBoxPort = new QSpinBox(livePage);
        spinBoxPort->setObjectName("spinBoxPort");
        spinBoxPort->setStyleSheet(QString::fromUtf8("background-color: #383838; color: white; padding: 8px; border-radius: 4px;"));
        spinBoxPort->setMaximum(65535);
        spinBoxPort->setValue(12345);

        tcpLayout->addWidget(spinBoxPort);

        btnConnectTCP = new QPushButton(livePage);
        btnConnectTCP->setObjectName("btnConnectTCP");
        btnConnectTCP->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #555555;  color: white; border-radius: 4px; padding: 8px 16px; font-weight: bold; border: none; }"));

        tcpLayout->addWidget(btnConnectTCP);


        liveMainLayout->addLayout(tcpLayout);

        liveScenesList = new QListWidget(livePage);
        liveScenesList->setObjectName("liveScenesList");
        liveScenesList->setStyleSheet(QString::fromUtf8("QListWidget { background-color: #383838; color: white; border-radius: 4px; } QListWidget::item { padding: 15px; border-bottom: 1px solid #444; } QListWidget::item:selected { background-color: #107c7c; font-weight: bold; }"));

        liveMainLayout->addWidget(liveScenesList);

        btnLaunchLiveScene = new QPushButton(livePage);
        btnLaunchLiveScene->setObjectName("btnLaunchLiveScene");
        btnLaunchLiveScene->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #388e3c; color: white; padding: 20px; border-radius: 8px; font-size: 18px; font-weight: bold; } QPushButton:disabled { background-color: #444444; color: #777777; }"));

        liveMainLayout->addWidget(btnLaunchLiveScene);

        stackedWidget->addWidget(livePage);

        mainLayout->addWidget(stackedWidget);

        InterfacePcClient->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(InterfacePcClient);
        statusBar->setObjectName("statusBar");
        InterfacePcClient->setStatusBar(statusBar);

        retranslateUi(InterfacePcClient);

        stackedWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(InterfacePcClient);
    } // setupUi

    void retranslateUi(QMainWindow *InterfacePcClient)
    {
        InterfacePcClient->setWindowTitle(QCoreApplication::translate("InterfacePcClient", "DMX Equipment Manager", nullptr));
        mainTitle->setText(QCoreApplication::translate("InterfacePcClient", "DMX Equipment Manager", nullptr));
        btnGoToList->setText(QCoreApplication::translate("InterfacePcClient", "Univers & \303\211quipements", nullptr));
        btnAddMain->setText(QCoreApplication::translate("InterfacePcClient", "Ajouter \303\211quipement", nullptr));
        btnGoToScenes->setText(QCoreApplication::translate("InterfacePcClient", "Cr\303\251er les Sc\303\250nes", nullptr));
        btnGoToLive->setText(QCoreApplication::translate("InterfacePcClient", "Live / R\303\251gie", nullptr));
        btnGoToConnect->setText(QCoreApplication::translate("InterfacePcClient", "Ajout Utilisateur", nullptr));
        label_5->setText(QCoreApplication::translate("InterfacePcClient", "Utilisateurs", nullptr));
        lineEditNomUser->setPlaceholderText(QCoreApplication::translate("InterfacePcClient", "nom d'utilisateur", nullptr));
        lineEditMDP->setPlaceholderText(QCoreApplication::translate("InterfacePcClient", "mot de passe", nullptr));
        pushButtonSaveUser->setText(QCoreApplication::translate("InterfacePcClient", "Ajout Utilisateur", nullptr));
        label_4->setText(QCoreApplication::translate("InterfacePcClient", "Liste des Utilisateurs", nullptr));
        pushButtonEditUser->setText(QCoreApplication::translate("InterfacePcClient", "Modifier", nullptr));
        pushButtonDeleteUser->setText(QCoreApplication::translate("InterfacePcClient", "Supprimer", nullptr));
        filterLabel->setText(QCoreApplication::translate("InterfacePcClient", "Filtrer les \303\251quipements :", nullptr));
        uniTitle->setText(QCoreApplication::translate("InterfacePcClient", "UNIVERS DMX", nullptr));
        btnEditUnivers->setText(QCoreApplication::translate("InterfacePcClient", "Modifier", nullptr));
        btnDeleteUnivers->setText(QCoreApplication::translate("InterfacePcClient", "Supprimer", nullptr));
        btnAddUnivers->setText(QCoreApplication::translate("InterfacePcClient", "+ Ajouter Univers", nullptr));
        formTitle->setText(QCoreApplication::translate("InterfacePcClient", "\303\211quipement DMX", nullptr));
        startAddressEdit->setPlaceholderText(QCoreApplication::translate("InterfacePcClient", "Ex. 1", nullptr));
        nameEdit->setPlaceholderText(QCoreApplication::translate("InterfacePcClient", "Ex. iRock7s", nullptr));
        pushButtonCouleur->setText(QCoreApplication::translate("InterfacePcClient", "Couleur", nullptr));
        label->setStyleSheet(QCoreApplication::translate("InterfacePcClient", "color: #cccccc;", nullptr));
        label->setText(QCoreApplication::translate("InterfacePcClient", "NOM DE L'\303\211QUIPEMENT", nullptr));
        label_2->setText(QCoreApplication::translate("InterfacePcClient", "ADRESSE DMX DE D\303\211PART", nullptr));
        label_3->setText(QCoreApplication::translate("InterfacePcClient", "UNIVERS", nullptr));
        pushButtonImport->setText(QCoreApplication::translate("InterfacePcClient", "Importer \303\211quipement", nullptr));
        btnAddChannelGlobal->setText(QCoreApplication::translate("InterfacePcClient", "+ Ajouter un canal", nullptr));
        btnCancel->setText(QCoreApplication::translate("InterfacePcClient", "Annuler", nullptr));
        btnSave->setText(QCoreApplication::translate("InterfacePcClient", "Enregistrer", nullptr));
        scenesTitle->setText(QCoreApplication::translate("InterfacePcClient", "GESTION DES SC\303\210NES", nullptr));
        btnResetSliders->setText(QCoreApplication::translate("InterfacePcClient", "R\303\251initialiser les curseurs", nullptr));
        btnRenameScene->setText(QCoreApplication::translate("InterfacePcClient", "Renommer", nullptr));
        btnDeleteScene->setText(QCoreApplication::translate("InterfacePcClient", "Supprimer", nullptr));
        btnSaveScene->setText(QCoreApplication::translate("InterfacePcClient", "Sauvegarder", nullptr));
        liveTitle->setText(QCoreApplication::translate("InterfacePcClient", "R\303\211GIE LIVE", nullptr));
        liveFilterLabel->setText(QCoreApplication::translate("InterfacePcClient", "Filtrer par univers :", nullptr));
        lineEditIP->setText(QCoreApplication::translate("InterfacePcClient", "192.168.1.20", nullptr));
        btnConnectTCP->setText(QCoreApplication::translate("InterfacePcClient", "Connexion", nullptr));
        btnLaunchLiveScene->setText(QCoreApplication::translate("InterfacePcClient", "Lancer la sc\303\250ne", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InterfacePcClient: public Ui_InterfacePcClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERFACEPCCLIENT_H
