#pragma once
#include "ObjectEnums.h"
#include <Urho3D/Scene/Node.h>


using namespace Urho3D;

class Entity
{
public:
	Entity(ObjectType _type);
	virtual ~Entity();

	Urho3D::Node* getNode();
	virtual bool isAlive() const;
	ObjectType getType();
	virtual int getDbID();
	virtual std::string getValues(int precision);
	static std::string getColumns();

protected:
	Urho3D::Node* node;
	ObjectType type;

};
