#pragma once
#include <vector>

struct db_attack;
class Physical;
struct ProjectileData;

namespace Urho3D {
	class Vector3;
	class Node;
}

class ProjectileManager {
public:
	static void update(float timeStep);
	static void shoot(const Urho3D::Vector3& start, Physical* aim, float speed, char player, db_attack* dbAttack);
	static ProjectileData* findNext();
	static void init();
	static void dispose();

private:
	ProjectileManager() = default;
	~ProjectileManager();

	static ProjectileManager* instance;

	std::vector<ProjectileData*> projectiles;
};
