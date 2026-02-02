#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>   // Remplacé QListWidget
#include <QSlider>
#include <QLabel>
#include <QStatusBar>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void cas_configurerReseau();    // UC1
    void cas_composerScene();       // UC2
    void cas_confirmerSelection();  // Nouvelle étape de sécurité
    void cas_lancerScene();         // UC3
    void cas_obtenirListeScenes();  // UC4
    void réinitialiserSliders();
    void gestionReception();

private:
    void initialiserIHM();

    QTcpSocket  *socketClient;
    QTabWidget  *onglets;
    QLineEdit   *editIP;
    QLineEdit   *editNomScene;
    QLineEdit   *editNomEquipment;
    QSpinBox    *spinNbCanaux;
    QPushButton *buttonEnregistrer;
    QLineEdit   *editIPEquipment;

    QComboBox   *comboScenes;        // Nouveau sélecteur
    QPushButton *btnConfirmer;      // Bouton de validation
    QPushButton *btnPlay;           // Bouton de lancement
    QString      sceneValidee;      // Stocke le nom confirmé
};

#endif
