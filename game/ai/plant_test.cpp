
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

class plant_test : public idAI {
public:

	CLASS_PROTOTYPE(plant_test);

	plant_test(void);

	void				Spawn(void);

	virtual void		OnDeath(void);

	// Add some dynamic externals for debugging
	virtual void		GetDebugInfo(debugInfoProc_t proc, void* userData);

private:

	CLASS_STATES_PROTOTYPE(plant_test);
};

CLASS_DECLARATION(idAI, plant_test)
END_CLASS

/*
================
rvMonsterScientist::rvMonsterScientist
================
*/
plant_test::plant_test(void) {
}

/*
================
rvMonsterScientist::Spawn
================
*/
void plant_test::Spawn(void) {
	common->Printf("spawning");
	PlayEffect("fx_fly", animator.GetJointHandle("effects_bone"), true);
}

/*
================
rvMonsterScientist::OnDeath
================
*/
void plant_test::OnDeath(void) {
	StopEffect("fx_fly");

	idAI::OnDeath();
}

/*
================
rvMonsterScientist::GetDebugInfo
================
*/
void plant_test::GetDebugInfo(debugInfoProc_t proc, void* userData) {
	// Base class first
	idAI::GetDebugInfo(proc, userData);
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(plant_test)
END_CLASS_STATES
