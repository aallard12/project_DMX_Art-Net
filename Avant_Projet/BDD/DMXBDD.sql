-- phpMyAdmin SQL Dump
-- version 5.2.2deb1+deb13u1
-- https://www.phpmyadmin.net/
--
-- Hôte : localhost:3306
-- Généré le : lun. 18 mai 2026 à 12:39
-- Version du serveur : 11.8.6-MariaDB-0+deb13u1 from Debian
-- Version de PHP : 8.4.16

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données : `DMXBDD`
--

-- --------------------------------------------------------

--
-- Structure de la table `CANAUX`
--

CREATE TABLE `CANAUX` (
  `idCanal` int(11) NOT NULL,
  `numeroCanal` smallint(6) DEFAULT NULL,
  `description` varchar(100) DEFAULT NULL,
  `idEquipement` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `CANAUX`
--

INSERT INTO `CANAUX` (`idCanal`, `numeroCanal`, `description`, `idEquipement`) VALUES
(131, 1, 'Rouge', 6),
(132, 2, 'Vert', 6),
(133, 3, 'Bleu', 6),
(134, 4, 'Intensité', 6),
(141, 1, 'Rouge', 2),
(142, 2, 'Vert', 2),
(143, 3, 'Bleu', 2),
(144, 4, 'Intensité', 2),
(151, 1, 'Rouge ', 13),
(152, 2, 'Vert', 13),
(153, 3, 'Bleu', 13),
(154, 4, 'Intensité', 13),
(155, 3, 'Rouge', 14),
(156, 4, 'Vert', 14),
(157, 5, 'Bleu', 14),
(158, 6, 'Intensité', 14),
(159, 5, 'Rouge', 15),
(160, 6, 'Vert', 15),
(161, 7, 'Bleu', 15),
(162, 8, 'Intensité', 15),
(201, 3, 'Rouge', 3),
(202, 4, 'Vert', 3),
(203, 5, 'Bleu', 3),
(204, 6, 'Zones', 3),
(205, 7, 'Mode', 3),
(206, 8, 'Effet', 3),
(235, 6, 'Pan', 46),
(236, 7, 'Tilt / Barrel', 46),
(237, 8, 'Shutter / Shaking', 46),
(238, 9, 'Gobo', 46),
(239, 10, 'Color', 46),
(240, 11, 'Gobo Rotation', 46),
(241, 12, 'Dimmer', 46);

-- --------------------------------------------------------

--
-- Structure de la table `EQUIPEMENTS`
--

CREATE TABLE `EQUIPEMENTS` (
  `idEquipement` int(11) NOT NULL,
  `nomEquipement` varchar(100) DEFAULT NULL,
  `adresseDepart` smallint(6) DEFAULT NULL,
  `nbCanal` smallint(6) DEFAULT NULL,
  `idUnivers` int(11) NOT NULL,
  `couleur` varchar(7) DEFAULT '#FFFFFF'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `EQUIPEMENTS`
--

INSERT INTO `EQUIPEMENTS` (`idEquipement`, `nomEquipement`, `adresseDepart`, `nbCanal`, `idUnivers`, `couleur`) VALUES
(2, 'Projecteur Contest 1', 1, 4, 1, '#1c71d8'),
(3, 'Ruban Led', 3, 6, 1, '#57e389'),
(6, 'Projecteur Contest 2', 1, 4, 2, '#9141ac'),
(13, 'proj 3', 1, 4, 3, '#f5c211'),
(14, 'proj contest 4', 3, 4, 3, '#ed333b'),
(15, 'proj contest 5', 5, 4, 3, '#b5835a'),
(46, 'iRock 7S/7B', 6, 7, 1, '#c01c28');

-- --------------------------------------------------------

--
-- Structure de la table `FONCTIONNALITE_CANAL`
--

CREATE TABLE `FONCTIONNALITE_CANAL` (
  `idFonctionnalite` int(11) NOT NULL,
  `valeurMin` int(11) DEFAULT NULL,
  `valeurMax` int(11) DEFAULT NULL,
  `fonction` varchar(100) DEFAULT NULL,
  `idCanal` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `FONCTIONNALITE_CANAL`
--

INSERT INTO `FONCTIONNALITE_CANAL` (`idFonctionnalite`, `valeurMin`, `valeurMax`, `fonction`, `idCanal`) VALUES
(274, 0, 255, 'Rouge', 131),
(275, 0, 255, 'Vert', 132),
(276, 0, 255, 'Bleu', 133),
(277, 0, 189, 'Intensité', 134),
(278, 190, 250, 'Strob', 134),
(279, 251, 255, 'ledFixe', 134),
(302, 0, 255, 'rouge', 141),
(303, 0, 255, 'Vert', 142),
(304, 0, 255, 'Bleu', 143),
(305, 0, 190, 'Intensité', 144),
(306, 191, 250, 'Strob', 144),
(307, 251, 255, 'ledFixe', 144),
(330, 0, 255, 'Rouge', 151),
(331, 0, 255, 'Vert', 152),
(332, 0, 255, 'Bleu', 153),
(333, 0, 190, 'Intensité', 154),
(334, 191, 250, 'Strob', 154),
(335, 251, 255, 'ledFixe', 154),
(336, 0, 255, 'Rouge', 155),
(337, 0, 255, 'Vert', 156),
(338, 0, 255, 'Bleu', 157),
(339, 0, 190, 'Intensité', 158),
(340, 191, 250, 'Strob', 158),
(341, 251, 255, 'ledFixe', 158),
(342, 0, 255, 'Rouge', 159),
(343, 0, 255, 'Vert', 160),
(344, 0, 255, 'Bleu', 161),
(345, 0, 190, 'Intensité', 162),
(346, 191, 250, 'Strob', 162),
(347, 251, 255, 'ledFixe', 162),
(424, 0, 255, 'Rouge', 201),
(425, 0, 255, 'Vert', 202),
(426, 0, 255, 'Bleu', 203),
(427, 0, 49, 'Zone 1', 204),
(428, 50, 99, 'Zone 2', 204),
(429, 100, 149, 'Zone 3', 204),
(430, 150, 199, 'Zone 4', 204),
(431, 200, 249, 'MixZones', 204),
(432, 250, 255, 'AllZone', 204),
(433, 0, 149, 'Intensité', 205),
(434, 150, 249, 'Strob', 205),
(435, 250, 255, 'Rainbow', 205),
(436, 0, 25, 'EffetRainbow', 206),
(437, 26, 51, 'Chenillard', 206),
(438, 52, 76, 'Confetti', 206),
(439, 77, 102, 'Sinelon', 206),
(440, 103, 127, 'BPM', 206),
(441, 128, 153, 'Juggle', 206),
(442, 154, 179, 'Police', 206),
(443, 180, 204, 'WarpDrive', 206),
(444, 205, 230, 'Breathing', 206),
(445, 231, 255, 'Feu', 206),
(626, 0, 255, 'Mouvement Horizontal', 235),
(627, 0, 9, 'Stopped (7B)', 236),
(628, 10, 10, 'Fastest speed counterclockwise (7B)', 236),
(629, 120, 120, 'Slowest speed counterclockwise (7B)', 236),
(630, 121, 134, 'Stopped (7B)', 236),
(631, 135, 135, 'Slowest speed clockwise (7B)', 236),
(632, 245, 245, 'Fastest speed clockwise (7B)', 236),
(633, 246, 255, 'Stopped (7B)', 236),
(634, 0, 7, 'Blackout', 237),
(635, 8, 15, 'Open', 237),
(636, 16, 16, 'Slowest speed shutter', 237),
(637, 131, 131, 'Fastest speed shutter', 237),
(638, 132, 132, 'Slowest speed shaking', 237),
(639, 247, 247, 'Fastest speed shaking', 237),
(640, 248, 255, 'Open', 237),
(641, 0, 15, 'Open / White', 238),
(642, 16, 31, 'Gobo 1', 238),
(643, 32, 47, 'Gobo 2', 238),
(644, 48, 63, 'Gobo 3', 238),
(645, 64, 79, 'Gobo 4', 238),
(646, 80, 95, 'Gobo 5', 238),
(647, 96, 111, 'Gobo 6', 238),
(648, 112, 127, 'Gobo 7', 238),
(649, 128, 128, 'Slowest speed Gobo change', 238),
(650, 255, 255, 'Fastest speed Gobo change', 238),
(651, 0, 12, 'White', 239),
(652, 13, 25, 'Green', 239),
(653, 26, 38, 'Magenta', 239),
(654, 39, 51, 'Light Blue', 239),
(655, 52, 63, 'Yellow', 239),
(656, 64, 76, 'Orange', 239),
(657, 77, 89, 'Blue', 239),
(658, 90, 102, 'UV Purple', 239),
(659, 103, 115, 'Light Green', 239),
(660, 116, 127, 'Pink', 239),
(661, 128, 128, 'Slowest speed Rainbow effect', 239),
(662, 255, 255, 'Fastest speed Rainbow effect', 239),
(663, 0, 9, 'Stopped', 240),
(664, 10, 10, 'Fastest speed counterclockwise', 240),
(665, 120, 120, 'Slowest speed counterclockwise', 240),
(666, 121, 134, 'Stopped', 240),
(667, 135, 135, 'Slowest speed clockwise', 240),
(668, 245, 245, 'Fastest speed clockwise', 240),
(669, 246, 255, 'Stopped', 240),
(670, 0, 255, 'Intensity 0-100%', 241);

-- --------------------------------------------------------

--
-- Structure de la table `PILOTE`
--

CREATE TABLE `PILOTE` (
  `idScene` int(11) NOT NULL,
  `idCanal` int(11) NOT NULL,
  `valeurCanaux` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `PILOTE`
--

INSERT INTO `PILOTE` (`idScene`, `idCanal`, `valeurCanaux`) VALUES
(15, 151, 255),
(15, 154, 255),
(15, 156, 255),
(15, 158, 255),
(15, 161, 255),
(15, 162, 255),
(21, 132, 255),
(21, 134, 255),
(37, 131, 255),
(37, 134, 187),
(38, 143, 255),
(38, 144, 255),
(38, 201, 127),
(38, 202, 127),
(38, 203, 255),
(38, 204, 255),
(38, 205, 97),
(39, 204, 255),
(39, 205, 255),
(39, 206, 164);

-- --------------------------------------------------------

--
-- Structure de la table `SCENES`
--

CREATE TABLE `SCENES` (
  `idScene` int(11) NOT NULL,
  `nomScene` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `SCENES`
--

INSERT INTO `SCENES` (`idScene`, `nomScene`) VALUES
(15, 'Univ3_3proj'),
(21, 'Uni2_Vert'),
(37, 'UNi2'),
(38, 'Uni1_projRuban'),
(39, 'Uni1_Police'),
(40, 'BLACKOUT');

-- --------------------------------------------------------

--
-- Structure de la table `UNIVERS`
--

CREATE TABLE `UNIVERS` (
  `idUnivers` int(11) NOT NULL,
  `numeroUnivers` smallint(6) DEFAULT NULL,
  `adresseIp` varchar(45) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_uca1400_ai_ci;

--
-- Déchargement des données de la table `UNIVERS`
--

INSERT INTO `UNIVERS` (`idUnivers`, `numeroUnivers`, `adresseIp`) VALUES
(1, 1, '192.168.1.31'),
(2, 2, '192.168.1.32'),
(3, 3, '192.168.1.33');

--
-- Index pour les tables déchargées
--

--
-- Index pour la table `CANAUX`
--
ALTER TABLE `CANAUX`
  ADD PRIMARY KEY (`idCanal`),
  ADD KEY `idEquipement` (`idEquipement`);

--
-- Index pour la table `EQUIPEMENTS`
--
ALTER TABLE `EQUIPEMENTS`
  ADD PRIMARY KEY (`idEquipement`),
  ADD KEY `idUnivers` (`idUnivers`);

--
-- Index pour la table `FONCTIONNALITE_CANAL`
--
ALTER TABLE `FONCTIONNALITE_CANAL`
  ADD PRIMARY KEY (`idFonctionnalite`),
  ADD KEY `idCanal` (`idCanal`);

--
-- Index pour la table `PILOTE`
--
ALTER TABLE `PILOTE`
  ADD PRIMARY KEY (`idScene`,`idCanal`),
  ADD KEY `fk_pilote_canal` (`idCanal`);

--
-- Index pour la table `SCENES`
--
ALTER TABLE `SCENES`
  ADD PRIMARY KEY (`idScene`);

--
-- Index pour la table `UNIVERS`
--
ALTER TABLE `UNIVERS`
  ADD PRIMARY KEY (`idUnivers`),
  ADD UNIQUE KEY `unique_numeroUnivers` (`numeroUnivers`),
  ADD UNIQUE KEY `unique_adresseIp` (`adresseIp`);

--
-- AUTO_INCREMENT pour les tables déchargées
--

--
-- AUTO_INCREMENT pour la table `CANAUX`
--
ALTER TABLE `CANAUX`
  MODIFY `idCanal` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=242;

--
-- AUTO_INCREMENT pour la table `EQUIPEMENTS`
--
ALTER TABLE `EQUIPEMENTS`
  MODIFY `idEquipement` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=47;

--
-- AUTO_INCREMENT pour la table `FONCTIONNALITE_CANAL`
--
ALTER TABLE `FONCTIONNALITE_CANAL`
  MODIFY `idFonctionnalite` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=671;

--
-- AUTO_INCREMENT pour la table `SCENES`
--
ALTER TABLE `SCENES`
  MODIFY `idScene` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=42;

--
-- AUTO_INCREMENT pour la table `UNIVERS`
--
ALTER TABLE `UNIVERS`
  MODIFY `idUnivers` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=99;

--
-- Contraintes pour les tables déchargées
--

--
-- Contraintes pour la table `CANAUX`
--
ALTER TABLE `CANAUX`
  ADD CONSTRAINT `CANAUX_ibfk_1` FOREIGN KEY (`idEquipement`) REFERENCES `EQUIPEMENTS` (`idEquipement`) ON DELETE CASCADE;

--
-- Contraintes pour la table `EQUIPEMENTS`
--
ALTER TABLE `EQUIPEMENTS`
  ADD CONSTRAINT `EQUIPEMENTS_ibfk_1` FOREIGN KEY (`idUnivers`) REFERENCES `UNIVERS` (`idUnivers`);

--
-- Contraintes pour la table `FONCTIONNALITE_CANAL`
--
ALTER TABLE `FONCTIONNALITE_CANAL`
  ADD CONSTRAINT `FONCTIONNALITE_CANAL_ibfk_1` FOREIGN KEY (`idCanal`) REFERENCES `CANAUX` (`idCanal`) ON DELETE CASCADE;

--
-- Contraintes pour la table `PILOTE`
--
ALTER TABLE `PILOTE`
  ADD CONSTRAINT `PILOTE_ibfk_1` FOREIGN KEY (`idScene`) REFERENCES `SCENES` (`idScene`) ON DELETE CASCADE,
  ADD CONSTRAINT `fk_pilote_canal` FOREIGN KEY (`idCanal`) REFERENCES `CANAUX` (`idCanal`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
