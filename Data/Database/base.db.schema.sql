BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS `units` (
	`id`	INTEGER,
	`name`	TEXT,
	`rotatable`	INTEGER,
	`nation`	INTEGER,
	`icon`	TEXT,
	`actionState`	INTEGER,
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`),
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `unit_level` (
	`level`	INTEGER,
	`unit`	INTEGER,
	`name`	TEXT,
	`minDist`	REAL,
	`maxSep`	REAL,
	`model`	TEXT,
	`texture`	TEXT,
	`mass`	REAL,
	`scale`	REAL,
	`attack`	REAL,
	`attackSpeed`	INTEGER,
	`attackRange`	REAL,
	`defense`	REAL,
	`maxHp`	INTEGER,
	`maxSpeed`	REAL,
	`minSpeed`	REAL,
	`collectSpeed`	REAL,
	`upgrade_speed`	real,
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`)
);
CREATE TABLE IF NOT EXISTS `settings` (
	`graph`	INTEGER,
	`resolution`	INTEGER,
	FOREIGN KEY(`graph`) REFERENCES `graph_settings`(`id`),
	FOREIGN KEY(`resolution`) REFERENCES `resolution`(`id`)
);
CREATE TABLE IF NOT EXISTS `resource` (
	`id`	INTEGER,
	`name`	TEXT,
	`icon`	TEXT,
	`maxCapacity`	INTEGER,
	`texture`	TEXT,
	`model`	TEXT,
	`scale`	REAL,
	`sizeX`	INTEGER,
	`sizeZ`	INTEGER,
	`maxUsers`	INTEGER,
	`mini_map_color`	INTEGER,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `resolution` (
	`id`	INTEGER,
	`x`	INTEGER,
	`y`	INTEGER,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `player_colors` (
	`id`	INTEGER,
	`unit`	INTEGER,
	`building`	INTEGER,
	`name`	text,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `orders_to_unit` (
	`id`	INTEGER,
	`unit`	INTEGER,
	`order`	INTEGER,
	FOREIGN KEY(`order`) REFERENCES `orders`(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`),
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `orders` (
	`id`	INTEGER,
	`icon`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `nation` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `map` (
	`id`	INTEGER,
	`height_map`	TEXT,
	`texture`	TEXT,
	`scale_hor`	REAL,
	`scale_ver`	REAL,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `hud_size_vars` (
	`id`	INTEGER,
	`hud_size`	INTEGER,
	`name`	TEXT,
	`value`	REAL,
	FOREIGN KEY(`hud_size`) REFERENCES `hud_size`(`id`)
);
CREATE TABLE IF NOT EXISTS `hud_size` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `graph_settings` (
	`id`	INTEGER,
	`hud_size`	INTEGER,
	`style`	INTEGER,
	`fullscreen`	INTEGER,
	`max_fps`	REAL,
	`min_fps`	REAL,
	`name`	TEXT,
	`v_sync`	INT,
	`shadow`	INT,
	`texture_quality`	INT,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`hud_size`) REFERENCES `hud_size`(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_unit_level` (
	`unit`	INTEGER,
	`level`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	FOREIGN KEY(`level`) REFERENCES `unit_level`(`level`),
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_unit` (
	`id`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	`unit`	INTEGER,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`),
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_building_level` (
	`building`	INTEGER,
	`level`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`),
	FOREIGN KEY(`building`) REFERENCES `building`(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_building` (
	`id`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	`building`	INTEGER,
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`),
	PRIMARY KEY(`id`),
	FOREIGN KEY(`building`) REFERENCES `building`(`id`)
);
CREATE TABLE IF NOT EXISTS `building_to_unit` (
	`id`	INTEGER,
	`building`	INTEGER,
	`unit`	INTEGER,
	FOREIGN KEY(`building`) REFERENCES `building`(`id`),
	PRIMARY KEY(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`)
);
CREATE TABLE IF NOT EXISTS `building_level` (
	`level`	INTEGER,
	`building`	INTEGER,
	`name`	TEXT,
	`model`	TEXT,
	`texture`	TEXT,
	`scale`	REAL,
	`queue_max_capacity`	INTEGER,
	FOREIGN KEY(`building`) REFERENCES `building`(`id`)
);
CREATE TABLE IF NOT EXISTS `building` (
	`id`	INTEGER,
	`name`	TEXT,
	`sizeX`	INTEGER,
	`sizeZ`	INTEGER,
	`nation`	INTEGER,
	`icon`	TEXT,
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`),
	PRIMARY KEY(`id`)
);
COMMIT;
