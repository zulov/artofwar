#pragma once
#include <string>

struct SQLConsts {

	inline const static std::string BASIC_COL =
		"id_db		INT NOT NULL,"
		"hp_coef	INT NOT NULL,";

	inline const static std::string PHYSICAL_COL =
		BASIC_COL
		+ "player		INT NOT NULL,"
		+ "level		INT NOT NULL,";

	inline const static std::string STATIC_COL =
		"bucket_x		INT NOT NULL,"
		"bucket_y		INT NOT NULL,"
		"state		INT NOT NULL,"
		"next_state	INT NOT NULL";

	inline const static std::string BUILDING_COL ="buildings(" +
		PHYSICAL_COL +
		STATIC_COL +
		",deploy_Idx		INT NOT NULL);";

	inline const static std::string RESOURCE_COL = "resource_entities(" +
		BASIC_COL + STATIC_COL + ");";

	inline const static std::string UNIT_COL = "units(" +
		PHYSICAL_COL +
		"position_x		INT NOT NULL,"
		"position_z		INT NOT NULL,"
		"state			INT NOT NULL,"
		"velocity_x		INT NOT NULL,"
		"velocity_z		INT NOT NULL);";

	inline const static std::string PLAYER_COL =
		"players("
		"id			INT NOT NULL,"
		"is_active	INT NOT NULL,"
		"team		INT NOT NULL,"
		"nation		INT NOT NULL,"
		"name		TEXT NOT NULL,"
		"color		INT NOT NULL);";

	inline const static std::string PLAYER_RESOURCES_COL =
		"resources("
		"player		INT NOT NULL,"
		"food		INT NOT NULL,"
		"wood		INT NOT NULL,"
		"stone		INT NOT NULL,"
		"gold		INT NOT NULL);";

	inline const static std::string CONFIG_COL ="config(" 
		"precision	INT NOT NULL,"
		"map		INT NOT NULL,"
		"size		INT NOT NULL);";

	inline const static std::string CREATE_TABLE = "CREATE TABLE ";

	inline const static std::string SELECT = "SELECT * from ";
	inline const static std::string COUNT = "SELECT count(*) from ";
};
