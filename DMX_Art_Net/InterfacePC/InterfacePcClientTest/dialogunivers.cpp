/**
 * @file dialogunivers.cpp
 * @brief Implémentation de la classe DialogUnivers
 * @author Antoine ALLARD
 * @date 10/04/2026
 * @details Classe représentant une boîte de dialogue permettant de saisir
 * le numéro d’un univers ainsi que son adresse IP, avec validation du format de l’IP.
 */

#include "dialogunivers.h"
#include "ui_dialogunivers.h"

/**
 * @brief DialogUnivers::DialogUnivers
 * @details Initialise l'interface utilisateur et configure la validation de la saisie
 * de l'adresse IP. Une expression régulière est utilisée pour restreindre la saisie
 * au réseau 192.168.1.X, avec un dernier octet compris entre 1 et 254.
 * @param parent Pointeur vers le widget parent.
 */
DialogUnivers::DialogUnivers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUnivers)
{
    ui->setupUi(this);
    QRegularExpression rx("^192\\.168\\.1\\.([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$");
    ui->ipEdit->setValidator(new QRegularExpressionValidator(rx, this));
    ui->ipEdit->setPlaceholderText("192.168.1.X");
}

/**
 * @brief DialogUnivers::~DialogUnivers
 * @details Libère les ressources mémoire allouées à l'interface utilisateur (ui) lors
 * de la destruction de l'objet.
 */
DialogUnivers::~DialogUnivers()
{
    delete ui;
}

/**
 * @brief DialogUnivers::getNumUnivers
 * @details Récupère la valeur saisie dans le champ d'édition du numéro d'univers
 * et la convertit en entier.
 * @return Le numéro de l'univers sous forme d'entier (int).
 */
int DialogUnivers::getNumUnivers()
{
    return ui->numEdit->text().toInt();
}

/**
 * @brief DialogUnivers::getIpUnivers
 * @details Récupère l'adresse IP saisie par l'utilisateur dans le champ de texte dédié.
 * @return L'adresse IP sous forme de chaîne de caractères (QString).
 */
QString DialogUnivers::getIpUnivers()
{
    return ui->ipEdit->text();
}
