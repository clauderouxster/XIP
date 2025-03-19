-- MySQL dump 10.9
--
-- Host: localhost    Database: CacaoProject
-- ------------------------------------------------------
-- Server version       4.0.20-standard
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Collections`
--

DROP TABLE IF EXISTS `Collections`;
CREATE TABLE `Collections` (
  `ID` int(11) NOT NULL auto_increment,
  `library_id` int(11) default NULL,
  `description` varchar(255) default NULL,
  `code` varchar(255) default NULL,
  `lastupdate` datetime default NULL,
  PRIMARY KEY  (`ID`),
  KEY `collections_library_id` (`library_id`),
  KEY `collections_code` (`code`),
  KEY `collections_lastupdate` (`lastupdate`)
) TYPE=MyISAM;

--
-- Table structure for table `Libraries`
--

DROP TABLE IF EXISTS `Libraries`;
CREATE TABLE `Libraries` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(128) default NULL,
  `languages` varchar(255) default NULL,
  `url` varchar(255) default NULL,
  `protocol` varchar(32) default NULL,
  PRIMARY KEY  (`ID`),
  KEY `libraries_name` (`name`)
) TYPE=MyISAM;

--
-- Table structure for table `MetaData`
--

DROP TABLE IF EXISTS `MetaData`;
CREATE TABLE `MetaData` (
  `ID` int(11) NOT NULL auto_increment,
  `record_id` int(11) default NULL,
  `title` mediumtext,
  `creator` mediumtext,
  `subject` mediumtext,
  `description` mediumtext,
  `publisher` mediumtext,
  `contributor` mediumtext,
  `date` mediumtext,
  `type` mediumtext,
  `format` mediumtext,
  `identifier` mediumtext,
  `source` mediumtext,
  `language` mediumtext,
  `relation` mediumtext,
  `coverage` mediumtext,
  `rights` mediumtext,
  `subjectAttributes` mediumtext,
  `edition` mediumtext,
  `audience` mediumtext,
  `location` mediumtext,
  `recordId` mediumtext,
  PRIMARY KEY  (`ID`),
  KEY `metadata_record_id` (`record_id`)
) TYPE=MyISAM;

--
-- Table structure for table `Records`
--

DROP TABLE IF EXISTS `Records`;
CREATE TABLE `Records` (
  `ID` int(11) NOT NULL auto_increment,
  `library_id` int(11) default NULL,
  `collection_id` int(11) default NULL,
  `dublincoredata` mediumtext,
  `telapdata` mediumtext,
  `lastupdate` datetime default NULL,
  `deleted` int(11) default '0',
  `record_id` varchar(255) default NULL,
  `multiCollection` varchar(255) default NULL,
  `skipped` int(11) default '0',
  PRIMARY KEY  (`ID`),
  KEY `records_library_id` (`library_id`),
  KEY `records_collection_id` (`collection_id`),
  KEY `records_lastupdate` (`lastupdate`),
  KEY `recordIdIdx` (`record_id`)
) TYPE=MyISAM;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

