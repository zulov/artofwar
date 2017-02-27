#include "BuildingFactory.h"


BuildingFactory::BuildingFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene): EntityFactory(_cache, _scene) {

}

BuildingFactory::~BuildingFactory() {
}
