#include "SceneSaver.h"

#include <algorithm>
#include <format>

#include "SQLConsts.h"
#include "database/db_insert_defs.h"
#include "database/db_insert_utils.h"
#include "database/db_utils.h"
#include "math/RandGen.h"
#include "objects/building/Building.h"
#include "objects/projectile/ProjectileManager.h"
#include "objects/queue/QueueElement.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/FormationOrder.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "player/ai/ActionCenter.h"
#include "scene/load/RuntimeSaveData.h"
#include "simulation/FrameInfo.h"
#include "simulation/formation/Formation.h"
#include "simulation/formation/FormationManager.h"

namespace {
	std::string serializePath(const std::vector<int>& path) {
		std::string result;
		for (size_t i = 0; i < path.size(); ++i) {
			if (i > 0) {
				result += ',';
			}
			result += std::to_string(path[i]);
		}
		return result;
	}
} // namespace

SceneSaver::SceneSaver(int precision) : precision(precision), savingProgress(17) {
	// TODO zapisywanie powinno byc tylko miedzy klatkami
}

bool SceneSaver::createDatabase(const Urho3D::String& fileName) {
	database = nullptr;
	path = std::string("saves/") + fileName.CString() + ".db";
	if (const int rc = sqlite3_open(path.c_str(), &database)) {
		const std::string detail = database ? sqlite3_errmsg(database) : "unknown error";
		std::cerr << "Error opening SQLite3 database: " << (database ? sqlite3_errmsg(database) : "unknown error")
				  << "\n\n";
		if (database) {
			sqlite3_close_v2(database);
		}
		database = nullptr;
		fail("open '" + path + "' failed (code " + std::to_string(rc) + "): " + detail);
		return false;
	}

	if (!execSql(database, "PRAGMA journal_mode = DELETE;")) {
		fail("configure '" + path + "' failed: " + sqlite3_errmsg(database));
		return false;
	}
	if (!execSql(database, "PRAGMA synchronous = NORMAL;")) {
		fail("configure '" + path + "' failed: " + sqlite3_errmsg(database));
		return false;
	}
	if (!execSql(database, "PRAGMA temp_store = MEMORY;")) {
		fail("configure '" + path + "' failed: " + sqlite3_errmsg(database));
		return false;
	}
	if (!execSql(database, "PRAGMA page_size = 1024")) {
		fail("configure '" + path + "' failed: " + sqlite3_errmsg(database));
		return false;
	}
	return true;
}

bool SceneSaver::createTable(const std::string& name, const std::string& sql) {
	savingProgress.inc(std::format("saving {}", name));
	auto createSql = SQLConsts::CREATE_TABLE + name + sql;
	if (!execSql(database, createSql.c_str())) {
		fail("save '" + path + "': create table '" + name + "' failed: " + sqlite3_errmsg(database));
		return false;
	}
	return true;
}

bool SceneSaver::createSave(const Urho3D::String& fileName, const std::vector<Unit*>* units,
							const std::vector<Building*>* buildings, const std::vector<ResourceEntity*>* resources,
							const std::vector<Player*>& players, int mapId, int size) {
	error.clear();
	savingProgress.reset("create database");

	if (!createDatabase(fileName) || !execSql(database, "BEGIN;")) {
		if (error.empty()) {
			fail("save '" + path + "': begin transaction failed: " +
					(database ? sqlite3_errmsg(database) : "database is not open"));
		}
		close();
		return false;
	}

	const bool saved = saveUnits(units) && saveBuildings(buildings) && saveResources(resources) && savePlayers(players) &&
			saveConfig(mapId, size) && saveRuntimeState(units, buildings, players);
	if (!saved || !execSql(database, "COMMIT;")) {
		if (error.empty()) {
			fail("save '" + path + "': " + (saved ? "commit failed: " : "write failed: ") + sqlite3_errmsg(database));
		}
		execSql(database, "ROLLBACK;");
		close();
		return false;
	}
	if (!finalizeDatabase()) {
		std::cerr << "[Save Warning] save '" << path << "' was committed, but VACUUM failed: " << sqlite3_errmsg(database)
				  << "\n";
	}
	close();
	return true;
}

