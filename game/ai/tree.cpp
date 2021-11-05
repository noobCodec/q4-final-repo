
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class tree : public idAI {
public:

	CLASS_PROTOTYPE(tree);

	tree(void);

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
	idEntity* kids[3] = { NULL,NULL,NULL };
	CLASS_STATES_PROTOTYPE(tree);
};

CLASS_DECLARATION(idAI, tree)
END_CLASS

/*
================
rvMonsterTurret::rvMonsterTurret
================
*/
tree::tree() {
}

/*
================
rvMonsterTurret::Spawn
================
*/
void tree::Spawn(void) {
	time = gameLocal.GetTime();
	common->Printf("spawned");

}

/*
================
rvMonsterTurret::Save
================
*/
void tree::Save(idSaveGame* savefile) const {
	savefile->WriteInt(time);
}

/*
================
rvMonsterTurret::Restore
================
*/
void tree::Restore(idRestoreGame* savefile) {
	savefile->ReadInt(time);
}

/*
================
rvMonsterTurret::CheckActions
================
*/
bool tree::CheckActions(void) {
	int i;
	bool found = false;
	for (i = 0; i < 3; i++)
	{
		if (!kids[i])
		{
			found = true;
			break;
		}
	}
	if ( found && ((time + growidx * 3000) < gameLocal.GetTime()))
	{
		common->Printf("spawning\n");
		idDict	  args;
		idEntity* newLegs;

		args.Copy(*gameLocal.FindEntityDefDict("test_head"));
		idVec3 test = GetPhysics()->GetOrigin();
		test.x += 2;
		test.y -= 8;
		test.z += 20*(i+1);
		args.SetVector("origin", test);
		args.SetInt("angle", 180);
		gameLocal.SpawnEntityDef(args, &newLegs);
		static_cast<idAI*>(newLegs)->SetDamageEntity(this);
		static_cast<idAI*>(newLegs)->OverrideFlag(AIFLAGOVERRIDE_NOGRAVITY, true);
		kids[i] = newLegs;
		growidx += 1;
		//store the name of the entity in the Makron's keys so we can burn it out as well.
	}
	return true;
}

/*
================
rvMonsterTurret::Pain
================
*/
bool tree::Pain(idEntity* inflictor, idEntity* attacker, int damage, const idVec3& dir, int location) {
	common->Printf("in pain");
	if (inflictor->GetClassname() == "fruit")
	{
		time = gameLocal.GetTime();
		growidx = 1;
		for (int i = 0; i < 3; i++)
		{
			if (kids[i] == inflictor)
			{
				kids[i] = NULL;
				break;
			}
		}
	}
	return idAI::Pain(inflictor, attacker, damage, dir, location);
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(tree)
STATE("State_Combat", tree::State_Combat)
STATE("State_Killed", tree::State_Killed)
END_CLASS_STATES

/*
================
rvMonsterTurret::State_Combat
================
*/
stateResult_t tree::State_Combat(const stateParms_t& parms) {
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
stateResult_t tree::State_Killed(const stateParms_t& parms) {
	idPlayer* player = gameLocal.GetLocalPlayer();
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

