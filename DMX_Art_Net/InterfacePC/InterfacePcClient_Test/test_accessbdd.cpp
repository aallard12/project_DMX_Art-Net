#include <gtest/gtest.h>
#include "accessbdd.h"

#include "/usr/lib/python3/dist-packages/mypyc/external/googletest/src/gtest-all.cc"

// 1. La classe de configuration (Fixture)
class AccessBDDTest : public ::testing::Test {
protected:
    static AccessBDD* access;

    // Se lance une seule fois pour tous les tests
    static void SetUpTestSuite() {
        access = new AccessBDD();
    }

    // Se lance une seule fois à la fin
    static void TearDownTestSuite() {
        delete access;
    }

    // Se lance avant CHAQUE test
    void SetUp() override {
        // Optionnel : nettoyer ta BDD ici si besoin
    }
};

// 2. Initialisation obligatoire du membre statique (UNE SEULE FOIS)
AccessBDD* AccessBDDTest::access = nullptr;

// 3. Tes tests (Chaque nom de test doit être UNIQUE)
TEST_F(AccessBDDTest, CRUD_Univers) {
    bool okInsert = access->enregistrerUnivers(999, "TEST_192.168.1.100");
    ASSERT_TRUE(okInsert) << "L'insertion de l'univers a échoué.";

    // ... la suite de ton code de test ...
}

TEST_F(AccessBDDTest, ContrainteCleEtrangere) {
    // Un autre test différent
}
