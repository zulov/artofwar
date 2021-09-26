#include "database/db_strcut.h"
#include "objects/Physical.h"
#include "math/MathUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include "Game.h"

struct ProjectileBase {
	Physical* aim;

	float distance;
	float distanceSoFar;

	float speed;

	float attackVal;

	char player;
	bool active = false;

	ProjectileBase() = default;

	ProjectileBase(const ProjectileBase&) = delete;

	virtual void init(Physical* shooter, Physical* aim, float speed, char player, db_attack* dbAttack) {
		this->speed = speed;
		this->aim = aim;
		this->player = player;
		if (aim->getType() == ObjectType::BUILDING) {
			this->attackVal = dbAttack->buildingAttackVal;
		} else {
			this->attackVal = dbAttack->rangeAttackVal;
		}
		this->attackVal = attackVal;
		const auto start = shooter->getPosition();
		const auto end = aim->getPosition();

		distance = sqrt(sqDistAs2D(start, end));

		distanceSoFar = 0;

		active = true;
	}

	virtual bool update(float timeStep) {
		if (aim && !aim->isAlive()) {
			aim = nullptr;
		}
		distanceSoFar += speed * timeStep;

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

	virtual void reset() {
		aim = nullptr;
		active = false;
	}

	bool isActive() const {
		return active;
	}


};
