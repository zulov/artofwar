#pragma once
#include "Entity.h"
#include <vector>

using namespace Urho3D;

class Unit : public Entity
{
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode, Font* _font);
	~Unit();

	void move(double timeStep);
	void applyForce(double timeStep);
	virtual int getType() override;
	void select() override;
	void unSelect() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();
	virtual void action(ActionType actionType, Entity* entity) override;
	Vector3* getAim();
	Vector3* getVelocity();
	double getMass();
	double getUnitRadius();
protected:
	Vector3* acceleration;
	Vector3* velocity;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	std::vector<Urho3D::Vector3*>* aims;
private:
	void addAim(Entity* entity);
	double unitRadius = 2;
};
