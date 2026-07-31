#include "DebugLineRepo.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "simulation/SimGlobals.h"
#include "utils/OtherUtils.h"

std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> DebugLineRepo::geometry;

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& geom = geometry[castC(type)];
			if (!geom) {
				geom = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
			}
		}
	}
}

void DebugLineRepo::dispose() { for (auto& customGeometries : geometry) { customGeometries = nullptr; } }

DebugLineRepo::~DebugLineRepo() {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) { for (const auto& customGeometry : geometry) { customGeometry->Remove(); } }
	}
}

void DebugLineRepo::commit(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[castC(type)];
			switch (type) {
			case DebugLineType::INFLUENCE:
			case DebugLineType::MAIN_GRID:
				geom->SetMaterial(Game::getColorPaletteRepo()->getInfluenceMaterial());
				break;
			default:
				geom->SetMaterial(Game::getColorPaletteRepo()->getLineMaterial());
			}
			geom->Commit();
		}
	}
}

void DebugLineRepo::beginGeometry(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			const auto primitiveType = (type == DebugLineType::INFLUENCE || type == DebugLineType::MAIN_GRID)
					? Urho3D::PrimitiveType::TRIANGLE_LIST
					: Urho3D::PrimitiveType::LINE_LIST;

			geometry[castC(type)]->BeginGeometry(0, primitiveType);
		}
	}
}

void DebugLineRepo::clear(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			if (auto* geom = geometry[castC(type)]) {
				geom->Clear();
				geom->SetNumGeometries(1);
			}
		}
	}
}

void DebugLineRepo::drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                             const Urho3D::Color& color) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[castC(type)];
			geom->DefineVertex(first);
			geom->DefineColor(color);
			geom->DefineVertex(second);
			geom->DefineColor(color);
		}
	}
}


void DebugLineRepo::drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                                 const Urho3D::Vector3& third, const Urho3D::Color& color) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[castC(type)];
			geom->DefineVertex(first);
			geom->DefineColor(color);
			geom->DefineVertex(second);
			geom->DefineColor(color);
			geom->DefineVertex(third);
			geom->DefineColor(color);
		}
	}
}

void DebugLineRepo::drawQuad(DebugLineType type, const std::array<Urho3D::Vector3, 4>& corners,
                             const Urho3D::Color& color) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[castC(type)];
			geom->DefineVertex(corners[0]);
			geom->DefineColor(color);
			geom->DefineVertex(corners[2]);
			geom->DefineColor(color);
			geom->DefineVertex(corners[1]);
			geom->DefineColor(color);

			geom->DefineVertex(corners[1]);
			geom->DefineColor(color);
			geom->DefineVertex(corners[3]);
			geom->DefineColor(color);
			geom->DefineVertex(corners[0]);
			geom->DefineColor(color);
		}
	}
}
