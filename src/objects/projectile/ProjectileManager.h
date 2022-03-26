#pragma once
#include <vector>

struct ProjectileBase;
class Physical;

namespace Urho3D {
	class Vector3;
	class Node;
}

class ProjectileManager {
public:
	static void update(float timeStep);
	static void shoot(Physical* shooter, Physical* aim, float speed, char player);
	static ProjectileBase* findNext();
	static void init();
	static void dispose();
	static void reset();

private:
	ProjectileManager() = default;
	~ProjectileManager();

	static ProjectileManager* instance;

	std::vector<ProjectileBase*> projectiles;
};
