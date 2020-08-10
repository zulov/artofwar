#pragma once
#include <array>
#include <vector>
#include <Urho3D/Graphics/BillboardSet.h>

#include "utils/defines.h"


enum class ObjectType : char;

namespace Urho3D
{
	class BillboardSet;
	class Node;
}

class BillboardSetProvider {
public:
	BillboardSetProvider();
	BillboardSetProvider(const BillboardSetProvider& rhs) = delete;
	~BillboardSetProvider();
	void init();
	void reset();
	Urho3D::Billboard* getNextBar(ObjectType type, char player, short id);
	Urho3D::Billboard* getNextAura(ObjectType type, char player, short id);
	void commit();

private:
	Urho3D::BillboardSet* createSet(Urho3D::Node* node, Urho3D::String& materialName, int size) const;
	Urho3D::Node* nodeBar;
	Urho3D::Node* nodeAura;

	std::array<Urho3D::BillboardSet*, 4> resourceAura;
	std::array<Urho3D::BillboardSet*,MAX_PLAYERS> perPlayerAura;

	std::array<Urho3D::BillboardSet*, 4> resourceBar;
	std::array<Urho3D::BillboardSet*,MAX_PLAYERS> perPlayerBar;

	int auraIdx = 0;
	int barIdx = 0;
};
