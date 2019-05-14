#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "defines.h"

Urho3D::CustomGeometry* DebugLineRepo::geometry[] = {nullptr, nullptr};

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (geometry[static_cast<char>(type)] == nullptr) {
			geometry[static_cast<char>(type)] = Game::getScene()
			                                    ->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
		}
	}
}

DebugLineRepo::DebugLineRepo() = default;


DebugLineRepo::~DebugLineRepo() {
	if constexpr (DEBUG_LINES_ENABLED) {
		for (auto customGeometry : geometry) {
			customGeometry->Remove();
		}
	}
}

void DebugLineRepo::commit(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)]->SetMaterial(Game::getColorPaletteRepo()->getLineMaterial());
		geometry[static_cast<char>(type)]->Commit();
	}
}

void DebugLineRepo::beginGeometry(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)]->BeginGeometry(0, Urho3D::PrimitiveType::LINE_LIST);
	}
}

void DebugLineRepo::clear(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)]->Clear();
		geometry[static_cast<char>(type)]->SetNumGeometries(1);
	}
}

void DebugLineRepo::drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                             const Urho3D::Color& color) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)]->DefineVertex(first);
		geometry[static_cast<char>(type)]->DefineColor(color);
		geometry[static_cast<char>(type)]->DefineVertex(second);
		geometry[static_cast<char>(type)]->DefineColor(color);
	}
}
