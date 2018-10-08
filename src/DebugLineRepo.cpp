#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPeletteRepo.h"
#include "defines.h"

Urho3D::CustomGeometry* DebugLineRepo::geometry = nullptr;
int DebugLineRepo::i = 0;

void DebugLineRepo::init() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		if (geometry == nullptr) {
			geometry = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
			i = 0;
		}
	}
}

DebugLineRepo::DebugLineRepo() {
}


DebugLineRepo::~DebugLineRepo() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		geometry->Remove();
		geometry = nullptr;
	}
}

void DebugLineRepo::commit() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		geometry->SetMaterial(Game::getColorPeletteRepo()->getLineMaterial());
		geometry->Commit();
	}
}

void DebugLineRepo::beginGeometry() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		geometry->BeginGeometry(i, Urho3D::PrimitiveType::LINE_LIST);
		i++;
	}
}

void DebugLineRepo::clear(int size) {
	if constexpr (UNIT_DEBUG_ENABLED) {
		geometry->Clear();
		geometry->SetNumGeometries(size);
		i = 0;
	}
}
