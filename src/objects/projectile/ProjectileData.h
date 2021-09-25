#pragma once
#include "objects/NodeUtils.h"
#include "math/MathUtils.h"
#include <Urho3D/Math/Vector3.h>

#include "math/RandGen.h"
#include "objects/Physical.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"

struct ProjectileData {
	Urho3D::Vector2 direction;
	Urho3D::Node* node;
	Physical* aim;

	float startHeight;
	float endHeight;

	float distance;
	float distanceSoFar;

	float peakHeight;
	float speed;

	float attackVal;

	char player;
	bool active = false;

	ProjectileData(Urho3D::Node* node): node(node) {
	}

	~ProjectileData() {
		// if (node) {
		// 	node->Remove();
		// }
	}

	ProjectileData(const ProjectileData&) = delete;

	void init(const Urho3D::Vector3& start, Physical* aim, float speed, char player, float attackVal) {
		this->speed = speed;
		this->aim = aim;
		this->player = player;
		this->attackVal = attackVal;

		auto end = aim->getPosition();

		direction = dirTo(start, end);
		distance = direction.Length();

		distanceSoFar = 0;
		if (node) {
			const auto model = node->GetComponent<Urho3D::StaticModel>();
			if (aim->getType() == ObjectType::BUILDING) {
				changeMaterial("Materials/projectiles/black.xml", model);
			} else {
				changeMaterial("Materials/projectiles/brown.xml", model);
			}

			end.y_ += aim->getModelHeight() / (RandGen::nextRand(RandFloatType::OTHER, 3) + 2.f);
			startHeight = start.y_;

			endHeight = end.y_;
			peakHeight = distance / (RandGen::nextRand(RandFloatType::OTHER, 3) + 4.1f);
			direction.Normalize();
			direction *= speed;
			node->SetEnabled(true);
			node->SetPosition(start);
		}

		active = true;
	}

	bool update(float timeStep) {
		if (aim && !aim->isAlive()) {
			aim = nullptr;
		}
		distanceSoFar += speed * timeStep;
		if (node) {
			auto pos = node->GetPosition();

			pos.x_ += direction.x_ * timeStep;
			pos.z_ += direction.y_ * timeStep;
			pos.y_ = sin(distanceSoFar * Urho3D::M_PI / distance) * peakHeight + startHeight * (distance -
					distanceSoFar) /
				distance + distanceSoFar / distance * endHeight;
			node->SetDirection(pos - node->GetPosition());
			node->SetPosition(pos);
		}
		if (distanceSoFar >= distance && active) {
			if (aim && aim->isAlive()) {
				const auto [value, died] = aim->absorbAttack(attackVal);
				Game::getEnvironment()->addAttack(player, aim->getPosition(), value);
				if (died) {
					Game::getPlayersMan()->getPlayer(player)->addKilled(aim);
				}
			}

			reset();
			return true;
		}
		return false;
	}

	void reset() {
		if (node) {
			node->SetEnabled(false);
		}
		aim = nullptr;
		active = false;
	}

	bool isActive() const {
		return active;
	}

};
