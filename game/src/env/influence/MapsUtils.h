#pragma once
#include <vector>


constexpr unsigned char INF_LEVEL = 4;
constexpr unsigned char INF_GRID_FIELD_SIZE = 8;
constexpr unsigned char VISIBILITY_GRID_FIELD_SIZE = INF_GRID_FIELD_SIZE / 2;

namespace MapsUtils {
	template <typename T>
	void resetMaps(const std::vector<T*>& maps) {
		for (auto map : maps) {
			map->reset();
		}
	}

	template <typename T>
	void drawAll(const std::vector<T*>& maps, Urho3D::String name) {
		for (int i = 0; i < maps.size(); ++i) {
			maps[i]->print(name + "_" + Urho3D::String(i) + "_");
		}
	}

	template <typename T>
	void drawMap(unsigned char index, const std::vector<T*>& maps) {
		index = index % maps.size();
		maps[index]->draw();
	}

	template <typename T>
	void drawMapRaw(unsigned char index, const std::vector<T*>& maps) {
		index = index % maps.size();
		maps[index]->drawRaw();
	}

	template <typename T>
	void drawMapKernel(unsigned char index, const std::vector<T*>& maps) {
		index = index % maps.size();
		maps[index]->drawKernel();
	}
}
