#include "SceneLoader.h"

#include <algorithm>
#include <charconv>
#include <sstream>
#include <system_error>
#include <string_view>

#include "RuntimeSaveData.h"
#include "database/db_read_defs.h"
#include "database/db_utils.h"
#include "dbload_container.h"
#include "math/RandGen.h"
#include "utils/StringUtils.h"

namespace {
	bool tableExists(sqlite3* database, const char* tableName) {
		if (!database) {
			return false;
		}
		sqlite3_stmt* statement{};
		if (sqlite3_prepare_v2(database, "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?;", -1, &statement,
								nullptr) != SQLITE_OK) {
			return false;
		}
		const bool bound = sqlite3_bind_text(statement, 1, tableName, -1, SQLITE_STATIC) == SQLITE_OK;
		const bool exists = bound && sqlite3_step(statement) == SQLITE_ROW;
		sqlite3_finalize(statement);
		return exists;
	}

	bool inspectTable(sqlite3* database, const char* tableName, const std::vector<std::string>& expected,
					 std::string& detail) {
		const std::string sql = "PRAGMA table_info(" + std::string(tableName) + ");";
		sqlite3_stmt* statement{};
		if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
			detail = sqlite3_errmsg(database);
			return false;
		}
		std::vector<std::string> actual;
		int rc = SQLITE_OK;
		while ((rc = sqlite3_step(statement)) == SQLITE_ROW) {
			actual.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
		}
		if (rc != SQLITE_DONE) {
			detail = sqlite3_errmsg(database);
			sqlite3_finalize(statement);
			return false;
		}
		sqlite3_finalize(statement);

		std::vector<std::string> missing;
		for (const auto& column : expected) {
			if (std::ranges::find(actual, column) == actual.end()) {
				missing.push_back(column);
			}
		}
		if (missing.empty()) {
			return true;
		}

		std::ostringstream message;
		message << "missing columns: ";
		for (size_t i = 0; i < missing.size(); ++i) {
			if (i > 0) {
				message << ", ";
			}
			message << missing[i];
		}
		message << "; actual columns: ";
		for (size_t i = 0; i < actual.size(); ++i) {
			if (i > 0) {
				message << ", ";
			}
			message << actual[i];
		}
		detail = message.str();
		return false;
	}

	bool parsePath(std::string_view value, std::vector<int>& path, std::string& detail) {
		path.clear();
		if (value.empty()) {
			return true;
		}

		size_t begin = 0;
		while (begin <= value.size()) {
			const auto end = value.find(',', begin);
			const auto tokenEnd = end == std::string_view::npos ? value.size() : end;
			if (begin == tokenEnd) {
				detail = "empty cell index";
				return false;
			}

			int cell{};
			const auto [parsedEnd, error] = std::from_chars(value.data() + begin, value.data() + tokenEnd, cell);
			if (error != std::errc{} || parsedEnd != value.data() + tokenEnd || cell < 0) {
				detail = "invalid cell index '" + std::string(value.substr(begin, tokenEnd - begin)) + "'";
				return false;
			}
			path.push_back(cell);

			if (end == std::string_view::npos) {
				return true;
			}
			begin = end + 1;
		}
		return true;
	}
} // namespace

SceneLoader::~SceneLoader() {
	close();
	delete dbLoad;
	dbLoad = nullptr;
}

void SceneLoader::load() {
	if (!loadPlayers() || hasError()) {
		return;
	}
	loadUnits();
	if (hasError()) {
		return;
	}
	loadBuildings();
	if (hasError()) {
		return;
	}
	loadResourcesEntities();
	close();
}

void SceneLoader::reset() {
	close();
	delete dbLoad;
	dbLoad = new dbload_container();
	error.clear();
}

dbload_container* SceneLoader::getData() const { return dbLoad; }

