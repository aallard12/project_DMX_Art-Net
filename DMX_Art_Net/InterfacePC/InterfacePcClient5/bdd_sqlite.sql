-- =============================================
-- DMXBDD - Schema SQLite
-- Equivalent du dump MariaDB
-- =============================================

PRAGMA foreign_keys = ON;

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS UNIVERS (
    idUnivers     INTEGER PRIMARY KEY AUTOINCREMENT,
    numeroUnivers INTEGER,
    adresseIp     TEXT
);

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS EQUIPEMENTS (
    idEquipement  INTEGER PRIMARY KEY AUTOINCREMENT,
    nomEquipement TEXT,
    adresseDepart INTEGER,
    nbCanal       INTEGER,
    idUnivers     INTEGER NOT NULL,
    couleur       TEXT DEFAULT '#000000',
    FOREIGN KEY (idUnivers) REFERENCES UNIVERS(idUnivers)
);

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS CANAUX (
    idCanal       INTEGER PRIMARY KEY AUTOINCREMENT,
    numeroCanal   INTEGER,
    description   TEXT,
    idEquipement  INTEGER NOT NULL,
    FOREIGN KEY (idEquipement) REFERENCES EQUIPEMENTS(idEquipement) ON DELETE CASCADE
);

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS FONCTIONNALITE_CANAL (
    idFonctionnalite INTEGER PRIMARY KEY AUTOINCREMENT,
    valeurMin        INTEGER,
    valeurMax        INTEGER,
    fonction         TEXT,
    idCanal          INTEGER NOT NULL,
    FOREIGN KEY (idCanal) REFERENCES CANAUX(idCanal) ON DELETE CASCADE
);

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS SCENES (
    idScene  INTEGER PRIMARY KEY AUTOINCREMENT,
    nomScene TEXT
);

-- --------------------------------------------------------
CREATE TABLE IF NOT EXISTS PILOTE (
    idScene      INTEGER NOT NULL,
    idCanal      INTEGER NOT NULL,
    valeurCanaux INTEGER,
    PRIMARY KEY (idScene, idCanal),
    FOREIGN KEY (idScene) REFERENCES SCENES(idScene) ON DELETE CASCADE,
    FOREIGN KEY (idCanal) REFERENCES CANAUX(idCanal) ON DELETE CASCADE
);

-- =============================================
-- Données de test (identiques au dump MariaDB)
-- =============================================

INSERT INTO UNIVERS (idUnivers, numeroUnivers, adresseIp) VALUES
(1, 1, '192.168.1.31'),
(2, 2, '192.168.1.32'),
(3, 3, '192.168.1.33');

INSERT INTO EQUIPEMENTS (idEquipement, nomEquipement, adresseDepart, nbCanal, idUnivers, couleur) VALUES
(2, 'Projecteur Contest 1', 1, 4, 1, '#1c71d8'),
(3, 'Ruban Led',            3, 6, 1, '#57e389'),
(6, 'Projecteur Contest 2', 3, 4, 2, '#9141ac');

INSERT INTO CANAUX (idCanal, numeroCanal, description, idEquipement) VALUES
(131, 3, 'Rouge',     6),
(132, 4, 'Vert',      6),
(133, 5, 'Bleu',      6),
(134, 6, 'Intensité', 6),
(141, 1, 'Rouge',     2),
(142, 2, 'Vert',      2),
(143, 3, 'Bleu',      2),
(144, 4, 'Intensité', 2),
(145, 3, 'Rouge',     3),
(146, 4, 'Vert',      3),
(147, 5, 'Bleu',      3),
(148, 6, 'Zones',     3),
(149, 7, 'Mode',      3),
(150, 8, 'Effet',     3);

INSERT INTO FONCTIONNALITE_CANAL (idFonctionnalite, valeurMin, valeurMax, fonction, idCanal) VALUES
(274, 0,   255, 'Rouge',        131),
(275, 0,   255, 'Vert',         132),
(276, 0,   255, 'Bleu',         133),
(277, 0,   189, 'Intensité',    134),
(278, 190, 250, 'Strob',        134),
(279, 251, 255, 'ledFixe',      134),
(302, 0,   255, 'rouge',        141),
(303, 0,   255, 'Vert',         142),
(304, 0,   255, 'Bleu',         143),
(305, 0,   190, 'Intensité',    144),
(306, 191, 250, 'Strob',        144),
(307, 251, 255, 'ledFixe',      144),
(308, 0,   255, 'Rouge',        145),
(309, 0,   255, 'Vert',         146),
(310, 0,   255, 'Bleu',         147),
(311, 0,    49, 'Zone 1',       148),
(312, 50,   99, 'Zone 2',       148),
(313, 100, 149, 'Zone 3',       148),
(314, 150, 199, 'Zone 4',       148),
(315, 200, 249, 'MixZones',     148),
(316, 250, 255, 'AllZone',      148),
(317, 0,   149, 'Intensité',    149),
(318, 150, 249, 'Strob',        149),
(319, 250, 255, 'Rainbow',      149),
(320, 0,    25, 'EffetRainbow', 150),
(321, 26,   51, 'Chenillard',   150),
(322, 52,   76, 'Confetti',     150),
(323, 77,  102, 'Sinelon',      150),
(324, 103, 127, 'BPM',          150),
(325, 128, 153, 'Jungle',       150),
(326, 154, 179, 'Police',       150),
(327, 180, 204, 'WarpDrive',    150),
(328, 205, 230, 'Breathing',    150),
(329, 231, 255, 'Fire',         150);

INSERT INTO SCENES (idScene, nomScene) VALUES
(9,  'Strob'),
(10, 'Vert_WarpDrive'),
(13, 'QueDuBleu2'),
(14, 'QueDuVert');

INSERT INTO PILOTE (idScene, idCanal, valeurCanaux) VALUES
(9,  141, 255), (9,  142, 127), (9,  143, 127), (9,  144, 214),
(9,  145, 255), (9,  147, 255), (9,  148, 255), (9,  149, 223),
(10, 142, 255), (10, 143, 127), (10, 144, 255), (10, 148, 255),
(10, 149, 255), (10, 150, 190),
(13, 143, 255), (13, 144, 255), (13, 147, 255), (13, 148, 255), (13, 149, 147),
(14, 142, 255), (14, 144, 255), (14, 146, 255), (14, 148, 255), (14, 149, 149);