CREATE TABLE `CACAO`.`Word2Cat_ClassificationSystem` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `Name` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`ID`)
)
ENGINE = InnoDB;

CREATE TABLE `CACAO`.`Word2Cat_ClassificationCategory` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `ClassificationSystemID` INTEGER UNSIGNED NOT NULL,
  `CategoryID` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`ID`),
  INDEX `Index_2`(`ClassificationSystemID`),
  INDEX `Index_3`(`CategoryID`)
)
ENGINE = InnoDB;

CREATE TABLE `CACAO`.`Word2Cat_Terms` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `Term` VARCHAR(80) NOT NULL,
  `POS` VARCHAR(10) NOT NULL,
  `language` CHAR(2) NOT NULL,
  PRIMARY KEY (`ID`),
  INDEX `Index_2`(`Term`),
  INDEX `Index_3`(`POS`),
  INDEX `Index_4`(`language`)
)
ENGINE = InnoDB;

CREATE TABLE `CACAO`.`Word2Cat_MapResource` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `TermID` INTEGER UNSIGNED NOT NULL,
  `CategoryID` INTEGER UNSIGNED NOT NULL,
  `MappingID` INTEGER UNSIGNED NOT NULL,
  PRIMARY KEY (`ID`),
  INDEX `Index_2`(`TermID`),
  INDEX `Index_3`(`CategoryID`),
  INDEX `Index_4`(`MappingID`)
)
ENGINE = InnoDB;

CREATE TABLE `CACAO`.`Word2Cat_Mappings` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `Name` VARCHAR(45) NOT NULL,
  `TaskDescription` VARCHAR(150) NOT NULL,
  `Date` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`ID`),
  INDEX `Index_2`(`Name`)
)
ENGINE = InnoDB;

CREATE TABLE `CACAO`.`Word2Cat_ClassificationCategoriesLabels` (
  `ID` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `ClassificationCategoryID` INTEGER UNSIGNED NOT NULL,
  `lang` CHAR(2) NOT NULL,
  `label` VARCHAR(150) NOT NULL,
  PRIMARY KEY (`ID`),
  INDEX `Index_2`(`ClassificationCategoryID`)
)
ENGINE = InnoDB;
