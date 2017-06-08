BEGIN TRANSACTION;
CREATE TABLE "units" (
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
	FOREIGN KEY(`type`) REFERENCES `unit_type`(`id`)
);
INSERT INTO `units` (name,minDist,maxSep,type,model,texture,font,mass,maxSpeed,scale) VALUES ('warrior',0.5,2.0,0,'Cube.mdl','grey.xml','Anonymous Pro.ttf',1.0,5.0,1.0),
 ('archer',0.4,2.0,3,'Prism.mdl','darkgrey.xml','Anonymous Pro.ttf',0.8,6.0,1.0),
 ('pikeman',0.4,2.0,1,'Pyramid.mdl','red.xml','Anonymous Pro.ttf',0.9,3.0,1.0),
 ('cavalry',0.6,3.5,2,'Cylinder.mdl','red.xml','Anonymous Pro.ttf',2.0,12.0,1.0),
 ('worker',0.3,2.0,4,'Sphere.mdl','red.xml','Anonymous Pro.ttf',0.5,3.0,1.0);
CREATE TABLE "unit_type" (
	`id`	INTEGER,
	`name`	TEXT,
	`icon`	TEXT,
	PRIMARY KEY(`id`)
);
INSERT INTO `unit_type` (id,name,icon) VALUES (0,'warrior','warrior.png'),
 (1,'pikeman','spear.png'),
 (2,'cavalry','cavalry.png'),
 (3,'archer','bow.png'),
 (4,'worker','hammer.png');
CREATE TABLE `nation` (
	`id`	INTEGER,
	`name`	TEXT,
	PRIMARY KEY(`id`)
);
INSERT INTO `nation` (id,name) VALUES (0,NULL);
CREATE TABLE "hud_size" (
	`id`	INTEGER,
	`name`	TEXT,
	`icon_size_x`	INTEGER,
	`icon_size_y`	INTEGER,
	`space_size_x`	INTEGER,
	`space_size_y`	INTEGER,
	PRIMARY KEY(`id`)
);
INSERT INTO `hud_size` (id,name,icon_size_x,icon_size_y,space_size_x,space_size_y) VALUES (0,'s',42,42,8,4),
 (1,'m',64,64,12,6),
 (2,'l',86,86,16,8),
 (3,'xl',128,128,24,12);
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
INSERT INTO `graph_settings` (id,hud_size,res_x,res_y,style,fullscreen,max_fps,min_fps) VALUES (0,1,1366,768,'DefaultStyle.xml','false',10000.0,0.01);
CREATE TABLE `building_type` (
	`id`	INTEGER,
	`name`	TEXT,
	`icon`	TEXT,
	PRIMARY KEY(`id`)
);
INSERT INTO `building_type` (id,name,icon) VALUES (0,'house','house.png'),
 (1,'tower','tower.png'),
 (2,'barracks','house.png'),
 (3,'archery_range','house.png');
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
	PRIMARY KEY(`id`),
	FOREIGN KEY(`type`) REFERENCES `building_type`(`id`)
);
INSERT INTO `building` (id,name,minDist,type,model,texture,font,scale,texture_temp) VALUES (0,'house',6.0,0,'House.mdl','house.xml','Anonymous Pro.ttf',1.0,'house_temp.xml'),
 (1,'tower',3.0,1,'Tower.mdl','tower.xml','Anonymous Pro.ttf',1.0,'tower_temp.xml'),
 (2,'barracks',10.0,2,'Barracks.mdl','barracks.xml','Anonymous Pro.ttf',1.0,'barracks_temp.xml'),
 (3,'archery_range',6.0,3,'Barracks.mdl','barracks.xml','Anonymous Pro.ttf',1.0,'barracks_temp.xml');
COMMIT;
