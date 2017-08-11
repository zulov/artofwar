BEGIN TRANSACTION;
CREATE TABLE "units" (
	`id`	INTEGER, 
	`name`	TEXT,
	`minDist`	REAL,
	`maxSep`	REAL,
	`type`	INTEGER,
	`model`	TEXT,
	`texture`	TEXT,
	`font`	TEXT,
	`mass`	REAL,
	`maxSpeed`	REAL,
	`scale`	REAL,
	`rotatable`	INTEGER,
	`nation`	INTEGER,
	`icon`	TEXT,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`type`) REFERENCES `unit_type`(`id`),
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`)
);
CREATE TABLE "unit_type" (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE "resource" (
	`id`	INTEGER,
	`name`	TEXT,
	`icon`	TEXT,
	`maxCapacity`	INTEGER,
	`texture`	TEXT,
	`model`	TEXT,
	`scale`	REAL,
	`minDist`	REAL,
	PRIMARY KEY(`id`)
);
CREATE TABLE `nation` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE "hud_size_vars" (
	`id`	INTEGER,
	`hud_size`	INTEGER,
	`name`	TEXT,
	`value`	REAL,
	FOREIGN KEY(`hud_size`) REFERENCES `hud_size`(`id`)
);
CREATE TABLE "hud_size" (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE "graph_settings" (
	`id`	INTEGER,
	`hud_size`	INTEGER,
	`res_x`	INTEGER,
	`res_y`	INTEGER,
	`style`	INTEGER,
	`fullscreen`	INTEGER,
	`max_fps`	REAL,
	`min_fps`	REAL,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`hud_size`) REFERENCES `hud_size`(`id`)
);
CREATE TABLE `building_type` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE `building_to_unit` (
	`id`	INTEGER,
	`building`	INTEGER,
	`unit`	INTEGER,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`building`) REFERENCES `building`(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`)
);
CREATE TABLE "building" (
	`id`	INTEGER,
	`name`	TEXT,
	`minDist`	REAL,
	`type`	INTEGER,
	`model`	TEXT,
	`texture`	TEXT,
	`font`	TEXT,
	`scale`	REAL,
	`texture_temp`	TEXT,
	`nation`	INTEGER,
	`icon`	TEXT,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`type`) REFERENCES `building_type`(`id`),
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`)
);
COMMIT;