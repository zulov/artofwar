#pragma once

#include <span>
#include <vector>
#include "map/InfluenceMapCombine.h"
#include "map/InfluenceMapFloat.h"
#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapInt.h"
#include "map/InfluenceMapQuad.h"
#include "objects/Physical.h"
#include "player/ai/ActionMaker.h"
#include "player/ai/InfluenceType.h"

enum class CellState : char;
class Unit;
class Building;
class ResourceEntity;
struct content_info;

class InfluenceManager {
public:
	explicit InfluenceManager(char numberOfPlayers);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings) const;
	void update(std::vector<ResourceEntity*>* resources) const;

	void updateBasic(std::vector<Unit*>* units, std::vector<Building*>* buildings) const;
	void updateQuad(std::vector<Unit*>* units, std::vector<Building*>* buildings) const;
	void updateWithHistory() const;

	void drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const;
	void drawMap(char index, const std::vector<InfluenceMapCombine*>& vector, ValueType type) const;

	void draw(InfluenceType type, char index);
	void drawAll();
	void switchDebug();

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	std::vector<float>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::vector<int> getIndexesIterative(const std::span<float> result, float tolerance, int min,
	                                     std::array<InfluenceMapFloat*, 9>& maps) const;

	std::vector<Urho3D::Vector2> getAreasIterative(const std::span<float> result, char player, float tolerance,
	                                               int min);

	float getFieldSize();
	std::vector<Urho3D::Vector2> getAreas(const std::span<float> result, char player);
	void addCollect(Unit* unit, float value);
	void addAttack(Unit* unit, float value);

private:
	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, float* values,
	                                                const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, const std::vector<int>& intersection);

	template <typename T>
	void resetMaps(const std::vector<T*>& maps) const;
	template <typename T>
	void calcStats(const std::vector<T*>& maps) const;
	template <typename T>
	void drawAll(const std::vector<T*>& maps, Urho3D::String name) const;

	void basicValuesFunc(float* weights, Physical* thing) const;

	std::vector<std::array<InfluenceMapFloat*, 9>> mapsForAiPerPlayer;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;

	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> resourceInfluence;

	std::vector<InfluenceMapHistory*> gatherSpeed;
	std::vector<InfluenceMapHistory*> attackSpeed;

	std::vector<InfluenceMapCombine*> basicValues;

	InfluenceMapQuad* quad;

	InfluenceType debugType = InfluenceType::UNITS_INFLUENCE_PER_PLAYER;
	content_info* ci;
	std::vector<float> dataFromPos;
	short currentDebugBatch = 0;
};
