#pragma once
#include <array>

#include "objects/SelectedObject.h"
#include "utils/defines.h"

constexpr int PLAYER_SIZE = 5000;
constexpr int RESOURCE_SIZE = 1000;
constexpr int VECTOR_SIZE = std::max(PLAYER_SIZE, RESOURCE_SIZE);

class SelectedObject;
enum class ObjectType : char;

namespace Urho3D
{
	class BillboardSet;
	class Node;
	class String;
	struct Billboard;
}

class BillboardSetProvider {
public:
	BillboardSetProvider() = default;
	BillboardSetProvider(const BillboardSetProvider& rhs) = delete;
	~BillboardSetProvider();
	void init();
	void reset();

	SelectedObject* getNext(ObjectType type, char player, short id);
	void commit();

private:
	Urho3D::BillboardSet* createSet(Urho3D::Node* node, Urho3D::String& materialName, int size) const;

	Urho3D::Node* nodeBar{};
	Urho3D::Node* nodeAura{};

	Urho3D::BillboardSet* resourceAura[RESOURCES_SIZE];
	std::array<Urho3D::BillboardSet*, MAX_PLAYERS> perPlayerAura;

	Urho3D::BillboardSet* resourceBar;
	std::array<Urho3D::BillboardSet*, MAX_PLAYERS> perPlayerBar;

	int idx = 0;
	SelectedObject EMPTY;
	SelectedObject* objects{};
};
