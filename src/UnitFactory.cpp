#include "UnitFactory.h"
#include "Unit.h"
#include <vector>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Model.h>
#include "EntityFactory.h"

UnitFactory::UnitFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene):EntityFactory(_cache, _scene) {

}

UnitFactory::~UnitFactory() {}

std::vector<Unit*>* UnitFactory::createUnits() {
	int startSize = -(size / 2);
	int endSize = size + startSize;

	std::vector<Unit*>* units = new std::vector<Unit *>();
	units->reserve(size * size);

	for (int y = startSize; y < endSize; ++y) {
		for (int x = startSize; x < endSize; ++x) {
			Vector3 position = Vector3(x * space, 0, y * space);
			Node* boxNode = scene->CreateChild("Box");
			boxNode->SetPosition(position);
			
			StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
			boxObject->SetModel(cache->GetResource<Model>("Models/Cube.mdl"));

			Node* title = boxNode->CreateChild("title");
			title->SetPosition(Vector3(0.0f, 1.2f, 0.0f));
			Text3D* titleText = title->CreateComponent<Text3D>();
			titleText->SetText("Entity");

			titleText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 24);
			titleText->SetColor(Color::GREEN);
			titleText->SetAlignment(HA_CENTER, VA_CENTER);
			titleText->SetFaceCameraMode(FC_LOOKAT_MIXED);

			Unit* newUnit = new Unit(position, boxNode);
			units->push_back(newUnit);
		}
	}
	return units;
}