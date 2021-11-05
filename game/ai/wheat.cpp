
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class wheat : public idAI {
public:

	CLASS_PROTOTYPE(wheat);

	wheat(void);

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
	CLASS_STATES_PROTOTYPE(wheat);
};

CLASS_DECLARATION(idAI, wheat)
END_CLASS

/*
================
rvMonsterTurret::rvMonsterTurret
================
*/
wheat::wheat() {
}

/*
================
rvMonsterTurret::Spawn
================
*/
void wheat::Spawn(void) {
	time = gameLocal.GetTime();
	common->Printf("spawned");

}

/*
================
rvMonsterTurret::Save
================
*/
void wheat::Save(idSaveGame* savefile) const {
	savefile->WriteInt(time);
}

/*
================
rvMonsterTurret::Restore
================
*/
void wheat::Restore(idRestoreGame* savefile) {
	savefile->ReadInt(time);
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool wheat::CheckActions(void) {
	if (state < 3 && (time + growidx * 5000) < gameLocal.GetTime()) {
		switch (state) {
		case 0:
			SetModel("env_ragdoll_strogg_test");
			break;
		case 1:
			SetModel("model_monster_heavy_hovertank");
			break;
		case 2:
			SetModel("model_monster_gunner");
			break;
		}
		growidx++;
		state++;
	}
	return true;
}

/*
================
rvMonsterTurret::Pain
================
*/
bool wheat::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
	return idAI::Pain(inflictor, attacker, damage, dir, location);
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(wheat)
STATE("State_Combat", wheat::State_Combat)
STATE("State_Killed", wheat::State_Killed)
END_CLASS_STATES

/*
================
rvMonsterTurret::State_Combat
================
*/
stateResult_t wheat::State_Combat(const stateParms_t& parms) {
	// Aquire a new enemy if we dont have one
	if (!enemy.ent) {
		CheckForEnemy(true);
	}

	FaceEnemy();

	// try moving, if there was no movement run then just try and action instead
	UpdateAction();

	return SRESULT_WAIT;
}

/*
================
rvMonsterTurret::State_Killed
================
*/
stateResult_t wheat::State_Killed(const stateParms_t& parms) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	idDict tmp;
	common->Printf("dead");
	common->Printf("%d", state);
	if (state == 3)
	{
		tmp.Copy(*gameLocal.FindEntityDefDict("first_seed"));
		player->GiveInventoryItem(&tmp);
	}
	return idAI::State_Killed(parms);
}

