#pragma once
#include <vector>

struct ProjectileData;

namespace Urho3D {
	class Node;
}

class ProjectileManager {
public:
	void add();
	static void init();
	static void dispose();

private:
	ProjectileManager();
	~ProjectileManager();

	static ProjectileManager* instance;

	std::vector<Urho3D::Node*> nodes;
	std::vector<ProjectileData*> projectiles;
};
