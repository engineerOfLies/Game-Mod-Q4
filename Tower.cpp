
#include "../../idlib/precompiled.h"
#pragma hdrstop
#include "game/Game_local.h"


class rvTower : public idAI {
public:

	CLASS_PROTOTYPE(rvTower);

	rvTower(void);

	void				Spawn(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);

protected:

	idStr				health;
	idAI  sp;
	rvAIAction            action;

	
};
CLASS_DECLARATION(idAI, rvTower)
END_CLASS

rvTower::rvTower() {

}

void rvTower::Spawn(void) {

	health = spawnArgs.GetString("health", "100");	
	action.Init(spawnArgs, "passive", NULL, 0);

}

void rvTower::Save(idSaveGame* savefile) const {
	savefile->WriteString(health);
}


void rvTower::Restore(idRestoreGame* savefile) {
	savefile->ReadString( health );

}




