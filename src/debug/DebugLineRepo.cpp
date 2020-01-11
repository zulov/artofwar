#include "DebugLineRepo.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "defines.h"
#include "objects/CellState.h"

std::vector<Urho3D::CustomGeometry*> DebugLineRepo::geometry[];

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (geometry[static_cast<char>(type)].empty()) {
			geometry[static_cast<char>(type)].push_back(Game::getScene()->CreateChild()
			                                                            ->GetOrCreateComponent<Urho3D::CustomGeometry
			                                                            >());
		}
	}
}

void DebugLineRepo::init(DebugLineType type, short batches) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (geometry[static_cast<char>(type)].empty()) {
			for (int i = 0; i < batches; ++i) {
				geometry[static_cast<char>(type)].push_back(Game::getScene()->CreateChild()
				                                                            ->GetOrCreateComponent<Urho3D::
					                                                            CustomGeometry
				                                                            >());
			}
		}
	}
}

DebugLineRepo::DebugLineRepo() = default;


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
		geometry[static_cast<char>(type)].at(batch)->SetMaterial(Game::getColorPaletteRepo()->getLineMaterial());
		geometry[static_cast<char>(type)].at(batch)->Commit();
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
		geometry[static_cast<char>(type)].at(batch)->Clear();
		geometry[static_cast<char>(type)].at(batch)->SetNumGeometries(1);
	}
}

void DebugLineRepo::drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                             const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)].at(batch)->DefineVertex(first);
		geometry[static_cast<char>(type)].at(batch)->DefineColor(color);
		geometry[static_cast<char>(type)].at(batch)->DefineVertex(second);
		geometry[static_cast<char>(type)].at(batch)->DefineColor(color);
	}
}

void DebugLineRepo::drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                                 const Urho3D::Vector3& third, const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		geometry[static_cast<char>(type)].at(batch)->DefineVertex(first);
		geometry[static_cast<char>(type)].at(batch)->DefineColor(color);
		geometry[static_cast<char>(type)].at(batch)->DefineVertex(second);
		geometry[static_cast<char>(type)].at(batch)->DefineColor(color);
		geometry[static_cast<char>(type)].at(batch)->DefineVertex(third);
		geometry[static_cast<char>(type)].at(batch)->DefineColor(color);
	}
}
