#pragma once
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/Scene/Component.h>

using namespace Urho3D;

class Entity  {
public:
	Entity(Vector3 _position, Urho3D::Node* _boxNode);
	~Entity();
	Vector3 getPosition();
	double getMinimalDistance();
	Urho3D::Node * getNode();
	int getLevelOfBucket();
	int getBucketX();
	int getBucketZ();
	bool isAlive();
	bool bucketHasChanged(int posX, int posY);
	void setBucket(int posX, int posY);
	virtual int getType();
protected:
	Urho3D::Node* node;
	Vector3 position;
	Vector3 rotation;
	double minimalDistance;

private:
	int bucketLevel;
	int bucketX, bucketZ;
	bool alive;
	Node* title;
	Text3D* titleText;
};

