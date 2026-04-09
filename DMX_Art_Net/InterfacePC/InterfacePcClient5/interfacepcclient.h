#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include "accessbdd.h"
#include "dialogunivers.h"
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSlider>
#include <QTcpSocket>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QColorDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class InterfacePcClient; }
QT_END_NAMESPACE


struct SliderWidgetSet {
    int      idCanalDB = -1;
    QLabel*  labelTitre;
    QSlider* slider;
    QLabel*  labelValeur;
    QString  nomEquipement;
    QString  descriptionBase;
    QString couleur;
    QList<DmxFunctionInfo> fonctions;
};

class InterfacePcClient : public QMainWindow
{
    Q_OBJECT

public:
    InterfacePcClient(QWidget *parent = nullptr);
    ~InterfacePcClient();

private slots:
    void on_btnGoToList_clicked();
    void on_btnAddMain_clicked();
    void on_btnGoToScenes_clicked();
    void on_btnGoToLive_clicked();
    void on_filterUniversCombo_currentIndexChanged(int index);

    void on_btnAddUnivers_clicked();
    void refreshUniversList();
    void on_btnEditUnivers_clicked();
    void on_btnDeleteUnivers_clicked();
    void on_uiUniversList_currentRowChanged(int currentRow);
    void clearForm();

    void on_pushButtonCouleur_clicked();
    void on_btnAddChannelGlobal_clicked();
    void on_btnCancel_clicked();
    void on_btnSave_clicked();

    void on_scenesUniversCombo_currentIndexChanged(int index);
    void on_scenesCombo_currentIndexChanged(int index);
    void on_btnResetSliders_clicked();
    void on_btnRenameScene_clicked();
    void on_btnDeleteScene_clicked();
    void on_btnSaveScene_clicked();
    void refreshScenesList();

    void on_btnConnectTCP_clicked();
    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void on_liveScenesList_itemSelectionChanged();
    void on_btnLaunchLiveScene_clicked();
    void lancerScene(int idScene);
    void on_liveUniversCombo_currentIndexChanged(int index);
    void refreshLiveScenesList(int idUniversFiltre);

private:
    Ui::InterfacePcClient *ui;
    AccessBDD bdd;
    QTcpSocket socketClient;

    QList<UniversData>     universList;
    QList<EquipmentData>   equipmentsList;
    QList<SceneData>       scenesList;
    QList<SliderWidgetSet> dmxSliders;

    int currentEditEquipIndex = -1;
    int channelCounter        = 0;
    int selectedLiveSceneId   = -1;
    QString couleurActuelle = "#383838";
    QLabel*      statusLabel;

    void refreshEquipmentsGrid();
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);
    void editEquipment(int index);
    void deleteEquipment(int index);
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
    void creerSliders(int nombreCanaux);
};
#endif // INTERFACEPCCLIENT_H
