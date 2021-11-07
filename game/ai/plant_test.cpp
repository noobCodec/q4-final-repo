
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
	idEntity* prev = NULL;
	idEntity* next = NULL;
	char i = 'a';
	int time;
	int growidx=1;
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
	time = gameLocal.GetTime();

}

/*
================
rvMonsterTurret::Save
================
*/
void plant_test::Save(idSaveGame* savefile) const {
	savefile->WriteInt(time);
}

/*
================
rvMonsterTurret::Restore
================
*/
void plant_test::Restore(idRestoreGame* savefile) {
	savefile->ReadInt(time);
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool plant_test::CheckActions(void) {
	if (!next&&((time + growidx * 500) < gameLocal.GetTime()))
	{
		common->Printf("spawning\n");
		idDict	  args;
		idEntity* newLegs;
		
		args.Copy(*gameLocal.FindEntityDefDict("failed_braindamage"));
		idVec3 test = GetPhysics()->GetOrigin();
		//test.x = 8349.64;
		//test.y = -8920.62;
		test.z += 40;
		args.SetVector("origin", test);
		args.SetInt("angle", 180);
		gameLocal.SpawnEntityDef(args, &newLegs);
		//store the name of the entity in the Makron's keys so we can burn it out as well.
		static_cast<plant_test*>(newLegs)->prev=this;
		next = newLegs;
		i += 3;
		growidx += 1;
	}
	return true;
}

/*
================
rvMonsterTurret::Pain
================
*/
bool plant_test::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
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
	idPlayer* player = gameLocal.GetLocalPlayer();
	idDict tospawn;
	idStr goodies[3] = { "first_seed","second_seed","third_seed"};
	tospawn.Copy(*gameLocal.FindEntityDefDict(goodies[gameLocal.random.RandomInt(2)]));
	gameLocal.PlayEffect(gameLocal.GetEffect(spawnArgs, "fx_death"), GetPhysics()->GetOrigin(), (-GetPhysics()->GetGravityNormal()).ToMat3());
	if (next) {
		next->Killed(NULL, NULL, NULL, next->GetPhysics()->GetOrigin(), NULL);
	}
	if (prev)
	{
		static_cast<plant_test*>(prev)->next = NULL;
		static_cast<plant_test*>(prev)->time = gameLocal.GetTime();
		static_cast<plant_test*>(prev)->growidx = 1;
	}
	player->GiveInventoryItem(&tospawn);
	
	return idAI::State_Killed(parms);
}

