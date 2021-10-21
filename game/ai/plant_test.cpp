

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class plant_test : public idAI {
public:

	CLASS_PROTOTYPE(plant_test);

	plant_test(void);

	void				InitSpawnArgsVariables(void);
	void				Spawn(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);

	virtual bool		Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location);

protected:

	virtual bool		CheckActions(void);

	stateResult_t		State_Combat(const stateParms_t& parms);
	stateResult_t		State_Killed(const stateParms_t& parms);

	int					shieldHealth;
	int					maxShots;
	int					minShots;
	int					shots;

private:

	rvAIAction			actionBlasterAttack;

	stateResult_t		State_Torso_BlasterAttack(const stateParms_t& parms);

	CLASS_STATES_PROTOTYPE(plant_test);
};

CLASS_DECLARATION(idAI, plant_test)
END_CLASS

/*
================
rvMonsterTurret::rvMonsterTurret
================
*/
plant_test::plant_test() {
	shieldHealth = 0;
}

void plant_test::InitSpawnArgsVariables(void) {
	maxShots = spawnArgs.GetInt("maxShots", "1");
	minShots = spawnArgs.GetInt("minShots", "1");
}
/*
================
rvMonsterTurret::Spawn
================
*/
void plant_test::Spawn(void) {
	actionBlasterAttack.Init(spawnArgs, "action_blasterAttack", "Torso_BlasterAttack", AIACTIONF_ATTACK);

	shieldHealth = spawnArgs.GetInt("shieldHealth");
	health += shieldHealth;

	InitSpawnArgsVariables();
	shots = 0;
}

/*
================
rvMonsterTurret::Save
================
*/
void plant_test::Save(idSaveGame* savefile) const {
	savefile->WriteInt(shieldHealth);
	savefile->WriteInt(shots);
	actionBlasterAttack.Save(savefile);
}

/*
================
rvMonsterTurret::Restore
================
*/
void plant_test::Restore(idRestoreGame* savefile) {
	savefile->ReadInt(shieldHealth);
	savefile->ReadInt(shots);
	actionBlasterAttack.Restore(savefile);

	InitSpawnArgsVariables();
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool plant_test::CheckActions(void) {
	// Attacks
	if (PerformAction(&actionBlasterAttack, (checkAction_t)&idAI::CheckAction_RangedAttack, &actionTimerRangedAttack)) {
		return true;
	}
	return idAI::CheckActions();
}

/*
================
rvMonsterTurret::Pain
================
*/
bool plant_test::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
	// Handle the shield effects
	if (shieldHealth > 0) {
		shieldHealth -= damage;
		if (shieldHealth <= 0) {
			PlayEffect("fx_shieldBreak", GetPhysics()->GetOrigin(), (-GetPhysics()->GetGravityNormal()).ToMat3());
		}
		else {
			PlayEffect("fx_shieldHit", GetPhysics()->GetOrigin(), (-GetPhysics()->GetGravityNormal()).ToMat3());
		}
	}

	return idAI::Pain(inflictor, attacker, damage, dir, location);
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(plant_test)
STATE("State_Combat", plant_test::State_Combat)
STATE("State_Killed", plant_test::State_Killed)

STATE("Torso_BlasterAttack", plant_test::State_Torso_BlasterAttack)
END_CLASS_STATES

/*
================
rvMonsterTurret::State_Combat
================
*/
stateResult_t plant_test::State_Combat(const stateParms_t& parms) {
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
stateResult_t plant_test::State_Killed(const stateParms_t& parms) {
	gameLocal.PlayEffect(gameLocal.GetEffect(spawnArgs, "fx_death"), GetPhysics()->GetOrigin(), (-GetPhysics()->GetGravityNormal()).ToMat3());
	return idAI::State_Killed(parms);
}

/*
================
rvMonsterTurret::State_Torso_BlasterAttack
================
*/
stateResult_t plant_test::State_Torso_BlasterAttack(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_FIRE,
		STAGE_WAIT,
	};
	switch (parms.stage) {
	case STAGE_INIT:
		DisableAnimState(ANIMCHANNEL_LEGS);
		shots = (minShots + gameLocal.random.RandomInt(maxShots - minShots + 1)) * combat.aggressiveScale;
		return SRESULT_STAGE(STAGE_FIRE);

	case STAGE_FIRE:
		PlayAnim(ANIMCHANNEL_TORSO, "range_attack", 2);
		return SRESULT_STAGE(STAGE_WAIT);

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_TORSO, 2)) {
			if (--shots <= 0) {
				return SRESULT_DONE;
			}
			return SRESULT_STAGE(STAGE_FIRE);
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*//spawn in a new Makron leg,			
	idDict	  args;
	idEntity*  newLegs;

	//We may need to do this at a later point
	//SetSkin ( declManager->FindSkin	 ( spawnArgs.GetString ( "skin_legs" ) ) );

	args.Copy ( *gameLocal.FindEntityDefDict ( "monster_makron_legs" ) );
	args.SetVector ( "origin", GetPhysics()->GetOrigin() );
	args.SetInt ( "angle", move.current_yaw );
	gameLocal.SpawnEntityDef ( args, &newLegs );

	//store the name of the entity in the Makron's keys so we can burn it out as well.
	spawnArgs.Set( "legs_name", newLegs->GetName() );*/
