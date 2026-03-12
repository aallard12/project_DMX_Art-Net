#ifndef INTERFACEPCCLIENT_H
#define INTERFACEPCCLIENT_H

#include "accessbdd.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QComboBox>
#include <QListWidget>

// --- STRUCTURES DE DONNÉES (MCD) ---
struct UniversData {
    int numero;
    QString ip;
};

struct FunctionData {
    QString nom;
    QString min;
    QString max;
};

struct ChannelData {
    QString description;
    QList<FunctionData> fonctions;
};

struct EquipmentData {
    QString nom;
    QString univers;
    QString dmxStart;
    QList<ChannelData> canaux;
};
// -----------------------------------

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showAddForm();
    void showList();
    void addUnivers();
    void editUnivers();
    void deleteUnivers();
    void onUniversSelectionChanged();

    void saveEquipment();
    void clearForm();

private:
    void setupUi();
    void setupStyle();

    // Nouvelles méthodes pour gérer la dynamique des formulaires et de la grille
    void addChannelToForm(const ChannelData* data = nullptr);
    void addFunctionToChannel(QVBoxLayout* functionsLayout, const FunctionData* data = nullptr);
    void refreshUniversList();
    void refreshEquipmentsGrid();
    void editEquipment(int index);
    void deleteEquipment(int index);
    QFrame* createEquipmentCard(const EquipmentData& eq, int index);

    // Data
    QList<UniversData> universList;
    QList<EquipmentData> equipmentsList; // Stockage réel des équipements
    int currentEditEquipIndex;           // Index de l'équipement en cours de modification (-1 si nouveau)

    // Navigation
    QStackedWidget* stackedWidget;
    QWidget* listPage;
    QWidget* formPage;

    // List Page Elements
    QListWidget* uiUniversList;
    QPushButton* btnEditUnivers;
    QPushButton* btnDeleteUnivers;
    QGridLayout* equipmentsGrid;
    QWidget* equipmentsContainer;

    // Form Page Elements
    QLineEdit* nameEdit;
    QComboBox* universCombo;
    QLineEdit* startAddressEdit;
    QLabel* channelCountLabel;
    QVBoxLayout* channelsFormLayout;

    int channelCounter;
    AccessBDD bdd;

};

#endif // INTERFACEPCCLIENT_H
