#pragma once
#include "objects/NodeUtils.h"
#include "math/MathUtils.h"
#include <Urho3D/Math/Vector3.h>

#include "ProjectileBase.h"
#include "math/RandGen.h"
#include "player/Player.h"
#include "env/Environment.h"

struct ProjectileWithNode : public ProjectileBase {
	Urho3D::Node* node;
	Urho3D::Vector2 direction;
	float startHeight;
	float endHeight;

	float peakHeight;

	ProjectileWithNode(Urho3D::Node* node): ProjectileBase(), node(node) {
	}

	~ProjectileWithNode() {
		// if (node) {node->Remove(); }
	}

	ProjectileWithNode(const ProjectileWithNode&) = delete;

	void init(Physical* shooter, Physical* aim, float speed, char player) override {
		ProjectileBase::init(shooter, aim, speed, player);

		const auto end = aim->getPosition();
		const auto start = shooter->getPosition();
		direction = dirTo(start, end);
		const auto model = node->GetComponent<Urho3D::StaticModel>();
		if (aim->getType() == ObjectType::BUILDING) {
			changeMaterial("Materials/projectiles/black.xml", model);
		} else {
			changeMaterial("Materials/projectiles/brown.xml", model);
		}

		startHeight = start.y_ + shooter->getModelHeight() * 0.9f;

		endHeight = end.y_ + aim->getModelHeight() / (RandGen::nextRand(RandFloatType::OTHER, 3) + 2.f);
		peakHeight = direction.Length() / (RandGen::nextRand(RandFloatType::OTHER, 3) + 4.1f);
		scaleTo(direction, speed);
		node->SetEnabled(true);
		node->SetPosition(start);
	}

	bool update(float timeStep) override {
		const bool result = ProjectileBase::update(timeStep);

		auto pos = node->GetPosition();

		pos.x_ += direction.x_ * timeStep;
		pos.z_ += direction.y_ * timeStep;
		const auto percent = 1.f - percentToGo;
		pos.y_ = peakHeight * sin(percent * Urho3D::M_PI)
			+ startHeight * percentToGo
			+ endHeight * percent;
		node->SetDirection(pos - node->GetPosition());
		node->SetPosition(pos);

		return result;
	}

	void reset() override {
		ProjectileBase::reset();
		node->SetEnabled(false);
	}

};
