BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS `units` (
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
	`actionState`	INTEGER,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`),
	FOREIGN KEY(`type`) REFERENCES `unit_type`(`id`)
);
CREATE TABLE IF NOT EXISTS `unit_type` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
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
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`),
	FOREIGN KEY(`order`) REFERENCES `orders`(`id`),
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
	`res_x`	INTEGER,
	`res_y`	INTEGER,
	`style`	INTEGER,
	`fullscreen`	INTEGER,
	`max_fps`	REAL,
	`min_fps`	REAL,
	FOREIGN KEY(`hud_size`) REFERENCES `hud_size`(`id`),
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_unit` (
	`id`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	`unit`	INTEGER,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`)
);
CREATE TABLE IF NOT EXISTS `cost_building` (
	`id`	INTEGER,
	`resource`	INTEGER,
	`value`	INTEGER,
	`building`	INTEGER,
	FOREIGN KEY(`building`) REFERENCES `building`(`id`),
	PRIMARY KEY(`id`),
	FOREIGN KEY(`resource`) REFERENCES `resource`(`id`)
);
CREATE TABLE IF NOT EXISTS `building_type` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `building_to_unit` (
	`id`	INTEGER,
	`building`	INTEGER,
	`unit`	INTEGER,
	FOREIGN KEY(`building`) REFERENCES `building`(`id`),
	FOREIGN KEY(`unit`) REFERENCES `units`(`id`),
	PRIMARY KEY(`id`)
);
CREATE TABLE IF NOT EXISTS `building` (
	`id`	INTEGER,
	`name`	TEXT,
	`sizeX`	INTEGER,
	`sizeZ`	INTEGER,
	`type`	INTEGER,
	`model`	TEXT,
	`texture`	TEXT,
	`font`	TEXT,
	`scale`	REAL,
	`texture_temp`	TEXT,
	`nation`	INTEGER,
	`icon`	TEXT,
	`queue_max_capacity`	INTEGER,
	PRIMARY KEY(`id`),
	FOREIGN KEY(`type`) REFERENCES `building_type`(`id`),
	FOREIGN KEY(`nation`) REFERENCES `nation`(`id`)
);
COMMIT;
