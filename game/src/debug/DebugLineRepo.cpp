#include "DebugLineRepo.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "simulation/SimGlobals.h"
#include "utils/OtherUtils.h"

std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> DebugLineRepo::geometries;

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& geom = geometries[castC(type)];
			if (!geom) { geom = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>(); }
		}
	}
}

void DebugLineRepo::dispose() {
	//czy to cale jest konieczne
	for (auto& customGeometries : geometries) {
		customGeometries->Remove(); // nie wiem czy konieczne
		customGeometries = nullptr;
	}
}

DebugLineRepo::~DebugLineRepo() {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) { for (const auto& customGeometry : geometries) { customGeometry->Remove(); } }
	}
}

void DebugLineRepo::commit(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometries[castC(type)];
			switch (type) {
			case DebugLineType::GRID:
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
			const auto primitiveType = (type == DebugLineType::GRID)
					? Urho3D::PrimitiveType::TRIANGLE_LIST
					: Urho3D::PrimitiveType::LINE_LIST;

			geometries[castC(type)]->BeginGeometry(0, primitiveType);
		}
	}
}

void DebugLineRepo::clear(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			if (auto* geom = geometries[castC(type)]) {
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
			auto* geom = geometries[castC(type)];
			geom->DefineVertex(first);
			geom->DefineColor(color);
			geom->DefineVertex(second);
			geom->DefineColor(color);
		}
	}
}

void DebugLineRepo::drawQuads(DebugLineType type, unsigned short resolution, float*values , float maxValue) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& quads = quadCords[resolution];
			if (quads.empty()) {
				
			}
			auto colors = Game::getColorPaletteRepo();
			auto* geom = geometries[castC(type)];
			for (int i = 0; i < resolution * resolution; ++i) {
				auto color = colors->getColor(values[i], maxValue);
				auto& corners = quads[i];

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
}

