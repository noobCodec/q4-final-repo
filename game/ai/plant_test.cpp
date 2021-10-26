
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class plant_test : public idAI {
public:

	CLASS_PROTOTYPE(plant_test);

	plant_test(void);

	void				Spawn(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);
	
	virtual bool		Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location);

protected:

	virtual bool		CheckActions(void);

	stateResult_t		State_Combat(const stateParms_t& parms);
	stateResult_t		State_Killed(const stateParms_t& parms);

private:
	idEntity* seeds[10]		;
	char i = 'a';
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
}

/*
================
rvMonsterTurret::Spawn
================
*/
void plant_test::Spawn(void) {
}

/*
================
rvMonsterTurret::Save
================
*/
void plant_test::Save(idSaveGame* savefile) const {
}

/*
================
rvMonsterTurret::Restore
================
*/
void plant_test::Restore(idRestoreGame* savefile) {
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool plant_test::CheckActions(void) {
	// Attacks
	return idAI::CheckActions();
}

/*
================
rvMonsterTurret::Pain
================
*/
bool plant_test::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
	common->Printf("i am in pain");
	idDict	  args;
	idEntity* newLegs;
	//We may need to do this at a later point
	//SetSkin ( declManager->FindSkin	 ( spawnArgs.GetString ( "skin_legs" ) ) );
		args.Copy(*gameLocal.FindEntityDefDict("env_ragdoll_marine"));
		idVec3 test = GetPhysics()->GetOrigin();
		test.z +=i+2;
		args.SetVector("origin", test);
		args.SetInt("angle", move.current_yaw);
		gameLocal.SpawnEntityDef(args, &newLegs);
		//store the name of the entity in the Makron's keys so we can burn it out as well.
		common->Printf(newLegs->GetName());
		newLegs->Bind(this, 0);
		i+=3;
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

