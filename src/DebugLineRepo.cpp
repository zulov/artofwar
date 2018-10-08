#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPeletteRepo.h"

Urho3D::CustomGeometry* DebugLineRepo::geometry = nullptr;

void DebugLineRepo::init() {
	if (geometry == nullptr) {
		geometry = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
	}
}

DebugLineRepo::DebugLineRepo() {
}


DebugLineRepo::~DebugLineRepo() {
	geometry->Remove();
}
