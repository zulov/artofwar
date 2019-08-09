BEGIN TRANSACTION;
INSERT INTO `units` (id,name,rotatable,nation,icon,actionState) VALUES (0,'warrior',1,0,'warrior.png',5),
 (1,'archer',1,0,'archer.png',6),
 (2,'pikeman',1,0,'pikeman.png',5),
 (3,'cavalry',1,0,'cavalry.png',5),
 (4,'worker',0,0,'worker.png',9),
 (5,'general',1,0,'general.png',1);
INSERT INTO `unit_upgrade_path` (id,name) VALUES (0,'up_pth_attack'),
 (1,'up_pth_defense');
INSERT INTO `unit_upgrade_cost` (upgrade,resource,value) VALUES (0,0,1000),
 (0,1,1000),
 (1,2,500),
 (1,3,1500),
 (2,0,2500);
INSERT INTO `unit_upgrade` (id,path,level,name,attack,attackSpeed,attackRange,defense,maxHp,maxSpeed,minSpeed,collectSpeed,upgradeSpeed) VALUES (0,0,0,'up_attack_0',1.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,10.0),
 (1,1,0,'up_defense_0',1.0,1.0,1.0,1.2,1.25,1.0,1.0,1.0,5.0),
 (2,0,1,'up_attack_0',1.3,1.0,1.0,1.0,1.0,1.0,1.0,1.0,15.0);
INSERT INTO `unit_to_unit_upgrade` (upgrade,unit) VALUES (0,0),
 (0,1),
 (1,1),
 (1,2);
INSERT INTO `unit_level` (level,unit,name,minDist,maxSep,nodeName,mass,attack,attackSpeed,attackRange,defense,maxHp,maxSpeed,minSpeed,collectSpeed,upgrade_speed,max_force) VALUES 
 (0,0,'base',0.25,1.0,'warrior',100.0,2.0,4,1.0,0.2,100,5.0,1.0,0.0,2.0,5000.0),
 (1,0,'better',0.25,1.0,'warrior',100.0,3.0,4,1.0,0.3,150,5.5,1.0,0.0,2.0,5000.0),
 (0,1,'base',0.2,1.0,'archer',80.0,3.0,7,10.0,0.1,80,6.0,1.0,0.0,2.0,5000.0),
 (1,1,'better',0.2,1.0,'archer',80.0,4.0,7,10.0,0.15,100,7.0,1.0,0.0,3.0,5000.0),
 (0,2,'base',0.2,1.0,'pikeman',120.0,1.0,5,1.5,0.4,150,4.0,2.0,0.0,2.0,5000.0),
 (1,2,'better',0.2,1.0,'pikeman',120.0,1.0,5,1.5,0.5,180,4.2,1.7,0.0,3.0,5000.0),
 (0,3,'base',0.3,1.0,'cavalry',400.0,5.0,10,2.0,0.3,250,12.0,1.0,0.0,2.0,20000.0),
 (1,3,'better',0.3,1.0,'cavalry',400.0,6.0,10,2.0,0.4,300,13.0,1.0,0.0,3.0,12000.0),
 (0,4,'base',0.15,1.0,'worker',50.0,0.2,3,1.0,0.05,50,3.0,1.0,5.0,2.0,5000.0),
 (1,4,'better',0.15,1.0,'worker',50.0,0.2,3,1.0,0.05,60,4.0,1.0,6.0,2.0,5000.0),
 (0,5,'base',0.25,1.0,'general',50.0,0.5,3,1.0,0.05,70,5.0,1.0,0.0,3.0,5000.0),
 (1,5,'better',0.25,1.0,'general',50.0,0.6,3,1.0,0.06,80,5.0,1.0,0.0,3.0,5000.0);
