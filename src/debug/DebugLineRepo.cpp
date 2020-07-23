#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "objects/CellState.h"
#include "utils/defines.h"

std::vector<Urho3D::CustomGeometry*> DebugLineRepo::geometry[];

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto& geom = geometry[static_cast<char>(type)];
		if (geom.empty()) {
			geom.push_back(Game::getScene()->CreateChild()
			                               ->GetOrCreateComponent<Urho3D::CustomGeometry>());
		}
	}
}

void DebugLineRepo::init(DebugLineType type, short batches) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto& geom = geometry[static_cast<char>(type)];
		if (geom.empty()) {
			for (int i = 0; i < batches; ++i) {
				geom.push_back(Game::getScene()->CreateChild()
				                               ->GetOrCreateComponent<Urho3D::CustomGeometry>());
			}
		}
	}
}

DebugLineRepo::~DebugLineRepo() {
	if constexpr (DEBUG_LINES_ENABLED) {
		for (const auto& vector : geometry) {
			for (auto customGeometry : vector) {
				customGeometry->Remove();
			}
		}
	}
}

std::tuple<bool, Urho3D::Color> DebugLineRepo::getInfoForGrid(CellState state) {
	switch (state) {
	case CellState::EMPTY: return {false, Urho3D::Color::BLACK};
	case CellState::ATTACK: return {true, Urho3D::Color::RED};
	case CellState::COLLECT: return {true, Urho3D::Color::YELLOW};
	case CellState::NONE: return {false, Urho3D::Color::BLACK};
	case CellState::RESOURCE: return {true, Urho3D::Color::GREEN};
	case CellState::BUILDING: return {true, Urho3D::Color::BLUE};
	case CellState::DEPLOY: return {true, Urho3D::Color::CYAN};
	default: ;
	}
}

void DebugLineRepo::commit(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto* geom = geometry[static_cast<char>(type)].at(batch);
		geom->SetMaterial(Game::getColorPaletteRepo()->getLineMaterial());
		geom->Commit();
	}
}

void DebugLineRepo::beginGeometry(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (type == DebugLineType::INFLUANCE) {
			geometry[static_cast<char>(type)].at(batch)->BeginGeometry(0, Urho3D::PrimitiveType::TRIANGLE_LIST);
		} else {
			geometry[static_cast<char>(type)].at(batch)->BeginGeometry(0, Urho3D::PrimitiveType::LINE_LIST);
		}
	}
}

void DebugLineRepo::clear(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto* geom = geometry[static_cast<char>(type)].at(batch);
		geom->Clear();
		geom->SetNumGeometries(1);
	}
}

void DebugLineRepo::drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                             const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto* geom = geometry[static_cast<char>(type)].at(batch);
		geom->DefineVertex(first);
		geom->DefineColor(color);
		geom->DefineVertex(second);
		geom->DefineColor(color);
	}
}

void DebugLineRepo::drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                                 const Urho3D::Vector3& third, const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		auto* geom = geometry[static_cast<char>(type)].at(batch);
		geom->DefineVertex(first);
		geom->DefineColor(color);
		geom->DefineVertex(second);
		geom->DefineColor(color);
		geom->DefineVertex(third);
		geom->DefineColor(color);
	}
}