void SceneLoader::createLoad(const Urho3D::String& fileName, bool tryReuse) {
	if (fileName == lastLoad && tryReuse) {
		return;
	}
	reset();

	lastLoad = fileName;
	path = std::string("saves/") + fileName.CString();
	std::string openError;
	database = openDb(path, true, &openError);
	if (!database) {
		reportError("load '" + path + "' failed: " + openError);
		return;
	}
	for (const char* table : {SaveTable<ConfigCol>::name, SaveTable<PlayerCol>::name, SaveTable<UnitCol>::name,
							 SaveTable<BuildingCol>::name, SaveTable<ResourceCol>::name}) {
		if (!hasTable(table)) {
			reportError("load '" + path + "' failed: required table '" + table + "' is missing; this is an old or incomplete save, "
					"see docs/old-save-migration.txt");
			return;
		}
	}
	const auto validateTable = [this](const char* table, const auto& columns) {
		std::string detail;
		if (!inspectTable(database, table, columns, detail)) {
			reportError("load '" + path + "' failed: table '" + table + "' schema mismatch: " + detail +
					"; compare with the current save schema or run docs/old-save-migration.txt");
			return false;
		}
		return true;
	};
	if (!validateTable(SaveTable<ConfigCol>::name, saveColumns<ConfigCol>()) ||
			!validateTable(SaveTable<PlayerCol>::name, saveColumns<PlayerCol>()) ||
			!validateTable(SaveTable<UnitCol>::name, saveColumns<UnitCol>()) ||
			!validateTable(SaveTable<BuildingCol>::name, saveColumns<BuildingCol>()) ||
			!validateTable(SaveTable<ResourceCol>::name, saveColumns<ResourceCol>())) {
		return;
	}
	bool hasConfig = false;
	if (!loadSaveTable<ConfigCol>("", [this, &hasConfig](auto* s) {
		if (hasConfig) {
			reportError("load '" + path + "' failed: config table must contain exactly one row");
			return;
		}
		const auto config = readRow<ConfigSaveData>(s, 1);
		if (config.precision <= 0) {
			reportError("load '" + path + "' failed: config.precision must be positive, got " +
					std::to_string(config.precision));
			return;
		}
		if (config.map < 0 || config.size <= 0) {
			reportError("load '" + path + "' failed: invalid config values (map=" + std::to_string(config.map) +
					", size=" + std::to_string(config.size) + ")");
			return;
		}
		dbLoad->config = new dbload_config(config.precision, config.map, config.size, config.totalTicks);
		dbLoad->frame = dbLoad->config->frame;
		if (config.randomPresent) {
			dbLoad->random = config.random;
		}
		hasConfig = true;
	})) {
		return;
	}
	if (hasError()) {
		return;
	}
	if (!hasConfig) {
		reportError("load '" + path + "' failed: config table must contain exactly one row");
		return;
	}
	loadRuntimeState();
}

dbload_config* SceneLoader::getConfig() const { return dbLoad->config; }

const std::vector<dbload_player*>* SceneLoader::loadPlayers() const {
	if (hasError() || !dbLoad->config) {
		if (!hasError()) {
			reportError("load '" + path + "' failed: config must be loaded before players");
		}
		return nullptr;
	}
	if (dbLoad->players) {
		return dbLoad->players;
	}
	dbLoad->players = new std::vector<dbload_player*>();

	loadSaveTable<PlayerCol>("", [this](auto* s) {
		dbLoad->players->push_back(new dbload_player(s, dbLoad->config->precision));
	});
	return dbLoad->players;
}

void SceneLoader::loadUnits() const {
	if (hasError() || !dbLoad->config) {
		if (!hasError()) {
			reportError("load '" + path + "' failed: config must be loaded before units");
		}
		return;
	}
	if (dbLoad->units) {
		return;
	}
	dbLoad->units = new std::vector<dbload_unit*>();
	loadSaveTable<UnitCol>("", [this](auto* s) {
		auto* unit = new dbload_unit(s, dbLoad->config->precision);
		if (const auto it = dbLoad->unitVariable.find(unit->uid); it != dbLoad->unitVariable.end()) {
			unit->runtime.orders = std::move(it->second.orders);
			unit->runtime.aim.path = std::move(it->second.aimPath);
			unit->runtime.pendingAim.path = std::move(it->second.pendingAimPath);
			dbLoad->unitVariable.erase(it);
		}
		dbLoad->units->push_back(unit);
	});
}

void SceneLoader::loadBuildings() const {
	if (hasError() || !dbLoad->config) {
		if (!hasError()) {
			reportError("load '" + path + "' failed: config must be loaded before buildings");
		}
		return;
	}
	if (dbLoad->buildings) {
		return;
	}
	dbLoad->buildings = new std::vector<dbload_building*>();
	loadSaveTable<BuildingCol>("", [this](auto* s) {
		dbLoad->buildings->push_back(new dbload_building(s, dbLoad->config->precision));
	});
}

void SceneLoader::loadResourcesEntities() const {
	if (hasError() || !dbLoad->config) {
		if (!hasError()) {
			reportError("load '" + path + "' failed: config must be loaded before resources");
		}
		return;
	}
	if (dbLoad->resources) {
		return;
	}
	dbLoad->resources = new std::vector<dbload_resource*>();

	if (!count(SaveTable<ResourceCol>::name, [this](auto* s) { dbLoad->resources->reserve(asInt(s, 0)); })) {
		return;
	}
	loadSaveTable<ResourceCol>("", [this](auto* s) {
		dbLoad->resources->push_back(new dbload_resource(s, dbLoad->config->precision));
	});
}

void SceneLoader::end() { close(); }

void SceneLoader::close() {
	if (database) {
		sqlite3_close_v2(database);
		database = nullptr;
	}
}

bool SceneLoader::hasTable(const char* tableName) const { return tableExists(database, tableName); }

void SceneLoader::reportError(const std::string& message) const {
	if (error.empty() || error.find("[Load Error]") == std::string::npos) {
		error = "[Load Error] " + message;
		std::cerr << error << "\n";
	}
}

