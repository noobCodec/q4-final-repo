
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class fruit : public idAI {
public:

	CLASS_PROTOTYPE(fruit);

	fruit(void);

	void				Spawn(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);

	virtual bool		Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location);


protected:

	virtual bool		CheckActions(void);

	stateResult_t		State_Combat(const stateParms_t& parms);
	stateResult_t		State_Killed(const stateParms_t& parms);

private:
	int time;
	int state = 0;
	int growidx = 1;
	CLASS_STATES_PROTOTYPE(fruit);
};

CLASS_DECLARATION(idAI, fruit)
END_CLASS

/*
================
rvMonsterTurret::rvMonsterTurret
================
*/
fruit::fruit() {
}

/*
================
rvMonsterTurret::Spawn
================
*/
void fruit::Spawn(void) {
	time = gameLocal.GetTime();
}

/*
================
rvMonsterTurret::Save
================
*/
void fruit::Save(idSaveGame* savefile) const {
	savefile->WriteInt(time);
}

/*
================
rvMonsterTurret::Restore
================
*/
void fruit::Restore(idRestoreGame* savefile) {
	savefile->ReadInt(time);
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool fruit::CheckActions(void) {
	return true;
}

/*
================
rvMonsterTurret::Pain
================
*/
bool fruit::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
	return idAI::Pain(inflictor, attacker, damage, dir, location);
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(fruit)
STATE("State_Combat", fruit::State_Combat)
STATE("State_Killed", fruit::State_Killed)
END_CLASS_STATES

/*
================
rvMonsterTurret::State_Combat
================
*/
stateResult_t fruit::State_Combat(const stateParms_t& parms) {
	return SRESULT_WAIT;
}

/*
================
rvMonsterTurret::State_Killed
================
*/
stateResult_t fruit::State_Killed(const stateParms_t& parms) {
	idEntity* ptr = GetDamageEntity();
	idDict tmp;
	idPlayer* player = gameLocal.GetLocalPlayer();
	idStr drops[11] = { "item_health_small","weapon_grenadelauncher","weapon_hyperblaster","first_seed","second_seed","third_seed","weapon_lightninggun","weapon_machinegun","weapon_railgun","weapon_rocketlauncher","weapon_shotgun" };
	tmp.Copy(*gameLocal.FindEntityDefDict(drops[gameLocal.random.RandomInt(11)]));
	tmp.Set("origin", GetPhysics()->GetOrigin().ToString());
	idEntity* newEnt;
	gameLocal.SpawnEntityDef(tmp, &newEnt);
	if(ptr)
	static_cast<idAI*>(ptr)->Pain(this, NULL, 0, GetPhysics()->GetOrigin(), 0);
	tmp.Copy(*gameLocal.FindEntityDefDict("third_seed"));
	player->GiveInventoryItem(&tmp);
	return idAI::State_Killed(parms);
}

