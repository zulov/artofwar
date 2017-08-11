BEGIN TRANSACTION;
INSERT INTO `units` (name,minDist,maxSep,type,model,texture,font,mass,maxSpeed,scale,rotatable,nation,id,icon) VALUES 
 ('warrior',0.5,2.0,0,'Cube.mdl','grey.xml','Anonymous Pro.ttf',1.0,5.0,1.0,1,0,0,'warrior.png'),
 ('archer',0.4,2.0,3,'Hedra.mdl','darkgrey.xml','Anonymous Pro.ttf',0.8,6.0,1.0,1,0,1,'archer.png'),
 ('pikeman',0.4,2.0,1,'Pyramid.mdl','red.xml','Anonymous Pro.ttf',0.9,3.0,1.0,1,0,2,'pikeman.png'),
 ('cavalry',0.6,3.5,2,'horse.mdl','horse.xml','Anonymous Pro.ttf',2.0,12.0,1.0,1,0,3,'cavalry.png'),
 ('worker',0.3,2.0,4,'Sphere.mdl','red.xml','Anonymous Pro.ttf',0.5,3.0,1.0,1,0,4,'worker.png');
INSERT INTO `unit_type` (id,name) VALUES 
 (0,'warrior'),
 (1,'pikeman'),
 (2,'cavalry'),
 (3,'archer'),
 (4,'worker');
INSERT INTO `resource` (id,name,icon,maxCapacity,texture,model,scale,minDist) VALUES (0,'gold','gold.png',1000,'gold.xml
','rock.mdl',0.75,3.0),
 (1,'wood','wood.png',100,'tree0.xml
tree1.xml','tree.mdl',1.75,5.0),
 (2,'food','food.png',100,'food.xml',NULL,1.0,4.0),
 (3,'stone','stone.png',1000,'stone.xml','rock.mdl',0.75,3.0);
INSERT INTO `nation` (id,name) VALUES 
 (0,'basic'),
 (1,'enemy');
INSERT INTO `hud_size_vars` (id,hud_size,name,value) VALUES 
 (0,0,'iconX',42.0),
 (1,0,'iconY',42.0),
 (2,0,'spaceX',4.0),
 (3,0,'spaceY',3.0),
 (4,0,'bigX',960.0),
 (5,0,'bigY',540.0),
 (6,1,'iconX',64.0),
 (7,1,'iconY',64.0),
 (8,1,'spaceX',6.0),
 (9,1,'spaceY',4.0),
 (10,1,'bigX',1440.0),
 (11,1,'bigY',810.0),
 (12,2,'iconX',86.0),
 (13,2,'iconY',86.0),
 (14,2,'spaceX',4.0),
 (15,2,'spaceY',6.0),
 (16,2,'bigX',1920.0),
 (17,2,'bigY',1080.0),
 (18,3,'iconX',128.0),
 (19,3,'iconY',128.0),
 (20,3,'spaceX',12.0),
 (21,3,'spaceY',8.0),
 (22,3,'bigX',2880.0),
 (23,3,'bigY',1620.0),
 (24,0,'fontSize',10.0),
 (25,1,'fontSize',12.0),
 (26,2,'fontSize',14.0),
 (27,3,'fontSize',18.0);
INSERT INTO `hud_size` (id,name) VALUES 
 (0,'s'),
 (1,'m'),
 (2,'l'),
 (3,'xl');
INSERT INTO `graph_settings` (id,hud_size,res_x,res_y,style,fullscreen,max_fps,min_fps) VALUES 
 (0,0,1366,768,'DefaultStyle.xml',0,140.0,1.0);
INSERT INTO `building_type` (id,name) VALUES 
 (0,'house'),
 (1,'tower'),
 (2,'barracks'),
 (3,'archery_range'),
 (4,'mock');
INSERT INTO `building_to_unit` (id,building,unit) VALUES 
 (0,4,0),
 (1,2,0),
 (2,2,2),
 (3,3,1),
 (4,4,0),
 (5,4,1),
 (6,4,2),
 (7,4,3),
 (8,4,4),
 (9,4,1),
 (10,4,2),
 (11,4,3),
 (12,4,4);
INSERT INTO `building` (id,name,minDist,type,model,texture,font,scale,texture_temp,nation,icon) VALUES 
 (0,'house',6.0,0,'House.mdl','house.xml','Anonymous Pro.ttf',1.0,'house_temp.xml',0,'house.png'),
 (1,'tower',3.0,1,'Tower.mdl','tower.xml','Anonymous Pro.ttf',1.0,'tower_temp.xml',0,'tower.png'),
 (2,'barracks',10.0,2,'Barracks.mdl','barracks.xml','Anonymous Pro.ttf',1.0,'barracks_temp.xml',0,'barracks.png'),
 (3,'archery_range',6.0,3,'Barracks.mdl','barracks.xml','Anonymous Pro.ttf',1.0,'barracks_temp.xml',0,'archery_range.png'),
 (4,'mock',10.0,4,'cube.mdl','transparent.xml','Anonymous Pro.ttf',10.0,'transparent_temp.xml',0,'mock.png');
COMMIT;
