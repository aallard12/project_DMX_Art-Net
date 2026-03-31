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
    Ui::DialogUnivers *ui;
};

#endif // DIALOGUNIVERS_H
