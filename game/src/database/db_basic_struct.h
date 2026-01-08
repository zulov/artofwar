#pragma once
#include <Urho3D/Container/Str.h>

constexpr char SPLIT_SIGN = ';';
constexpr char SPLIT_SIGN_2 = '|';

struct db_entity {
	const short id;

	explicit db_entity(short id) : id(id) {
	}
};

struct db_with_name : db_entity {
	const Urho3D::String name;

	db_with_name(short id, const Urho3D::String name)
		: db_entity(id), name(name) {
	}
};

struct db_with_icon : db_with_name {
	const Urho3D::String icon;

	db_with_icon(short id, const Urho3D::String name, const Urho3D::String icon)
		: db_with_name(id, name),  icon(icon) {
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

inline bool s2b(const char* s) {
	assert(s[1] == '\0');
	return s && s[0] == '1';
}