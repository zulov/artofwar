BEGIN;

-- Stage adjacency now defines age progression; explicit edges are redundant.
DROP TABLE age_transition;

-- Convert separate age-condition definitions and their join table into rows
-- that carry the owning age directly.
ALTER TABLE age_condition RENAME TO old_age_condition_link;
ALTER TABLE "condition" RENAME TO old_age_condition;
CREATE TABLE age_condition (
	age_id INT NOT NULL,
	metric INT NOT NULL,
	target REAL NOT NULL,
	PRIMARY KEY(age_id, metric, target)
) WITHOUT ROWID;
INSERT INTO age_condition (age_id, metric, target)
SELECT link.age_id, condition.metric, condition.target
FROM old_age_condition_link AS link
JOIN old_age_condition AS condition ON condition.id = link.condition_id;
DROP TABLE old_age_condition_link;
DROP TABLE old_age_condition;

-- This migration converts the immediately previous normalized-map schema:
-- string-typed reusable mission_goal + map_goal/map_condition junction tables.
CREATE TEMP TABLE old_map_goals AS
SELECT mg.map_id, mg.goal_id, g.type, g.text_key, g.target_player, g.target_id, g.target_value, g.time_ticks
FROM map_goal mg JOIN mission_goal g ON g.id = mg.goal_id;
CREATE TEMP TABLE goal_id_map (old_id INT PRIMARY KEY, new_id INT NOT NULL);
INSERT INTO goal_id_map
SELECT goal_id, ROW_NUMBER() OVER (ORDER BY goal_id) - 1 FROM (SELECT DISTINCT goal_id FROM old_map_goals);
CREATE TEMP TABLE extra_goal_start (id INT NOT NULL);
INSERT INTO extra_goal_start SELECT COUNT(*) FROM goal_id_map;
CREATE TEMP TABLE map_goal_ids (map_id INT PRIMARY KEY, goal_ids TEXT NOT NULL);
INSERT INTO map_goal_ids
SELECT map_id, group_concat(new_id, ',') FROM (
	SELECT old_map_goals.map_id, goal_id_map.new_id
	FROM old_map_goals JOIN goal_id_map ON goal_id_map.old_id = old_map_goals.goal_id
	GROUP BY old_map_goals.map_id, goal_id_map.new_id
	ORDER BY old_map_goals.map_id, goal_id_map.new_id
) GROUP BY map_id;

ALTER TABLE map ADD COLUMN goal_ids TEXT NOT NULL DEFAULT '';
DROP TABLE map_goal;
DROP TABLE map_condition;
DROP TABLE mission_goal;

CREATE TABLE mission_goal (
	id INT PRIMARY KEY,
	type TEXT NOT NULL CHECK(type IN (
		'ELIMINATE_PLAYER', 'ELIMINATE_TEAM', 'ELIMINATE_ALL', 'ELIMINATE_UNITS',
		'ELIMINATE_BUILDINGS', 'SURVIVE_WITH_BUILDING_UNTIL', 'SURVIVE_UNTIL',
		'KILL_UNITS', 'SCORE_LEAD_AT_TIME', 'RESOURCE_STOCK', 'RESOURCE_GATHERED',
		'BUILDING_COUNT', 'SCORE_REACH'
	)),
	text_key TEXT NOT NULL,
	target_player INT NOT NULL DEFAULT -1,
	target_id INT NOT NULL DEFAULT -1,
	target_value REAL NOT NULL DEFAULT 0,
	time_ticks INT NOT NULL DEFAULT 0
) WITHOUT ROWID;

INSERT INTO mission_goal
SELECT ids.new_id, goals.type, goals.text_key, goals.target_player, goals.target_id,
	goals.target_value, goals.time_ticks
FROM old_map_goals goals JOIN goal_id_map ids ON ids.old_id = goals.goal_id
GROUP BY ids.new_id;
UPDATE map SET goal_ids = COALESCE((SELECT goal_ids FROM map_goal_ids WHERE map_id = map.id), '');
DROP TABLE old_map_goals;
DROP TABLE goal_id_map;
DROP TABLE map_goal_ids;

-- Examples of other goal types. These definitions are not assigned to maps.
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start), 'ELIMINATE_PLAYER', 'mission_eliminate_player', 1, -1, 0, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 1, 'ELIMINATE_TEAM', 'mission_eliminate_specific_team', 1, -1, 0, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 2, 'ELIMINATE_UNITS', 'mission_eliminate_units', -1, -1, 0, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 3, 'ELIMINATE_BUILDINGS', 'mission_eliminate_buildings', -1, -1, 0, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 4, 'SURVIVE_UNTIL', 'mission_survive_10', -1, -1, 0, 18000);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 5, 'KILL_UNITS', 'mission_kill_20', -1, -1, 20, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 6, 'SCORE_LEAD_AT_TIME', 'mission_score_lead', -1, -1, 0, 18000);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 7, 'RESOURCE_STOCK', 'mission_stock_500_gold', -1, 3, 500, 0);
INSERT INTO mission_goal VALUES ((SELECT id FROM extra_goal_start) + 8, 'SCORE_REACH', 'mission_score_1000', -1, -1, 1000, 0);
DROP TABLE extra_goal_start;

COMMIT;
