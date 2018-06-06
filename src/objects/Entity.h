#pragma once
#include <Urho3D/Scene/Node.h>

enum class ObjectType : char;
using namespace Urho3D;

class Entity
{
public:
	Entity(ObjectType _type);
	virtual ~Entity();

	static std::string getColumns();

	Urho3D::Node* getNode() const { return node; }
	ObjectType getType() const { return type; }

	virtual bool isAlive() const;
	virtual int getDbID();
	virtual std::string getValues(int precision);
protected:
	Urho3D::Node* node;
	ObjectType type;
};