bool SceneSaver::finalizeDatabase() {
	return execSql(database, "VACUUM;");
}

void SceneSaver::fail(const std::string& message) {
	if (error.empty()) {
		error = message;
		std::cerr << "[Save Error] " << error << "\n";
	}
}

bool SceneSaver::saveUnits(const std::vector<Unit*>* units) {
	return saveRows<UnitCol>([&](sqlite3_stmt* stmt, const char* sql) {
		if (!units || units->empty()) { return true; }
		for (auto* u : *units) {
			bindRow(stmt, precision, u);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveBuildings(const std::vector<Building*>* buildings) {
	return saveRows<BuildingCol>([&](sqlite3_stmt* stmt, const char* sql) {
		if (!buildings || buildings->empty()) { return true; }
		for (auto* b : *buildings) {
			bindRow(stmt, precision, b);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveResources(const std::vector<ResourceEntity*>* resources) {
	return saveRows<ResourceCol>([&](sqlite3_stmt* stmt, const char* sql) {
		if (!resources || resources->empty()) { return true; }
		for (auto* r : *resources) {
			bindRow(stmt, precision, r);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::savePlayers(const std::vector<Player*>& players) {
	return saveRows<PlayerCol>([&](sqlite3_stmt* stmt, const char* sql) {
		if (players.empty()) { return true; }
		for (auto* p : players) {
			bindRow(stmt, precision, p);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveConfig(int mapId, int size) {
	const auto random = RandGen::saveState();
	const ConfigSaveData config{precision, mapId, size, Game::getFrameInfo()->getTotalTicks(), true, random};
	return saveRows<ConfigCol>([&](sqlite3_stmt* stmt, const char* sql) {
		bindRow(stmt, precision, &config);
		return stepAndReset(stmt, sql);
	});
}

bool SceneSaver::saveRuntimeState(const std::vector<Unit*>* units, const std::vector<Building*>* buildings,
								  const std::vector<Player*>& players) {
	std::vector<UnitRuntimeSaveData> unitStates;
	if (units) {
		unitStates.reserve(units->size());
		for (const auto* unit : *units) {
			unitStates.push_back(unit->captureRuntimeState());
		}
	}
	if (!saveUnitOrders(unitStates) || !saveAimPaths(unitStates) || !saveQueues(buildings, players) ||
			!savePlayerLevels(players) || !saveAiState(players) || !saveAiHistory(players) ||
			!saveAiWants(players) || !saveProjectiles() || !saveFormations() || !saveFormationOrders()) {
		return false;
	}
	const auto pendingCommands = Game::getActionCenter()->saveState();
	return savePendingCommands(pendingCommands) && savePendingCommandEntities(pendingCommands);
}

bool SceneSaver::saveUnitOrders(const std::vector<UnitRuntimeSaveData>& unitStates) {
	const bool hasOrders = std::ranges::any_of(unitStates, [](const auto& state) { return !state.orders.empty(); });
	if (!hasOrders) {
		return true;
	}
	return saveRows<UnitOrderCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto& state : unitStates) {
			unsigned short index = 0;
			for (const auto& order : state.orders) {
				const UnitOrderRow row{order, index++};
				bindRow(stmt, precision, &row);
				if (!stepAndReset(stmt, sql)) { return false; }
			}
		}
		return true;
	});
}

bool SceneSaver::saveAimPaths(const std::vector<UnitRuntimeSaveData>& unitStates) {
	const bool hasPaths = std::ranges::any_of(unitStates, [](const auto& state) {
		return !state.aim.path.empty() || !state.pendingAim.path.empty();
	});
	if (!hasPaths) {
		return true;
	}
	return saveRows<AimPathCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto& state : unitStates) {
			if (state.aim.path.empty() && state.pendingAim.path.empty()) {
				continue;
			}
			const AimPathSaveData row{state.uid, serializePath(state.aim.path), serializePath(state.pendingAim.path)};
			bindRow(stmt, precision, &row);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveQueues(const std::vector<Building*>* buildings, const std::vector<Player*>& players) {
	const bool hasQueues = std::ranges::any_of(players, [](const auto* player) { return !player->getQueue().isEmpty(); }) ||
			(buildings && std::ranges::any_of(*buildings, [](const auto* building) { return !building->getQueue().isEmpty(); }));
	if (!hasQueues) {
		return true;
	}
	return saveRows<QueueCol>([&](sqlite3_stmt* stmt, const char* sql) {
		auto saveQueue = [&](unsigned ownerId, unsigned char ownerType, const QueueManager& queue) {
			bool success = true;
			for (short i = 0; i < queue.getSize(); ++i) {
				const auto* element = queue.getAt(i);
				const QueueRow row{{ownerId, ownerType, static_cast<char>(element->getType()), element->getId(),
									element->getLevelId(), element->getAmount(), element->getElapsedTicks()},
									static_cast<unsigned short>(i)};
				bindRow(stmt, precision, &row);
				success = stepAndReset(stmt, sql);
				if (!success) { break; }
			}
			return success;
		};
		for (const auto* player : players) {
			if (!saveQueue(player->getId(), 0, player->getQueue())) { return false; }
		}
		if (buildings) {
			for (const auto* building : *buildings) {
				if (!saveQueue(building->getUid(), 1, building->getQueue())) { return false; }
			}
		}
		return true;
	});
}

bool SceneSaver::savePlayerLevels(const std::vector<Player*>& players) {
	const bool hasLevels = std::ranges::any_of(players, [](const auto* player) {
		return std::ranges::any_of(player->getUnitLevels(), [](const char level) { return level > 0; }) ||
				std::ranges::any_of(player->getBuildingLevels(), [](const char level) { return level > 0; });
	});
	if (!hasLevels) {
		return true;
	}
	return saveRows<PlayerLevelCol>([&](sqlite3_stmt* stmt, const char* sql) {
		auto saveLevels = [&](const Player* player, const auto& levels, unsigned char type) {
			bool success = true;
			for (unsigned short i = 0; i < levels.size(); ++i) {
				if (const char level = levels[i]; level > 0) {
					const PlayerLevelSaveData row{player->getId(), type, i, level};
					bindRow(stmt, precision, &row);
					success = stepAndReset(stmt, sql);
					if (!success) { break; }
				}
			}
			return success;
		};
		for (const auto* player : players) {
			if (!saveLevels(player, player->getUnitLevels(), 0) || !saveLevels(player, player->getBuildingLevels(), 1)) {
				return false;
			}
		}
		return true;
	});
}

bool SceneSaver::saveAiState(const std::vector<Player*>& players) {
	return saveRows<AiStateCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto* player : players) {
			const auto state = player->getAiOrchestrator().saveState(player->getId());
			bindRow(stmt, precision, &state);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveAiWants(const std::vector<Player*>& players) {
	const bool hasWants = std::ranges::any_of(players, [](const auto* player) {
		return !player->getAiOrchestrator().getWantItems().empty();
	});
	if (!hasWants) { return true; }
	return saveRows<AiWantCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto* player : players) {
			unsigned short index = 0;
			for (const auto& item : player->getAiOrchestrator().getWantItems()) {
				const AiWantRow row{{player->getId(), item.priority, item.basePriority, static_cast<unsigned char>(item.type),
						item.count, item.specificId, item.age, item.reserveTicks, item.active}, index++};
				bindRow(stmt, precision, &row);
				if (!stepAndReset(stmt, sql)) { return false; }
			}
		}
		return true;
	});
}

bool SceneSaver::saveAiHistory(const std::vector<Player*>& players) {
	const bool hasHistory = std::ranges::any_of(players, [](const auto* player) {
		return !player->getAiHistory().saveState(player->getId()).empty();
	});
	if (!hasHistory) {
		return true;
	}
	return saveRows<AiHistoryCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto* player : players) {
			unsigned short index = 0;
			for (const auto& state : player->getAiHistory().saveState(player->getId())) {
				const AiHistoryRow row{state, index++};
				bindRow(stmt, precision, &row);
				if (!stepAndReset(stmt, sql)) { return false; }
			}
		}
		return true;
	});
}

bool SceneSaver::saveProjectiles() {
	const auto projectiles = ProjectileManager::saveState();
	if (projectiles.empty()) {
		return true;
	}
	return saveRows<ProjectileCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto& projectile : projectiles) {
			bindRow(stmt, precision, &projectile);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveFormations() {
	const auto formations = Game::getFormationManager()->getFormations();
	if (std::ranges::none_of(formations, [](const auto* formation) { return formation != nullptr; })) {
		return true;
	}
	return saveRows<FormationCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto* formation : formations) {
			if (!formation) {
				continue;
			}
			const auto& direction = formation->getDirection();
			const FormationSaveData row{formation->getId(), static_cast<char>(formation->getState()),
					static_cast<char>(formation->getType()), direction.x_, direction.y_};
			bindRow(stmt, precision, &row);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::saveFormationOrders() {
	const auto formations = Game::getFormationManager()->getFormations();
	const bool hasOrders = std::ranges::any_of(formations, [](const auto* formation) {
		return formation && (!formation->getOrders().empty() || formation->getPendingOrder());
	});
	if (!hasOrders) { return true; }
	return saveRows<FormationOrderCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto* formation : formations) {
			if (!formation) { continue; }
			auto saveOrder = [&](const FormationOrder* order, unsigned short index, bool pending) {
				const auto saved = order->saveOrder(0);
				const FormationOrderRow row{{formation->getId(), saved.action, saved.append, saved.hasTarget,
						saved.targetUid, saved.x, saved.z}, index, pending};
				bindRow(stmt, precision, &row);
				return stepAndReset(stmt, sql);
			};
			for (unsigned short i = 0; i < formation->getOrders().size(); ++i) {
				if (!saveOrder(formation->getOrders()[i], i, false)) { return false; }
			}
			if (formation->getPendingOrder() &&
					!saveOrder(formation->getPendingOrder(), static_cast<unsigned short>(formation->getOrders().size()), true)) {
				return false;
			}
		}
		return true;
	});
}

bool SceneSaver::savePendingCommands(const std::vector<PendingCommandSaveData>& commands) {
	if (commands.empty()) {
		return true;
	}
	return saveRows<PendingCommandCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto& state : commands) {
			bindRow(stmt, precision, &state);
			if (!stepAndReset(stmt, sql)) { return false; }
		}
		return true;
	});
}

bool SceneSaver::savePendingCommandEntities(const std::vector<PendingCommandSaveData>& commands) {
	const bool hasEntities = std::ranges::any_of(commands, [](const auto& state) { return !state.entityUids.empty(); });
	if (!hasEntities) {
		return true;
	}
	return saveRows<PendingCommandEntityCol>([&](sqlite3_stmt* stmt, const char* sql) {
		for (const auto& state : commands) {
			for (unsigned short i = 0; i < state.entityUids.size(); ++i) {
				const PendingCommandEntityRow row{state.order, i, state.entityUids[i]};
				bindRow(stmt, precision, &row);
				if (!stepAndReset(stmt, sql)) { return false; }
			}
		}
		return true;
	});
}

void SceneSaver::close() {
	if (database) {
		sqlite3_close_v2(database);
		database = nullptr;
	}
	savingProgress.inc("");
}
