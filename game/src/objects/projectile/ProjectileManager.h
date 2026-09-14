#pragma once
#include <unordered_map>
#include <vector>
#include "scene/load/RuntimeSaveData.h"

struct ProjectileBase;
class Physical;

namespace Urho3D {
	class Vector3;
	class Node;
} // namespace Urho3D

class ProjectileManager {
public:
	static void update(float timeStep);
	static void shoot(Physical* shooter, Physical* aim, float speed, char player);
	static ProjectileBase* findNext();
	static void init();
	static void dispose();
	static void reset();
	static void clearNodesWithoutDelete();
	static void returnToPool(ProjectileBase* projectile);
	static std::vector<ProjectileSaveData> saveState();
	static void loadState(const std::vector<ProjectileSaveData>& state,
						  const std::unordered_map<unsigned, Physical*>& byUid);

private:
	ProjectileManager() = default;
	~ProjectileManager();

	static ProjectileManager* instance;

	std::vector<ProjectileBase*> projectiles;
	std::vector<ProjectileBase*> freeList;
};
