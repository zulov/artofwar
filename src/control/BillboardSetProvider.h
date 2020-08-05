#pragma once
#include <vector>
#include <Urho3D/Graphics/BillboardSet.h>


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
	Urho3D::Billboard* getNextBar(ObjectType type, char player, char id);
	Urho3D::Billboard* getNextAura(ObjectType type, char player, char id);
	void commit();

private:
	Urho3D::BillboardSet* createSet(Urho3D::Node* node, Urho3D::String& materialName) const;
	Urho3D::Node* node;
	Urho3D::Node* nodeAura;

	std::vector<Urho3D::BillboardSet*> resourceAura;
	std::vector<Urho3D::BillboardSet*> perPlayerAura;

	std::vector<Urho3D::BillboardSet*> resourceBar;
	std::vector<Urho3D::BillboardSet*> perPlayerBar;
	
	int auraIdx = 0;
	int barIdx = 0;
};
