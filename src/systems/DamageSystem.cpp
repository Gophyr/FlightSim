#include "DamageSystem.h"
#include "GameController.h"
#include "GameStateController.h"

#include <iostream>

void handleInstance(DamageInstance& inst, HealthComponent* hp)
{
	hp->health -= inst.amount;
}

void handleShieldedInstance(flecs::entity id, DamageInstance& inst, HealthComponent* hp, ShieldComponent* shield)
{
	f32 overflow = 0;
	if (shield) {
		bool shieldsDown = false;
		if (shield->shields <= 0) shieldsDown = true;
		shield->shields -= inst.amount;
		if (shield->shields <= 0) {
			if (!shieldsDown) audioDriver->playGameSound(id, "shield_down.ogg");
			overflow += -shield->shields;
			shield->shields = 0;
		}
		else {
			if (inst.amount > 10.f) audioDriver->playGameSound(id, "shield_impact_major.ogg");
			else audioDriver->playGameSound(id, "shield_impact_minor.ogg");
		}
		shield->timeSinceLastHit = 0;
	}
	else {
		overflow = inst.amount;
	}
	if (overflow > 0) handleInstance(inst, hp);
}

void handleEMPInstance(DamageInstance& inst, ShieldComponent* shield)
{
	if (shield) {
		shield->shields -= inst.amount;
		shield->timeSinceLastHit = 0;
	}
}

void damageSystem(flecs::iter it, DamageTrackingComponent* dtc, HealthComponent* hc)
{
	for (auto i : it) {
		auto dmg = &dtc[i];
		auto hp = &hc[i];
		auto id = it.entity(i);
		bool hasShld = id.has<ShieldComponent>();
		ShieldComponent* shld = nullptr;
		if (hasShld) shld = id.get_mut<ShieldComponent>();

		for (DamageInstance inst : dmg->instances) {
			switch (inst.type) {
			case DAMAGE_TYPE::NONE:
				break;
			case DAMAGE_TYPE::EXPLOSIVE:
				handleShieldedInstance(id, inst, hp, shld);
				break;
			case DAMAGE_TYPE::IMPACT:
				if (inst.time >= dmg->lastDamageTime + 500) audioDriver->playGameSound(id, "impact_1.ogg");
				handleInstance(inst, hp);
				break;
			case DAMAGE_TYPE::VELOCITY:
				handleInstance(inst, hp);
				break;
			case DAMAGE_TYPE::PLASMA:
				handleShieldedInstance(id, inst, hp, shld);
				break;
			case DAMAGE_TYPE::KINETIC:
				handleShieldedInstance(id, inst, hp, shld);
				break;
			case DAMAGE_TYPE::EMP:
				handleEMPInstance(inst, shld);
				break;
			default: //should be an error here
				break;
			}
			dmg->lastDamageType = inst.type;
			dmg->lastDamageTime = inst.time;
		}
		dmg->instances.clear();
	}
}