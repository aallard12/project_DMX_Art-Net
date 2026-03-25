#include <gtest/gtest.h>
#include <QApplication> // Ajoute cet include

int main(int argc, char *argv[])
{
    // On crée une QApplication (nécessaire pour QMessageBox)
    QApplication a(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
