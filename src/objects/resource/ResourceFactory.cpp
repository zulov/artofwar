#include "ResourceFactory.h"
#include "Game.h"
#include "objects/building/Building.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>


ResourceFactory::ResourceFactory() {
	resources = new std::vector<ResourceEntity *>();
	resources->reserve(DEFAULT_VECTOR_SIZE);
}


ResourceFactory::~ResourceFactory() {
	resources->clear();
	delete resources;
}

std::vector<ResourceEntity*>* ResourceFactory::create(unsigned number, int id, Vector3* center, SpacingType spacing) {
	resources->clear();

	double space = getSpecSize(spacing) * spaceCoef;

	int y = 0;
	int xMax = number / sqrt(number);
	double sideSize = xMax * space / 2;
	while (resources->size()  < number) {
		for (int x = 0; x < xMax; ++x) {

			Vector3* position = new Vector3(x * space + center->x_ - sideSize, 0 + center->y_,
			                                y * space + center->z_ - sideSize);

			ResourceEntity* entity = new ResourceEntity(position, id);
			resources->push_back(entity);

			if (resources->size() >= number) { break; }
		}
		++y;
	}
	return resources;
}
