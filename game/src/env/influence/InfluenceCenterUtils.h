#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>

#include "math/MathUtils.h"

namespace InfluenceCenterUtils {
	inline void rebuildQuad(std::span<const float> rawValues, unsigned short resolution,
	                        std::span<std::span<float>> quadLayers,
	                        std::span<const unsigned short> quadResolutions) {
		assert(!quadLayers.empty());
		assert(quadLayers.size() == quadResolutions.size());
		assert(rawValues.size() == resolution * resolution);

		std::span<const float> parent = rawValues;
		unsigned short parentResolution = resolution;
		for (int i = static_cast<int>(quadLayers.size()) - 1; i >= 0; --i) {
			auto current = quadLayers[i];
			const auto currentResolution = quadResolutions[i];
			assert(parentResolution == currentResolution * 2);
			assert(current.size() == currentResolution * currentResolution);

			for (int parentRow = 0; parentRow < parentResolution; parentRow += 2) {
				const int row0 = parentRow * parentResolution;
				const int row1 = row0 + parentResolution;
				int child = (parentRow >> 1) * currentResolution;

				for (int parentColumn = 0; parentColumn < parentResolution; parentColumn += 2, ++child) {
					const int index = row0 + parentColumn;
					current[child] = parent[index] + parent[index + 1] + parent[row1 + parentColumn]
					                 + parent[row1 + parentColumn + 1];
				}
			}

			parent = current;
			parentResolution = currentResolution;
		}
	}

	inline std::optional<unsigned> findCenterIndex(std::span<const float> rawValues,
	                                               std::span<const std::span<float>> quadLayers,
	                                               std::span<const unsigned short> quadResolutions) {
		assert(!quadLayers.empty());
		assert(quadLayers.size() == quadResolutions.size());

		const auto maxChild = [](const std::array<int, 4>& indexes, std::span<const float> values) {
			return static_cast<unsigned>(*std::max_element(indexes.begin(), indexes.end(),
			                                                [values](int left, int right) {
				                                                	return values[left] < values[right];
			                                                }));
		};

		const auto& smallestLayer = quadLayers[0];
		const auto hasValues = std::ranges::any_of(smallestLayer, [](float value) { return value > 0.f; });
		if (!hasValues) {
			return std::nullopt;
		}

		int index = static_cast<int>(std::distance(smallestLayer.begin(), std::ranges::max_element(smallestLayer)));
		unsigned short resolution = quadResolutions[0];
		for (int i = 1; i < static_cast<int>(quadLayers.size()); ++i) {
			index = maxChild(getCordsInHigher(resolution, index), quadLayers[i]);
			resolution *= 2;
		}
		return maxChild(getCordsInHigher(resolution, index), rawValues);
	}
}
