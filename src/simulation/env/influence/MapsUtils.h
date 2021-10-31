#pragma once
#include <vector>


constexpr char MAX_DEBUG_PARTS_INFLUENCE = 32;
constexpr char INF_LEVEL = 4;
constexpr short INF_GRID_FIELD_SIZE = 8.f;
constexpr short VISIBILITY_GRID_FIELD_SIZE = INF_GRID_FIELD_SIZE / 2;

namespace MapsUtils {
	template <typename T>
	void resetMaps(const std::vector<T*>& maps) {
		for (auto map : maps) {
			map->reset();
		}
	}

	template <typename T>
	void finalize(const std::vector<T*>& maps) {
		for (auto map : maps) {
			map->updateFromTemp();
		}
	}

	template <typename T>
	void drawAll(const std::vector<T*>& maps, Urho3D::String name) {
		for (int i = 0; i < maps.size(); ++i) {
			maps[i]->print(name + "_" + Urho3D::String(i) + "_");
		}
	}

	template <typename T>
	void drawMap(short currentDebugBatch, char index, const std::vector<T*>& maps) {
		index = index % maps.size();
		maps[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
	}
}