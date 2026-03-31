#include "interfacepcclient.h"
#include "ui_interfacepcclient.h"

InterfacePcClient::InterfacePcClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InterfacePcClient)
{
    ui->setupUi(this);
    statusLabel = new QLabel;
    statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
    ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->showMessage("Application démarrée !!", 5000);
    refreshUniversList();
    refreshEquipmentsGrid();
}

InterfacePcClient::~InterfacePcClient()
{
    delete ui;
}


void InterfacePcClient::on_btnGoToList_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->listPage);
    refreshUniversList();
    refreshEquipmentsGrid();
}


void InterfacePcClient::on_btnAddMain_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->formPage);
}


void InterfacePcClient::on_btnGoToScenes_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->scenesPage);
}


void InterfacePcClient::on_btnGoToLive_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->livePage);
}


void InterfacePcClient::on_btnAddUnivers_clicked()
{
    DialogUnivers univers;

    if (univers.exec() == QDialog::Accepted && univers.getNumUnivers() != 0 && !univers.getIpUnivers().isEmpty()){
        if (bdd.enregistrerUnivers(univers.getNumUnivers(), univers.getIpUnivers())){
            refreshUniversList();
            // qDebug() << "Bddok";
        }else {
            statusLabel->setText("Impossible d'enregistrer l'univers");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}


void InterfacePcClient::refreshUniversList()
{
    ui->uiUniversList->clear();
    ui->universCombo->clear();
    universList = bdd.chargerUnivers();
    for (const auto& u : universList) {
        QString display = QString("Univers %1 (%2)").arg(u.numero).arg(u.ip);
        ui->uiUniversList->addItem(display);
        ui->universCombo->addItem(display, u.idUnivers);
    }
    ui->btnEditUnivers->setEnabled(false);
    ui->btnDeleteUnivers->setEnabled(false);
}


void InterfacePcClient::on_btnEditUnivers_clicked()
{
    int row = ui->uiUniversList->currentRow();
    UniversData& u = universList[row];
    DialogUnivers univers;

    if (univers.exec() == QDialog::Accepted && univers.getNumUnivers() != 0 && !univers.getIpUnivers().isEmpty()){
        if (bdd.modifierUnivers(u.idUnivers, univers.getNumUnivers(), univers.getIpUnivers())){
            refreshUniversList();
            statusLabel->setText("Modification de l'univers réussie");
            statusLabel->setStyleSheet("color: green; font-weight: bold; margin-right: 10px;");
        }else {
            statusLabel->setText("Impossible de modifier l'univers");
            statusLabel->setStyleSheet("color: red; font-weight: bold; margin-right: 10px;");
        }
    }
}

void InterfacePcClient::on_btnDeleteUnivers_clicked()
{
    int row = ui->uiUniversList->currentRow();
    if (row < 0 || row >= universList.size()) return;

    if (QMessageBox::question(this, "Confirmation",
            "Supprimer cet univers ? Cela supprimera les équipements associés.")
        != QMessageBox::Yes) return;

    if (bdd.supprimerUnivers(universList[row].idUnivers)) {
        universList = bdd.chargerUnivers();
        for (int i = 0; i < universList.size(); ++i) {
            if (universList[i].numero != i + 1)
                bdd.modifierUnivers(universList[i].idUnivers, i + 1, universList[i].ip);
        }
        refreshUniversList();
        statusBar()->showMessage("Univers supprimé et liste réindexée.", 3000);
    }
}

void InterfacePcClient::on_uiUniversList_currentRowChanged(int currentRow)
{
    bool hasSelection = ui->uiUniversList->currentRow() >= 0;
    ui->btnEditUnivers->setEnabled(hasSelection);
    ui->btnDeleteUnivers->setEnabled(hasSelection);
}

void InterfacePcClient::refreshEquipmentsGrid()
{
    QLayoutItem* item;
    while ((item = ui->equipmentsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    equipmentsList = bdd.chargerTousLesEquipements();
    for (int i = 0; i < equipmentsList.size(); ++i)
        ui->equipmentsGrid->addWidget(createEquipmentCard(equipmentsList[i], i), i / 2, i % 2);
}

QFrame *InterfacePcClient::createEquipmentCard(const EquipmentData &eq, int index)
{
    QFrame* card = new QFrame();
    card->setObjectName("card");
    card->setFixedWidth(380);
    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* title = new QLabel(eq.nom);
    title->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    QGridLayout* info = new QGridLayout();
    info->addWidget(new QLabel("Univers:"),     0, 0); info->addWidget(new QLabel(eq.univers),                        0, 1);
    info->addWidget(new QLabel("Adresse DMX:"), 1, 0); info->addWidget(new QLabel(eq.dmxStart),                       1, 1);
    info->addWidget(new QLabel("Canaux:"),      2, 0); info->addWidget(new QLabel(QString::number(eq.canaux.size())), 2, 1);
    layout->addLayout(info);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton* btnEdit = new QPushButton("Modifier");
    btnEdit->setStyleSheet("QPushButton { background-color: #555555; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } "
                           "QPushButton:hover { background-color: #666666; }");
    connect(btnEdit, &QPushButton::clicked, [this, index]() { editEquipment(index); });
    QPushButton* btnDel = new QPushButton("Supprimer");
    btnDel->setStyleSheet("QPushButton { background-color: #d32f2f; color: white; border-radius: 4px; padding: 8px; font-weight: bold; } "
                          "QPushButton:hover { background-color: #f44336; }");
    connect(btnDel, &QPushButton::clicked, [this, index]() { deleteEquipment(index); });
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);
    layout->addLayout(btnLayout);
    return card;
}

void InterfacePcClient::editEquipment(int index)
{
    if (index < 0 || index >= equipmentsList.size()) return;
    currentEditEquipIndex = index;
    const EquipmentData& eq = equipmentsList[index];
    clearForm();
    ui->nameEdit->setText(eq.nom);
    ui->startAddressEdit->setText(eq.dmxStart);
    for (const ChannelData& cd : eq.canaux) addChannelToForm(&cd);
    ui->stackedWidget->setCurrentWidget(ui->formPage);
}

void InterfacePcClient::deleteEquipment(int index)
{
    if (index < 0 || index >= equipmentsList.size()) return;
    if (QMessageBox::question(this, "Confirmation", "Supprimer cet équipement ?") != QMessageBox::Yes) return;
    if (bdd.supprimerEquipment(equipmentsList[index].idEquipement))
        refreshEquipmentsGrid();
    else
        statusBar()->showMessage("Impossible de supprimer l'équipement", 3000);
}

void InterfacePcClient::clearForm()
{
    ui->nameEdit->clear();
    ui->startAddressEdit->clear();
    channelCounter = 0;
    ui->channelCountLabel->setText("CANAUX: <span style='color:#107c7c;'>0</span>");
    QLayoutItem* item;
    while ((item = ui->channelsFormLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}

void InterfacePcClient::addChannelToForm(const ChannelData *data)
{
    channelCounter++;
    ui->channelCountLabel->setText(
        QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));

    QFrame* channelFrame = new QFrame();
    channelFrame->setObjectName("card");
    QVBoxLayout* cLayout = new QVBoxLayout(channelFrame);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel*      cTitle       = new QLabel(QString("CANAL #%1").arg(channelCounter));
    cTitle->setObjectName("boldLabel");
    QLineEdit*   cDesc        = new QLineEdit();
    cDesc->setPlaceholderText("Description du canal");
    if (data) cDesc->setText(data->description);
    QPushButton* btnDelChannel = new QPushButton("Supprimer");
    btnDelChannel->setObjectName("btnRed");
    headerLayout->addWidget(cTitle);
    headerLayout->addWidget(cDesc, 1);
    headerLayout->addWidget(btnDelChannel);
    cLayout->addLayout(headerLayout);

    QVBoxLayout* funcsLayout = new QVBoxLayout();
    cLayout->addLayout(funcsLayout);

    QPushButton* btnAddFunc = new QPushButton("+ Ajouter fonction");
    btnAddFunc->setObjectName("btnDashed");
    cLayout->addWidget(btnAddFunc);

    connect(btnDelChannel, &QPushButton::clicked, [this, channelFrame]() {
        channelFrame->deleteLater();
        channelCounter--;
        ui->channelCountLabel->setText(
            QString("CANAUX: <span style='color:#107c7c;'>%1</span>").arg(channelCounter));
    });
    connect(btnAddFunc, &QPushButton::clicked,
            [this, funcsLayout]() { addFunctionToChannel(funcsLayout, nullptr); });

    if (data && !data->fonctions.isEmpty())
        for (const auto& func : data->fonctions) addFunctionToChannel(funcsLayout, &func);
    else
        addFunctionToChannel(funcsLayout, nullptr);

    ui->channelsFormLayout->addWidget(channelFrame);
}

void InterfacePcClient::addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data)
{
    QWidget*     funcWidget = new QWidget();
    QHBoxLayout* layout     = new QHBoxLayout(funcWidget);
    layout->setContentsMargins(0, 5, 0, 5);

    QLineEdit* fName = new QLineEdit(); fName->setPlaceholderText("Fonction");
    QLineEdit* fMin  = new QLineEdit(); fMin->setPlaceholderText("Min"); fMin->setFixedWidth(60);
    QLineEdit* fMax  = new QLineEdit(); fMax->setPlaceholderText("Max"); fMax->setFixedWidth(60);

    if (data) { fName->setText(data->nom); fMin->setText(data->min); fMax->setText(data->max); }

    QPushButton* btnDelFunc = new QPushButton("X");
    btnDelFunc->setObjectName("btnRed");
    connect(btnDelFunc, &QPushButton::clicked, funcWidget, &QWidget::deleteLater);

    layout->addWidget(fName);
    layout->addWidget(fMin);
    layout->addWidget(fMax);
    layout->addWidget(btnDelFunc);
    functionsLayout->addWidget(funcWidget);
}
