#pragma once
#include <Urho3D/Container/Str.h>

constexpr char SPLIT_SIGN = '\n';

struct db_entity {
	const short id;

	explicit db_entity(short id)
		: id(id) {
	}
};

struct db_with_name {
	const Urho3D::String name;

	explicit db_with_name(const Urho3D::String& name)
		: name(name) {
	}
};

template <typename T>
static void setEntity(std::vector<T*>& array, T* entity) {
	auto id = static_cast<db_entity*>(entity)->id;

	if (array.size() <= id) {
		array.resize(id + 1, nullptr);
	}

	array[id] = entity;
}