#pragma once
#include <Urho3D/Scene/Node.h>

enum class ObjectType : char;

class Entity
{
public:
	explicit Entity();
	virtual ~Entity();

	static std::string getColumns();

	Urho3D::Node* getNode() const { return node; }
	virtual ObjectType getType() const;

	virtual bool isAlive() const;
	virtual int getDbID();
	virtual std::string getValues(int precision);
protected:
	Urho3D::Node* node;
};
