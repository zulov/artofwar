#pragma once
#include "database/db_struct.h"
#include "objects/Physical.h"
#include "math/MathUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "env/Environment.h"
#include "Game.h"

struct ProjectileBase {
	Physical* aim;

	float percentToGo;
	float speed;
	float attackVal;

	char player;
	bool active = false;

	ProjectileBase() = default;

	ProjectileBase(const ProjectileBase&) = delete;

	virtual void init(Physical* shooter, Physical* aim, float speed, char player) {
		this->aim = aim;
		this->player = player;
		this->attackVal = shooter->getAttackVal(aim);

		const auto dist = (aim->getPosition() - shooter->getPosition()).Length();
		this->speed = speed / dist;

		percentToGo = 1.f;

		active = true;
	}

	virtual bool update(float timeStep) {
		if (aim && !aim->isAlive()) {
			aim = nullptr;
		}
		percentToGo -= speed * timeStep;

		if (percentToGo <= 0.f && active) {
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

	virtual void reset() {
		aim = nullptr;
		active = false;
	}

	bool isActive() const {
		return active;
	}

};
