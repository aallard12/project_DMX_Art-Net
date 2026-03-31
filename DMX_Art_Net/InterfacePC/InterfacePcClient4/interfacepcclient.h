#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include "accessbdd.h"
#include "dialogunivers.h"
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class InterfacePcClient; }
QT_END_NAMESPACE

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

    // void saveEquipment();
    // void clearForm();
    // void onScenesUniversChanged();
    // void saveCurrentScene();
    // void refreshScenesList();
    // void onSceneSelectionChanged();
    // void resetSliders();
    // void onRenameSceneClicked();
    // void onDeleteSceneClicked();
    // void onLiveSceneSelected();
    // void onLaunchButtonClicked();
    // void lancerScene(int idScene);
    // void onQTcpSocket_connected();
    // void onQTcpSocket_disconnected();
    // void on_btnConnect_clicked();

    void on_btnAddUnivers_clicked();
    void refreshUniversList();
    void on_btnEditUnivers_clicked();
    void on_btnDeleteUnivers_clicked();
    void on_uiUniversList_currentRowChanged(int currentRow);

    void clearForm();

private:
    Ui::InterfacePcClient *ui;
    AccessBDD bdd;

    QList<UniversData>     universList;
    QList<EquipmentData>   equipmentsList;

    int currentEditEquipIndex = -1;
    int channelCounter        = 0;
    QLabel*      statusLabel;

    void refreshEquipmentsGrid();
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);
    void editEquipment(int index);
    void deleteEquipment(int index);
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
};
#endif // INTERFACEPCCLIENT_H
