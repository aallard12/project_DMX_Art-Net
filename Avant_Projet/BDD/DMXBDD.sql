-- phpMyAdmin SQL Dump
-- version 5.2.2deb1+deb13u1
-- https://www.phpmyadmin.net/
--
-- Hôte : localhost:3306
-- Généré le : ven. 10 avr. 2026 à 13:38
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
(145, 3, 'Rouge', 3),
(146, 4, 'Vert', 3),
(147, 5, 'Bleu', 3),
(148, 6, 'Zones', 3),
(149, 7, 'Mode', 3),
(150, 8, 'Effet', 3),
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
(162, 8, 'Intensité', 15);

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
(15, 'proj contest 5', 5, 4, 3, '#b5835a');

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
(308, 0, 255, 'Rouge', 145),
(309, 0, 255, 'Vert', 146),
(310, 0, 255, 'Bleu', 147),
(311, 0, 49, 'Zone 1', 148),
(312, 50, 99, 'Zone 2', 148),
(313, 100, 149, 'Zone 3', 148),
(314, 150, 199, 'Zone 4', 148),
(315, 200, 249, 'MixZones', 148),
(316, 250, 255, 'AllZone', 148),
(317, 0, 149, 'Intensité', 149),
(318, 150, 249, 'Strob', 149),
(319, 250, 255, 'Rainbow', 149),
(320, 0, 25, 'EffetRainbow', 150),
(321, 26, 51, 'Chenillard', 150),
(322, 52, 76, 'Confetti', 150),
(323, 77, 102, 'Sinelon', 150),
(324, 103, 127, 'BPM', 150),
(325, 128, 153, 'Jungle', 150),
(326, 154, 179, 'Police', 150),
(327, 180, 204, 'WarpDrive', 150),
(328, 205, 230, 'Breathing', 150),
(329, 231, 255, 'Fire', 150),
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
(347, 251, 255, 'ledFixe', 162);

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
(9, 141, 255),
(9, 142, 127),
(9, 143, 127),
(9, 144, 214),
(9, 145, 255),
(9, 147, 255),
(9, 148, 255),
(9, 149, 223),
(10, 142, 255),
(10, 143, 127),
(10, 144, 255),
(10, 148, 255),
(10, 149, 255),
(10, 150, 190),
(13, 143, 255),
(13, 144, 255),
(13, 147, 255),
(13, 148, 255),
(13, 149, 147),
(14, 142, 255),
(14, 144, 255),
(14, 146, 255),
(14, 148, 255),
(14, 149, 149),
(15, 151, 255),
(15, 154, 255),
(15, 156, 255),
(15, 158, 255),
(15, 161, 255),
(15, 162, 255);

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
(9, 'Strob'),
(10, 'Vert_WarpDrive'),
(13, 'QueDuBleu2'),
(14, 'QueDuVert'),
(15, 'Univ3_3proj');

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
  MODIFY `idCanal` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=163;

--
-- AUTO_INCREMENT pour la table `EQUIPEMENTS`
--
ALTER TABLE `EQUIPEMENTS`
  MODIFY `idEquipement` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=16;

--
-- AUTO_INCREMENT pour la table `FONCTIONNALITE_CANAL`
--
ALTER TABLE `FONCTIONNALITE_CANAL`
  MODIFY `idFonctionnalite` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=348;

--
-- AUTO_INCREMENT pour la table `SCENES`
--
ALTER TABLE `SCENES`
  MODIFY `idScene` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=21;

--
-- AUTO_INCREMENT pour la table `UNIVERS`
--
ALTER TABLE `UNIVERS`
  MODIFY `idUnivers` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=40;

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
