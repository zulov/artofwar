#pragma once
#include <Urho3D/Math/Vector3.h>
#include "math/MathUtils.h"
#include "objects/NodeUtils.h"

#include "ProjectileBase.h"
#include "math/RandGen.h"

struct ProjectileWithNode : public ProjectileBase {
	Urho3D::Node* node;
	Urho3D::Vector2 direction;
	float startHeight;
	float endHeight;

	float peakHeight;

	ProjectileWithNode(Urho3D::Node* node) : ProjectileBase(), node(node) {}

	~ProjectileWithNode() {
		// if (node) {node->Remove(); }
	}

	ProjectileWithNode(const ProjectileWithNode&) = delete;

	void init(Physical* shooter, Physical* aim, float speed, char player) override {
		ensureNode();
		ProjectileBase::init(shooter, aim, speed, player);

		const auto end = aim->getNode()->GetPosition();
		const auto start = shooter->getNode()->GetPosition();
		direction = Urho3D::Vector2(end.x_ - start.x_, end.z_ - start.z_);
		const auto model = node->GetComponent<Urho3D::StaticModel>();
		if (aim->getType() == ObjectType::BUILDING) {
			changeMaterial("Materials/projectiles/black.xml", model);
		} else {
			changeMaterial("Materials/projectiles/brown.xml", model);
		}

		startHeight = start.y_ + shooter->getModelHeight() * 0.9f;

		endHeight = end.y_ + aim->getModelHeight() / (RandGen::nextRand(RandFloatType::OTHER, 3) + 2.f);
		peakHeight = direction.Length() / (RandGen::nextRand(RandFloatType::OTHER, 3) + 4.1f);
		direction.ScaleTo(speed);
		node->SetEnabled(true);
		node->SetPosition(start);
	}

	bool update(float timeStep) override {
		const bool result = ProjectileBase::update(timeStep);

		auto pos = node->GetPosition();

		pos.x_ += direction.x_ * timeStep;
		pos.z_ += direction.y_ * timeStep;
		const auto percent = 1.f - percentToGo;
		pos.y_ = peakHeight * sin(percent * Urho3D::M_PI) + startHeight * percentToGo + endHeight * percent;
		node->SetDirection(pos - node->GetPosition());
		node->SetPosition(pos);

		return result;
	}

	void reset() override {
		ProjectileBase::reset();
		if (node) {
			node->SetEnabled(false);
		}
	}

	void clearNodeWithoutDelete() override { node = nullptr; }

	void loadState(const ProjectileSaveData& state, Physical* target) override {
		ProjectileBase::loadState(state, target);
		if (!target || !target->getNode()) {
			return;
		}
		ensureNode();

		const auto targetPosition = target->getNode()->GetPosition();
		const auto targetHeight = target->getModelHeight();
		constexpr float VISUAL_SPEED = 7.f;
		direction = {VISUAL_SPEED, 0.f};
		startHeight = targetPosition.y_ + targetHeight * 0.9f;
		endHeight = targetPosition.y_ + targetHeight * 0.5f;
		peakHeight = Urho3D::Max(direction.Length() * (percentToGo + 0.5f), 1.f);

		const auto model = node->GetComponent<Urho3D::StaticModel>();
		if (target->getType() == ObjectType::BUILDING) {
			changeMaterial("Materials/projectiles/black.xml", model);
		} else {
			changeMaterial("Materials/projectiles/brown.xml", model);
		}
		const auto remainingTime = speed > 0.f ? percentToGo / speed : 0.f;
		const auto position = targetPosition - Urho3D::Vector3(direction.x_ * remainingTime, 0.f, direction.y_ * remainingTime);
		const auto progress = 1.f - percentToGo;
		const auto height = peakHeight * sin(progress * Urho3D::M_PI) + startHeight * percentToGo + endHeight * progress;
		node->SetPosition({position.x_, height, position.z_});
		node->SetEnabled(true);
	}

private:
	void ensureNode() {
		if (!node) {
			node = createNode("Objects/projectiles/arrow.xml");
		}
	}
};
