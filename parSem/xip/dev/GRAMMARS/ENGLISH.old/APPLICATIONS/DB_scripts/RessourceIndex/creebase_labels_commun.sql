#Base de données classes sur le serveur localhost 
# phpMyAdmin SQL Dump
# version 2.5.3
# http://www.phpmyadmin.net
#
# Serveur: localhost
# Version du serveur: 4.0.15
# Version de PHP: 4.3.3
# 
# Base de données: `gui`
# 

# --------------------------------------------------------


CREATE TABLE `entites` (
  `NoEntite` bigint(20) unsigned NOT NULL auto_increment,
  `Texte` varchar(100) BINARY NOT NULL default '',
  `Frequence` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`NoEntite`),
  KEY `Texte` (`Texte`)
); 
#TYPE=MyISAM AUTO_INCREMENT=1 ;


CREATE TABLE `labels` (
  `NoLabel` bigint(20) unsigned NOT NULL auto_increment,
  `Texte` varchar(100) BINARY NOT NULL default '',
  PRIMARY KEY  (`NoLabel`),
  KEY `Texte` (`Texte`)
);
# TYPE=MyISAM AUTO_INCREMENT=1 ;


CREATE TABLE `dependances` (
  `NoDependance` bigint(20) unsigned NOT NULL auto_increment,
  `Dependance` varchar(100) BINARY NOT NULL default '',
  `TraitDep` varchar(100) BINARY NOT NULL default '',
  `Valeur` varchar(100) BINARY NOT NULL default '',
  `Frequence` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`NoDependance`),
  KEY `NoDependance` (`NoDependance`)
);
# TYPE=MyISAM AUTO_INCREMENT=1 ;


CREATE TABLE `relations` (
  `NoRelation` bigint(20) unsigned NOT NULL auto_increment,
  `NoLabel` bigint(20) unsigned NOT NULL default '0',
  `NoEntite` bigint(20) unsigned NOT NULL default '0',
  `NoDependance` bigint(20) unsigned NOT NULL default '0',
  `Frequence` bigint(20) unsigned NOT NULL default '0',
  `Fichier` varchar(100) BINARY NOT NULL default '',
  `IdSentence` bigint(20) unsigned NOT NULL default '0',
#  `IdNodeEN` bigint(20) unsigned NOT NULL default '0',
#  `IdNodeX` bigint(20) unsigned NOT NULL default '0',
  `LeftEN` bigint(20) unsigned NOT NULL default '0',
  `RightEN` bigint(20) unsigned NOT NULL default '0',
#  `LeftX` bigint(20) unsigned NOT NULL default '0',
#  `RightX` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`NoRelation`),
  KEY `NoRelation` (`NoRelation`,`NoLabel`,`NoEntite`)
);
# TYPE=MyISAM AUTO_INCREMENT=1 ;
