
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "game/Game_local.h"
#include "Tower.h"


CLASS_DECLARATION(idAI, rvTower)
END_CLASS

rvTower::rvTower() {

}

void rvTower::Spawn(void) {

	health = 100;
	fl.takedamage = true;
	fl.solidForTeam = true;
	BecomeSolid();
	physicsObj.GetClipModel()->Link();

}

void rvTower::InitSpawnArgsVariables(void)
{
	healthRegen = spawnArgs.GetInt("healthRegen", "2");
	healthRegenEnabled = spawnArgs.GetBool("healthRegenEnabled", "0");
}


void rvTower::Think(void) {
	idAI::Think();

	if (health > 0) {
		//alive
		if (healthRegenEnabled && healthRegen) {
			if (gameLocal.GetTime() >= healthRegenNextTime) {
				health = idMath::ClampInt(0, maxHealth, health + healthRegen);
				healthRegenNextTime = gameLocal.GetTime() + 1000;
			}
		}
	}

	//crappy place to do this, just testing
	bool clearPrefix = true;
	bool facingWall = false;
	if (move.fl.moving && InCoverMode() && combat.fl.aware) {
		clearPrefix = false;
		if (DistanceTo(aasSensor->ReservedOrigin()) < move.walkRange * 2.0f) {
			facingWall = true;
		}
		else if (nextWallTraceTime < gameLocal.GetTime()) {
			//do an occasional check for solid architecture directly in front of us
			nextWallTraceTime = gameLocal.GetTime() + gameLocal.random.RandomInt(750) + 750;
			trace_t	wallTrace;
			idVec3 start, end;
			idMat3 axis;
			if (neckJoint != INVALID_JOINT) {
				GetJointWorldTransform(neckJoint, gameLocal.GetTime(), start, axis);
				end = start + axis[0] * 32.0f;
			}
			else {
				start = GetEyePosition();
				start += viewAxis[0] * 8.0f;//still inside bbox
				end = start + viewAxis[0] * 32.0f;
			}
			//trace against solid arcitecture only, don't care about other entities
			gameLocal.TracePoint(this, wallTrace, start, end, MASK_SOLID, this);
			if (wallTrace.fraction < 1.0f) {
				facingWall = true;
			}
			else {
				clearPrefix = true;
			}
		}
	}

	if (facingWall) {
		if (!animPrefix.Length()) {
			animPrefix = "nearcover";
		}
	}
	else if (clearPrefix && animPrefix == "nearcover") {
		animPrefix = "";
	}
}



void rvTower::Save(idSaveGame* savefile) const {
	savefile->WriteInt(health);
}


void rvTower::Restore(idRestoreGame* savefile) {
	savefile->ReadInt( health );

}




