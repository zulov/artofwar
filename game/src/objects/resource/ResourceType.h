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
