#pragma once

#include <magic_enum.hpp>

enum class ResourceType : unsigned char {
	FOOD = 0,
	WOOD = 1,
	STONE = 2,
	GOLD = 3
};

constexpr int cast(ResourceType rt) { return static_cast<int>(rt); }
constexpr unsigned char RESOURCES_SIZE = magic_enum::enum_count<ResourceType>();

static_assert(cast(ResourceType::FOOD) == 0 && cast(ResourceType::WOOD) == 1
              && cast(ResourceType::STONE) == 2 && cast(ResourceType::GOLD) == 3,
              "ResourceType must stay FOOD,WOOD,STONE,GOLD == 0,1,2,3 (parallel per-resource arrays depend on this)");