INSERT INTO `settings` (graph,resolution) VALUES (0,1);
INSERT INTO `resource` (id,name,icon,maxCapacity,nodeName,sizeX,sizeZ,maxUsers,mini_map_color) VALUES (0,'gold','gold.png',1000,'gold.xml',2,2,4,'0xFF00D0FF'),
 (1,'wood','wood.png',100,'tree_1.xml
 tree_2.xml
 tree_3.xml',2,2,3,'0xFF00B600'),
 (2,'food','food.png',100,'food.xml',2,2,3,'0xFF9000A7'),
 (3,'stone','stone.png',1000,'stone.xml',2,2,4,'0xFF808080');
INSERT INTO `resolution` (id,x,y) VALUES (0,1366,768),
 (1,1600,900),
 (2,1920,1080),
 (3,2560,1440),
 (4,4096,2160);
INSERT INTO `player_colors` (id,unit,building,name) VALUES (0,'0xFFCF0000','0xFF900000','blue'),
 (1,'0xFF0000CF','0xFF000090','red');
INSERT INTO `orders_to_unit` (id,unit,'order') VALUES (0,0,0),
 (1,0,1),
 (2,0,3),
 (4,0,4),
 (5,0,5),
 (6,1,0),
 (7,1,1),
 (8,1,3),
 (10,1,4),
 (11,2,0),
 (12,2,1),
 (13,2,3),
 (15,2,4),
 (16,2,5),
 (17,3,0),
 (18,3,1),
 (19,3,2),
 (20,3,3),
 (22,4,0),
 (23,4,1),
 (25,4,7),
 (26,5,0),
 (27,5,1),
 (28,5,2),
 (29,5,3),
 (31,5,4),
 (32,5,5),
 (33,5,6),
 (34,5,7),
 (35,0,6),
 (36,1,6),
 (37,2,6),
 (38,3,6),
 (39,4,6);
INSERT INTO `orders` (id,icon,name) VALUES (0,'go.png','ord_go'),
 (1,'stop.png','ord_stop'),
 (2,'charge.png','ord_charge'),
 (3,'attack.png','ord_attack'),
 (4,'dead.png','ord_dead'),
 (5,'defend.png','ord_defend'),
 (6,'follow.png','ord_follow'),
 (7,'collect.png','ord_collect');
INSERT INTO `nation` (id,name) VALUES (0,'basic'),
 (1,'enemy');
INSERT INTO `map` (id,height_map,texture,scale_hor,scale_ver,name) VALUES (0,'map/HeightMap.png','Materials/StoneTiled.xml',2.0,0.1,'podstawowa'),
 (1,'map/test2.png','Materials/test.xml',2.0,0.1,'testowa');
INSERT INTO `hud_size_vars` (id,hud_size,name,value) VALUES (0,0,'iconX',42.0),
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
INSERT INTO `hud_size` (id,name) VALUES (0,'s'),
 (1,'m'),
 (2,'l'),
 (3,'xl');
INSERT INTO `graph_settings` (id,hud_size,style,fullscreen,max_fps,min_fps,name,v_sync,shadow,texture_quality) VALUES (0,0,'DefaultStyle.xml
in_game.xml
mini_map.xml
top.xml
loading.xml
selected.xml
queue.xml
main_menu.xml
left_menu.xml
',0,120.0,1.0,'custom',0,1,2),
 (1,1,'DefaultStyle.xml
in_game.xml
mini_map.xml
top.xml
loading.xml
selected.xml
queue.xml
main_menu.xml
left_menu.xml
score.xml
',1,120.0,1.0,'high',0,1,2);
INSERT INTO `cost_unit_level` (unit,level,resource,value) VALUES (0,1,1,1000),
 (0,1,2,500),
 (1,1,0,500),
 (1,1,1,500),
 (2,1,2,2000),
 (3,1,1,2000),
 (4,1,2,3000),
 (5,1,2,2500);
INSERT INTO `cost_unit` (id,resource,value,unit) VALUES (0,0,90,3),
 (1,1,120,0),
 (2,0,80,1),
 (3,1,130,2),
 (4,2,50,4),
 (5,1,30,1),
 (6,0,200,5);
INSERT INTO `cost_building_level` (building,level,resource,value) VALUES (0,1,0,1000),
 (0,1,1,500),
 (1,1,2,500),
 (1,1,0,2000),
 (2,1,1,3000),
 (2,1,2,250),
 (3,1,2,3000),
 (4,1,0,4000),
 (5,1,1,3500);
INSERT INTO `cost_building` (id,resource,value,building) VALUES (0,3,100,0),
 (1,1,50,0),
 (2,3,100,1),
 (3,1,100,2),
 (4,3,100,2),
 (5,1,150,3),
 (6,3,50,3),
 (7,1,100,4),
 (8,3,100,4),
 (9,0,100,4),
 (10,0,300,5);
INSERT INTO `building_to_unit_upgrade_path` (building,path) VALUES (0,0),
 (0,1),
 (1,0),
 (2,1),
 (3,1);
INSERT INTO `building_to_unit` (id,building,unit) VALUES (0,4,0),
 (1,2,0),
 (2,2,2),
 (3,3,1),
 (4,5,0),
 (5,4,1),
 (6,4,2),
 (7,4,3),
 (8,4,4),
 (9,5,1),
 (10,5,2),
 (11,5,3),
 (12,0,4),
 (13,4,5),
 (14,5,5);
INSERT INTO `building_level` (level,building,name,nodeName,queue_max_capacity) VALUES (0,0,'base','house2',5),
 (1,0,'better','house2',7),
 (0,1,'base','tower',5),
 (1,1,'better','tower',7),
 (0,2,'base','barracks',10),
 (1,2,'better','barracks',12),
 (0,3,'base','barracks',10),
 (1,3,'better','barracks',12),
 (0,4,'base','cube',20),
 (1,4,'better','cube',25),
 (0,5,'base','cube2',20),
 (1,5,'better','cube2',25);
INSERT INTO `building` (id,name,sizeX,sizeZ,nation,icon) VALUES (0,'house',2,2,0,'house.png'),
 (1,'tower',1,1,0,'tower.png'),
 (2,'barracks',2,2,0,'barracks.png'),
 (3,'archery_range',2,2,0,'archery_range.png'),
 (4,'mock',3,3,0,'mock.png'),
 (5,'mock_enemy',3,3,1,'mock2.png');
COMMIT;
