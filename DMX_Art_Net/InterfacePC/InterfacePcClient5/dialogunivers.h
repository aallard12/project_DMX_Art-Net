/**
 * @file dialogunivers.h
 * @brief Déclaration de la classe DialogUnivers
 * @author Antoine ALLARD
 * @date 10/04/2026
 */

#ifndef DIALOGUNIVERS_H
#define DIALOGUNIVERS_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace Ui {
class DialogUnivers;
}

class DialogUnivers : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUnivers(QWidget *parent = nullptr);
    ~DialogUnivers();
    int getNumUnivers();
    QString getIpUnivers();

private:
    /// Pointeur vers l'interface utilisateur graphique de la boîte de dialogue
    Ui::DialogUnivers *ui;
};

#endif // DIALOGUNIVERS_H
