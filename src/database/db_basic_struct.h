#pragma once
#include <Urho3D/Container/Str.h>

constexpr char SPLIT_SIGN = ';';

struct db_entity {
	const short id;

	explicit db_entity(short id) : id(id) {
	}
};

struct db_with_name {
	const Urho3D::String name;

	explicit db_with_name(const Urho3D::String& name) : name(name) {
	}
};

template <typename T>
void ensureSize(int size, std::vector<T*>& array) {
	if (array.size() <= size) {
		array.resize(size + 1, nullptr);
	}
}

template <typename T>
static void setEntity(std::vector<T*>& array, T* entity) {
	auto id = static_cast<db_entity*>(entity)->id;

	ensureSize(id, array);

	array[id] = entity;
}
