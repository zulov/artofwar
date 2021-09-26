#pragma once
#include <vector>

struct ProjectileBase;
struct db_attack;
class Physical;

namespace Urho3D {
	class Vector3;
	class Node;
}

class ProjectileManager {
public:
	static void update(float timeStep);
	static void shoot(Physical* shooter, Physical* aim, float speed, char player, db_attack* dbAttack);
	static ProjectileBase* findNext();
	static void init();
	static void dispose();

private:
	ProjectileManager() = default;
	~ProjectileManager();

	static ProjectileManager* instance;

	std::vector<ProjectileBase*> projectiles;
};