void SceneLoader::loadRuntimeState() const {
	const int precision = dbLoad->config->precision;
	loadOptionalSaveTable<UnitOrderCol>(" ORDER BY unit_uid, order_idx", [this, precision](sqlite3_stmt* s) {
		const auto order = readRow<UnitOrderSaveData>(s, precision);
		dbLoad->unitVariable[order.unitUid].orders.push_back(order);
	});

	loadAimPaths();
	if (hasError()) {
		return;
	}

	loadOptionalSaveTable<QueueCol>(" ORDER BY owner_type, owner_id, order_idx", [this, precision](sqlite3_stmt* s) {
		dbLoad->queues.push_back(readRow<QueueRow>(s, precision).data);
	});

	loadOptionalSaveTable<PlayerLevelCol>("", [this, precision](sqlite3_stmt* s) {
		dbLoad->playerLevels.push_back(readRow<PlayerLevelSaveData>(s, precision));
	});

	if (dbLoad->random) {
		for (const auto index : dbLoad->random->floatIndexes) {
			if (index < 0 || index >= RAND_TAB_SIZE) {
				reportError("load '" + path + "' failed: random float stream index is out of range");
				return;
			}
		}
		for (const auto index : dbLoad->random->intIndexes) {
			if (index < 0 || index >= RAND_TAB_SIZE) {
				reportError("load '" + path + "' failed: random int stream index is out of range");
				return;
			}
		}
	}

	loadOptionalSaveTable<ProjectileCol>("", [this, precision](sqlite3_stmt* s) {
		dbLoad->projectiles.push_back(readRow<ProjectileSaveData>(s, precision));
	});

	loadOptionalSaveTable<FormationCol>("", [this, precision](sqlite3_stmt* s) {
		dbLoad->formations.push_back(readRow<FormationSaveData>(s, precision));
	});

	loadOptionalSaveTable<FormationOrderCol>(" ORDER BY formation_id, pending, order_idx", [this, precision](sqlite3_stmt* s) {
		dbLoad->formationOrders.push_back(readRow<FormationOrderRow>(s, precision));
	});

	loadOptionalSaveTable<PendingCommandCol>(" ORDER BY order_idx", [this, precision](sqlite3_stmt* s) {
		dbLoad->pendingCommands.push_back(readRow<PendingCommandSaveData>(s, precision));
	});

	loadOptionalSaveTable<PendingCommandEntityCol>(" ORDER BY command_idx, order_idx", [this](sqlite3_stmt* s) {
		const auto row = readRow<PendingCommandEntityRow>(s, 1);
		for (auto& command : dbLoad->pendingCommands) {
			if (command.order == row.commandIndex) {
				command.entityUids.push_back(row.uid);
				break;
			}
		}
	});

	loadOptionalSaveTable<AiStateCol>("", [this, precision](sqlite3_stmt* s) {
		dbLoad->aiStates.push_back(readRow<AiSaveData>(s, precision));
	});
	loadOptionalSaveTable<AiWantCol>(" ORDER BY player, order_idx", [this, precision](sqlite3_stmt* s) {
		dbLoad->aiWants.push_back(readRow<AiWantRow>(s, precision).data);
	});

	loadOptionalSaveTable<AiHistoryCol>(" ORDER BY player, action, order_idx", [this](sqlite3_stmt* s) {
		dbLoad->aiHistory.push_back(readRow<AiHistoryRow>(s, 1).data);
	});

}

void SceneLoader::loadAimPaths() const {
	if (!hasTable(SaveTable<AimPathCol>::name)) {
		return;
	}

	std::string detail;
	if (inspectTable(database, SaveTable<AimPathCol>::name, saveColumns<AimPathCol>(), detail)) {
		loadSaveTable<AimPathCol>("", [this](sqlite3_stmt* s) {
			const auto row = readRow<AimPathSaveData>(s, 1);
			auto& state = dbLoad->unitVariable[row.unitUid];
			std::string pathError;
			if (!parsePath(row.path, state.aimPath, pathError)) {
				reportError("load '" + path + "' failed: invalid aim_paths.path for unit " +
						std::to_string(row.unitUid) + ": " + pathError);
				return;
			}
			if (!parsePath(row.pendingPath, state.pendingAimPath, pathError)) {
				reportError("load '" + path + "' failed: invalid aim_paths.pending_path for unit " +
						std::to_string(row.unitUid) + ": " + pathError);
			}
		});
		return;
	}

	const std::vector<std::string> legacyColumns{"unit_uid", "pending", "order_idx", "cell"};
	if (!inspectTable(database, SaveTable<AimPathCol>::name, legacyColumns, detail)) {
		reportError("load '" + path + "' failed: table '" + SaveTable<AimPathCol>::name +
				" schema mismatch: " + detail);
		return;
	}

	const std::string sql = "SELECT unit_uid, pending, order_idx, cell FROM aim_paths ORDER BY unit_uid, pending, order_idx";
	std::string queryError;
	if (!loadFromTable(database, sql, [this](sqlite3_stmt* s) {
		const auto unitUid = asUI(s, 0);
		const bool pending = asBool(s, 1);
		const auto cell = asInt(s, 3);
		auto& state = dbLoad->unitVariable[unitUid];
		(pending ? state.pendingAimPath : state.aimPath).push_back(cell);
	}, &queryError)) {
		reportError("load '" + path + "': table 'aim_paths' failed: " + queryError);
	}
}
