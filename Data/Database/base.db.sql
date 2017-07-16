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
	`rotatable`	INTEGER,
	FOREIGN KEY(`type`) REFERENCES `unit_type`(`id`)
);
INSERT INTO `units` (name,minDist,maxSep,type,model,texture,font,mass,maxSpeed,scale,rotatable) VALUES ('warrior',0.5,2.0,0,'Cube.mdl','grey.xml','Anonymous Pro.ttf',1.0,5.0,1.0,1),
 ('archer',0.4,2.0,3,'Hedra.mdl','darkgrey.xml','Anonymous Pro.ttf',0.8,6.0,1.0,1),
 ('pikeman',0.4,2.0,1,'Pyramid.mdl','red.xml','Anonymous Pro.ttf',0.9,3.0,1.0,1),
 ('cavalry',0.6,3.5,2,'horse.mdl','horse.xml','Anonymous Pro.ttf',2.0,12.0,1.0,1),
 ('worker',0.3,2.0,4,'Sphere.mdl','red.xml','Anonymous Pro.ttf',0.5,3.0,1.0,1);
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
INSERT INTO `resource` (id,name,icon,maxCapacity,texture,model,scale,minDist) VALUES (0,'gold','gold.png',1000,'gold.xml
','rock.mdl',0.75,3.0),
 (1,'wood','wood.png',100,'tree0.xml
tree1.xml','tree.mdl',1.75,5.0),
 (2,'food','food.png',100,'food.xml',NULL,1.0,4.0),
 (3,'stone','stone.png',1000,'stone.xml','rock.mdl',0.75,3.0);
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
	`param_names`	TEXT, 
	`param_values`	TEXT,
	PRIMARY KEY(`id`)
);
INSERT INTO `hud_size` (id,name,icon_size_x,icon_size_y,space_size_x,space_size_y,param_names,param_values) VALUES (0,'s',42,42,8,4,'\$iconX
\$iconY
\$spaceX
\$spaceY
\$bigX
\$bigY','42
42
8
4
420
236'),
 (1,'m',64,64,12,6,'\$iconX
\$iconY
\$spaceX
\$spaceY
\$bigX
\$bigY','64
64
12
6
640
360'),
 (2,'l',86,86,16,8,'\$iconX
\$iconY
\$spaceX
\$spaceY
\$bigX
\$bigY','86
86
16
8
860
483'),
 (3,'xl',128,128,24,12,'\$iconX
\$iconY
\$spaceX
\$spaceY
\$bigX
\$bigY','128
128
24
12
1280
720');
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
INSERT INTO `graph_settings` (id,hud_size,res_x,res_y,style,fullscreen,max_fps,min_fps) VALUES (0,1,1366,768,'DefaultStyle.xml','false',140.0,0.01);
CREATE TABLE `building_type` (
	`id`	INTEGER,
	`name`	TEXT,
	`icon`	TEXT,
	PRIMARY KEY(`id`)
);
INSERT INTO `building_type` (id,name,icon) VALUES (0,'house','house.png'),
 (1,'tower','tower.png'),
 (2,'barracks','house.png'),
 (3,'archery_range','house.png'),
 (4,'mock','mock.png');
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
 (3,'archery_range',6.0,3,'Barracks.mdl','barracks.xml','Anonymous Pro.ttf',1.0,'barracks_temp.xml'),
 (4,'mock',10.0,4,'cube.mdl','transparent.xml','Anonymous Pro.ttf',10.0,'transparent_temp.xml');
COMMIT;
