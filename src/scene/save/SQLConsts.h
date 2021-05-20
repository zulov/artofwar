#pragma once
#include <string>

struct SQLConsts {

	inline const static std::string PHYSICAL_COL =
		"id_db		INT NOT NULL,"
		"hp_coef	INT NOT NULL,"
		"player		INT NOT NULL,"
		"level		INT NOT NULL,";

	inline const static std::string STATIC_COL =
		PHYSICAL_COL
		+ "bucket_x		INT NOT NULL,"
		+ "bucket_y		INT NOT NULL,"
		+ "state		INT NOT NULL,"
		+ "next_state	INT NOT NULL,";

	inline const static std::string BUILDING_COL =
		STATIC_COL +
		"deploy_Idx		INT NOT NULL";
	inline const static std::string RESOURCE_COL = STATIC_COL;
	inline const static std::string UNIT_COL =
		PHYSICAL_COL +
		"position_x		INT NOT NULL,"
		"position_z		INT NOT NULL,"
		"state			INT NOT NULL,"
		"velocity_x		INT NOT NULL,"
		"velocity_z		INT NOT NULL";

	inline const static std::string PLAYER_COL =
		"id			INT NOT NULL,"
		"is_active	INT NOT NULL,"
		"team		INT NOT NULL,"
		"nation		INT NOT NULL,"
		"name		TEXT NOT NULL,"
		"color		INT NOT NULL";

	inline const static std::string PLAYER_RESOURCES_COL =
		"player		INT NOT NULL,"
		"resource	INT NOT NULL,"
		"amount		INT NOT NULL";

	inline const static std::string CONFIG_COL =
		"precision	INT NOT NULL,"
		"map		INT NOT NULL,"
		"size		INT NOT NULL";

	inline const static std::string CREATE_TABLE = "CREATE TABLE ";

	inline const static std::string SELECT = "SELECT * from ";
};
