#include "DebugLineRepo.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "simulation/SimGlobals.h"
#include "utils/defines.h"
#include "utils/OtherUtils.h"

std::vector<Urho3D::CustomGeometry*> DebugLineRepo::geometry[];

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& geom = geometry[cast(type)];
			if (geom.empty()) {
				geom.push_back(Game::getScene()->CreateChild()
				                               ->GetOrCreateComponent<Urho3D::CustomGeometry>());
			}
		}
	}
}

void DebugLineRepo::init(DebugLineType type, short batches) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& geom = geometry[cast(type)];
			if (geom.empty()) {
				for (int i = 0; i < batches; ++i) {
					geom.push_back(Game::getScene()->CreateChild()
					                               ->GetOrCreateComponent<Urho3D::CustomGeometry>());
				}
			}
		}
	}
}

DebugLineRepo::~DebugLineRepo() {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			for (const auto& vector : geometry) {
				for (auto* customGeometry : vector) {
					customGeometry->Remove();
				}
			}
		}
	}
}

void DebugLineRepo::commit(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[cast(type)].at(batch);
			geom->SetMaterial(Game::getColorPaletteRepo()->getLineMaterial());
			geom->Commit();
		}
	}
}

void DebugLineRepo::beginGeometry(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			if (type == DebugLineType::INFLUENCE) {
				geometry[cast(type)].at(batch)->BeginGeometry(0, Urho3D::PrimitiveType::TRIANGLE_LIST);
			} else {
				geometry[cast(type)].at(batch)->BeginGeometry(0, Urho3D::PrimitiveType::LINE_LIST);
			}
		}
	}
}

void DebugLineRepo::clear(DebugLineType type, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[cast(type)].at(batch);
			geom->Clear();
			geom->SetNumGeometries(1);
		}
	}
}

void DebugLineRepo::drawLine(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                             const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[cast(type)].at(batch);
			geom->DefineVertex(first);
			geom->DefineColor(color);
			geom->DefineVertex(second);
			geom->DefineColor(color);
		}
	}
}


void DebugLineRepo::drawTriangle(DebugLineType type, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                                 const Urho3D::Vector3& third, const Urho3D::Color& color, short batch) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometry[cast(type)].at(batch);
			geom->DefineVertex(first);
			geom->DefineColor(color);
			geom->DefineVertex(second);
			geom->DefineColor(color);
			geom->DefineVertex(third);
			geom->DefineColor(color);
		}
	}
}
