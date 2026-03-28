#pragma once
#include <string>

struct SQLConsts {

#define BASIC_COL          \
	"id_db INT NOT NULL,"  \
	"hp_coef INT NOT NULL,"\
	"uid INT NOT NULL UNIQUE,"

#define PHYSICAL_COL      \
	BASIC_COL             \
	"player INT NOT NULL,"\
	"level INT NOT NULL,"

#define STATIC_COL          \
	"bucket_x INT NOT NULL,"\
	"bucket_y INT NOT NULL,"\
	"state INT NOT NULL,"   \
	"next_state INT NOT NULL"

	inline static constexpr const char* BUILDING_NAME = "buildings";
	inline static constexpr const char* BUILDING_COL = 
		"(" PHYSICAL_COL STATIC_COL ","
		"deploy_Idx INT NOT NULL);";

	inline static constexpr const char* RESOURCE_NAME = "resources";
	inline static constexpr const char* RESOURCE_COL = "(" BASIC_COL STATIC_COL ");";

	inline static constexpr const char* UNIT_NAME = "units";
	inline static constexpr const char* UNIT_COL = 
		"(" PHYSICAL_COL "position_x INT NOT NULL,"
		"position_z INT NOT NULL,"
		"state INT NOT NULL,"
		"velocity_x INT NOT NULL,"
		"velocity_z INT NOT NULL);";

	inline static constexpr const char* PLAYER_NAME = "players";
	inline static constexpr const char* PLAYER_COL = 
		"(id INT NOT NULL,"
		"is_active INT NOT NULL,"
		"team INT NOT NULL,"
		"nation INT NOT NULL,"
		"name TEXT NOT NULL,"
		"color INT NOT NULL,"
		"buildingUid INT NOT NULL,"
		"unitUid INT NOT NULL,"
		"food INT NOT NULL,"
		"wood INT NOT NULL,"
		"stone INT NOT NULL,"
		"gold INT NOT NULL);";

	inline static constexpr const char* CONFIG_NAME = "config";
	inline static constexpr const char* CONFIG_COL = 
		"(precision INT NOT NULL,"
		"map INT NOT NULL,"
		"size INT NOT NULL);";

	inline const static std::string CREATE_TABLE = "CREATE TABLE ";

	inline const static std::string SELECT = "SELECT * from ";
	inline const static std::string COUNT = "SELECT count(*) from ";
};
