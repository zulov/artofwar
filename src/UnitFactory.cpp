#include "UnitFactory.h"
#include "Unit.h"
#include <vector>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Model.h>
#include "EntityFactory.h"
#include "LinkComponent.h"
#include "ObjectManager.h"

UnitFactory::UnitFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene): EntityFactory(_cache, _scene) {

}

UnitFactory::~UnitFactory() {
}


std::vector<Unit*>* UnitFactory::createUnits() {
	int startSize = -(size / 2);
	int endSize = size + startSize;

	std::vector<Unit*>* units = new std::vector<Unit *>();
	units->reserve(size * size);
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	for (int y = startSize; y < endSize; ++y) {
		for (int x = startSize; x < endSize; ++x) {
			Vector3 position = Vector3(x * space, 0, y * space);
			Node* node = scene->CreateChild("Box");
			node->SetPosition(position);

			StaticModel* boxObject = node->CreateComponent<StaticModel>();
			boxObject->SetModel(cache->GetResource<Model>("Models/Cube.mdl"));

			Unit* newUnit = new Unit(position, node, font);
			units->push_back(newUnit);
			ObjectManager::createLink(node, newUnit);//to zrobic pozniej przy dodawaniu do object managerra
		}
	}
	return units;
}
