#include "DebugLineRepo.h"
#include <cassert>
#include <Urho3D/Graphics/CustomGeometry.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "env/Environment.h"
#include "env/EnvConsts.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"
#include "simulation/SimGlobals.h"
#include "utils/OtherUtils.h"

std::array<Urho3D::CustomGeometry*, magic_enum::enum_count<DebugLineType>()> DebugLineRepo::geometries;
std::unordered_map<unsigned short, std::vector<std::array<Urho3D::Vector3, 4>>> DebugLineRepo::quadCords;

void DebugLineRepo::defineQuad(Urho3D::CustomGeometry* geom, const std::array<Urho3D::Vector3, 4>& corners,
	                       const Urho3D::Color& color) {
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

void DebugLineRepo::fillQuad(Urho3D::CustomGeometry* geom, size_t index, const std::array<Urho3D::Vector3, 4>& corners,
	                     const Urho3D::Color& color) {
	const auto baseVertex = static_cast<unsigned>(index) * 6;
	const auto colorValue = color.ToUInt();
	auto* vertex = geom->GetVertex(0, baseVertex);
	assert(vertex);

	vertex[0].position_ = corners[0];
	vertex[0].color_ = colorValue;
	vertex[1].position_ = corners[2];
	vertex[1].color_ = colorValue;
	vertex[2].position_ = corners[1];
	vertex[2].color_ = colorValue;

	vertex[3].position_ = corners[1];
	vertex[3].color_ = colorValue;
	vertex[4].position_ = corners[3];
	vertex[4].color_ = colorValue;
	vertex[5].position_ = corners[0];
	vertex[5].color_ = colorValue;
}

std::vector<std::array<Urho3D::Vector3, 4>>& DebugLineRepo::getQuadCords(unsigned short resolution) {
	auto& quads = quadCords[resolution];
	const auto expectedSize = static_cast<size_t>(resolution) * resolution;
	if (quads.size() != expectedSize) {
		const auto* environment = Game::getEnvironment();
		assert(environment);
		const float mapSize = static_cast<float>(environment->getResolution()) * BUCKET_GRID_FIELD_SIZE;
		const auto* calculator = GridCalculatorProvider::get(resolution, mapSize);
		quads.resize(expectedSize);
		const float halfField = calculator->getFieldSize() / 2.3f;

		for (size_t i = 0; i < expectedSize; ++i) {
			const auto center = calculator->getCenter(static_cast<int>(i));
			auto& corners = quads[i];
			corners[0] = environment->getPosWithHeightAt(center.x_ - halfField, center.y_ + halfField);
			corners[1] = environment->getPosWithHeightAt(center.x_ + halfField, center.y_ - halfField);
			corners[2] = environment->getPosWithHeightAt(center.x_ + halfField, center.y_ + halfField);
			corners[3] = environment->getPosWithHeightAt(center.x_ - halfField, center.y_ - halfField);
			for (auto& corner : corners) {
				corner.y_ += 1.f;
			}
		}
	}

	return quads;
}

void DebugLineRepo::init(DebugLineType type) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& geom = geometries[castC(type)];
			if (!geom) {
				geom = Game::getScene()->CreateChild()->GetOrCreateComponent<Urho3D::CustomGeometry>();
				geom->SetDynamic(true);
			}
		}
	}
}

void DebugLineRepo::dispose() {
	for (auto& customGeometries : geometries) {
		customGeometries = nullptr;
	}
	quadCords.clear();
}

DebugLineRepo::~DebugLineRepo() {
	dispose();
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

void DebugLineRepo::drawQuads(DebugLineType type, unsigned short resolution, const unsigned char* values,
	                     float maxValue) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& quads = getQuadCords(resolution);
			auto* colors = Game::getColorPaletteRepo();
			auto* geom = geometries[castC(type)];
			if (type == DebugLineType::GRID) {
				ensureGridGeometry(geom, resolution);
				for (size_t i = 0; i < quads.size(); ++i) {
					fillQuad(geom, i, quads[i], colors->getColor(values[i], maxValue));
				}
			} else {
				for (size_t i = 0; i < quads.size(); ++i) {
					defineQuad(geom, quads[i], colors->getColor(values[i], maxValue));
				}
			}
		}
	}
}

void DebugLineRepo::drawQuads(DebugLineType type, unsigned short resolution, const float* values, float maxValue) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto& quads = getQuadCords(resolution);
			auto* colors = Game::getColorPaletteRepo();
			auto* geom = geometries[castC(type)];
			if (type == DebugLineType::GRID) {
				ensureGridGeometry(geom, resolution);
				for (size_t i = 0; i < quads.size(); ++i) {
					fillQuad(geom, i, quads[i], colors->getColor(values[i], maxValue));
				}
			} else {
				for (size_t i = 0; i < quads.size(); ++i) {
					defineQuad(geom, quads[i], colors->getColor(values[i], maxValue));
				}
			}
		}
	}
}

void DebugLineRepo::drawQuads(DebugLineType type, unsigned short resolution, const std::vector<Urho3D::Color>& colors) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (!SIM_GLOBALS.HEADLESS) {
			auto* geom = geometries[castC(type)];
			auto& quads = getQuadCords(resolution);
			assert(colors.size() == quads.size());

			if (type == DebugLineType::GRID) {
				ensureGridGeometry(geom, resolution);
				for (size_t i = 0; i < quads.size(); ++i) {
					fillQuad(geom, i, quads[i], colors[i]);
				}
			} else {
				for (size_t i = 0; i < quads.size(); ++i) {
					if (colors[i].a_ > 0.f) {
						defineQuad(geom, quads[i], colors[i]);
					}
				}
			}
		}
	}
}

void DebugLineRepo::ensureGridGeometry(Urho3D::CustomGeometry* geom, unsigned short resolution) {
	if (!geom || resolution == 0) {
		return;
	}

	const auto expectedVertices = static_cast<unsigned>(resolution) * resolution * 6u;
	if (geom->GetNumVertices(0) == expectedVertices) {
		return;
	}

	auto& quads = getQuadCords(resolution);
	geom->Clear();
	geom->SetNumGeometries(1);
	geom->DefineGeometry(0, Urho3D::PrimitiveType::TRIANGLE_LIST, expectedVertices, false, true, false, false);

	for (size_t i = 0; i < quads.size(); ++i) {
		fillQuad(geom, i, quads[i], Urho3D::Color::WHITE);
	}
}
