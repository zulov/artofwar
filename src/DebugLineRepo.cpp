#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPeletteRepo.h"
#include "defines.h"

Urho3D::CustomGeometry* DebugLineRepo::geometry = nullptr;

void DebugLineRepo::init() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		if (geometry == nullptr) {
			geometry = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
		}
	}
}

DebugLineRepo::DebugLineRepo() = default;


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
		geometry->BeginGeometry(0, Urho3D::PrimitiveType::LINE_LIST);
	}
}

void DebugLineRepo::clear() {
	if constexpr (UNIT_DEBUG_ENABLED) {
		geometry->Clear();
		geometry->SetNumGeometries(1);
	}
}

void DebugLineRepo::drawLine(const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                    const Urho3D::Color& color) {
	geometry->DefineVertex(first);
	geometry->DefineColor(color);
	geometry->DefineVertex(second);
	geometry->DefineColor(color);
}
