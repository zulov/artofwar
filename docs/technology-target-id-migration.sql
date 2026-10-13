BEGIN TRANSACTION;

CREATE TABLE technology_level_effect_without_target_id (
	id INTEGER PRIMARY KEY,
	technology_level INTEGER NOT NULL,
	stat TEXT NOT NULL,
	operation TEXT NOT NULL,
	source_kind TEXT NOT NULL,
	source_tag TEXT NOT NULL,
	target_kind TEXT NOT NULL DEFAULT 'none',
	target_tag TEXT NOT NULL DEFAULT 'any',
	resource_type TEXT NOT NULL DEFAULT 'none',
	value REAL NOT NULL
) WITHOUT ROWID;

INSERT INTO technology_level_effect_without_target_id (
	id, technology_level, stat, operation, source_kind, source_tag,
	target_kind, target_tag, resource_type, value
)
SELECT
	id, technology_level, stat, operation, source_kind, source_tag,
	target_kind, target_tag, resource_type, value
FROM technology_level_effect;

DROP TABLE technology_level_effect;
ALTER TABLE technology_level_effect_without_target_id RENAME TO technology_level_effect;

COMMIT;
