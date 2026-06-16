/********************************************************************************
** Form generated from reading UI file 'dialogunivers.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGUNIVERS_H
#define UI_DIALOGUNIVERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogUnivers
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *labelNum;
    QLineEdit *numEdit;
    QLabel *labelIp;
    QLineEdit *ipEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogUnivers)
    {
        if (DialogUnivers->objectName().isEmpty())
            DialogUnivers->setObjectName("DialogUnivers");
        DialogUnivers->resize(450, 200);
        DialogUnivers->setMinimumSize(QSize(450, 0));
        DialogUnivers->setStyleSheet(QString::fromUtf8("background-color: #2b2b2b;"));
        verticalLayout = new QVBoxLayout(DialogUnivers);
        verticalLayout->setSpacing(20);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setHorizontalSpacing(15);
        formLayout->setVerticalSpacing(15);
        labelNum = new QLabel(DialogUnivers);
        labelNum->setObjectName("labelNum");
        labelNum->setStyleSheet(QString::fromUtf8("color: #cccccc; font-family: 'Segoe UI'; font-weight: bold;"));

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, labelNum);

        numEdit = new QLineEdit(DialogUnivers);
        numEdit->setObjectName("numEdit");
        numEdit->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, numEdit);

        labelIp = new QLabel(DialogUnivers);
        labelIp->setObjectName("labelIp");
        labelIp->setStyleSheet(QString::fromUtf8("color: #cccccc; font-family: 'Segoe UI'; font-weight: bold;"));

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, labelIp);

        ipEdit = new QLineEdit(DialogUnivers);
        ipEdit->setObjectName("ipEdit");
        ipEdit->setStyleSheet(QString::fromUtf8("background-color: #383838; border: 1px solid #555; color: #ffffff; padding: 8px; border-radius: 4px;"));

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, ipEdit);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(DialogUnivers);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStyleSheet(QString::fromUtf8("QPushButton { border-radius: 4px; padding: 8px 20px; font-weight: bold; color: white; }\n"
"QPushButton[text=\"OK\"] { background-color: #388e3c; }\n"
"QPushButton[text=\"OK\"]:hover { background-color: #4caf50; }\n"
"QPushButton[text=\"Cancel\"] { background-color: #555555; }\n"
"QPushButton[text=\"Cancel\"]:hover { background-color: #666666; }"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DialogUnivers);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, DialogUnivers, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, DialogUnivers, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(DialogUnivers);
    } // setupUi

    void retranslateUi(QDialog *DialogUnivers)
    {
        DialogUnivers->setWindowTitle(QCoreApplication::translate("DialogUnivers", "Nouvel Univers", nullptr));
        labelNum->setText(QCoreApplication::translate("DialogUnivers", "Num\303\251ro de l'univers:", nullptr));
        labelIp->setText(QCoreApplication::translate("DialogUnivers", "Adresse IP:", nullptr));
        ipEdit->setPlaceholderText(QCoreApplication::translate("DialogUnivers", "192.168.1.X", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogUnivers: public Ui_DialogUnivers {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGUNIVERS_H
