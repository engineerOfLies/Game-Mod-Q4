#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"
#include "../spawner.h"
#include "../Projectile.h"

#define BLASTER_SPARM_CHARGEGLOW		6

bool canSpawnMon = true;
int enemyHealth_amount = 100;
bool inBattle = false;
int enemyAttack = 1;
int attackPower = 1;
int wins = 0;
bool start = true;
bool enemyProtection = false;
bool Protection = false;
const char* capturedMon = "";
const char* message;
const char* enemyMessage;
bool brokenProtection = false;

class rvWeaponBlaster : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponBlaster );

	rvWeaponBlaster ( void );

	virtual void		Spawn				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void				PreSave		( void );
	void				PostSave	( void );


protected:

	bool				UpdateAttack		( void );
	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	int					chargeTime;
	int					chargeDelay;
	idVec2				chargeGlow;
	bool				fireForced;
	int					fireHeldTime;

	stateResult_t		State_Raise				( const stateParms_t& parms );
	stateResult_t		State_Lower				( const stateParms_t& parms );
	stateResult_t		State_Idle				( const stateParms_t& parms );
	stateResult_t		State_Charge			( const stateParms_t& parms );
	stateResult_t		State_Charged			( const stateParms_t& parms );
	stateResult_t		State_Fire				( const stateParms_t& parms );
	stateResult_t		State_Flashlight		( const stateParms_t& parms );
	
	CLASS_STATES_PROTOTYPE ( rvWeaponBlaster );
};

CLASS_DECLARATION( rvWeapon, rvWeaponBlaster )
END_CLASS

/*
================
rvWeaponBlaster::rvWeaponBlaster
================
*/
rvWeaponBlaster::rvWeaponBlaster ( void ) {
}

/*
================
rvWeaponBlaster::UpdateFlashlight
================
*/
bool rvWeaponBlaster::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponBlaster::Flashlight
================
*/
void rvWeaponBlaster::Flashlight ( bool on ) {
	owner->Flashlight ( on );
	
	if ( on ) {
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
================
rvWeaponBlaster::UpdateAttack
================
*/
bool rvWeaponBlaster::UpdateAttack ( void ) {
	// Clear fire forced
	if ( fireForced ) {
		if ( !wsfl.attack ) {
			fireForced = false;
		} else {
			return false;
		}
	}

	// If the player is pressing the fire button and they have enough ammo for a shot
	// then start the shooting process.
	if ( wsfl.attack && gameLocal.time >= nextAttackTime ) {
		// Save the time which the fire button was pressed
		if ( fireHeldTime == 0 ) {		
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
			fireHeldTime   = gameLocal.time;
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, chargeGlow[0] );
		}
	}		

	// If they have the charge mod and they have overcome the initial charge 
	// delay then transition to the charge state.
	if ( fireHeldTime != 0 ) {
		if ( gameLocal.time - fireHeldTime > chargeDelay ) {
			SetState ( "Charge", 4 );
			return true;
		}

		// If the fire button was let go but was pressed at one point then 
		// release the shot.
		if ( !wsfl.attack ) {
			idPlayer * player = gameLocal.GetLocalPlayer();
			if( player )	{
			
				if( player->GuiActive())	{
					//make sure the player isn't looking at a gui first
					SetState ( "Lower", 0 );
				} else {
					SetState ( "Fire", 0 );
				}
			}
			return true;
		}
	}
	
	return false;
}

/*
================
rvWeaponBlaster::Spawn
================
*/
void rvWeaponBlaster::Spawn ( void ) {
	viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 0 );
	SetState ( "Raise", 0 );
	
	chargeGlow   = spawnArgs.GetVec2 ( "chargeGlow" );
	chargeTime   = SEC2MS ( spawnArgs.GetFloat ( "chargeTime" ) );
	chargeDelay  = SEC2MS ( spawnArgs.GetFloat ( "chargeDelay" ) );

	fireHeldTime		= 0;
	fireForced			= false;
			
	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponBlaster::Save
================
*/
void rvWeaponBlaster::Save ( idSaveGame *savefile ) const {
	savefile->WriteInt ( chargeTime );
	savefile->WriteInt ( chargeDelay );
	savefile->WriteVec2 ( chargeGlow );
	savefile->WriteBool ( fireForced );
	savefile->WriteInt ( fireHeldTime );
}

/*
================
rvWeaponBlaster::Restore
================
*/
void rvWeaponBlaster::Restore ( idRestoreGame *savefile ) {
	savefile->ReadInt ( chargeTime );
	savefile->ReadInt ( chargeDelay );
	savefile->ReadVec2 ( chargeGlow );
	savefile->ReadBool ( fireForced );
	savefile->ReadInt ( fireHeldTime );
}

/*
================
rvWeaponBlaster::PreSave
================
*/
void rvWeaponBlaster::PreSave ( void ) {

	SetState ( "Idle", 4 );

	StopSound( SND_CHANNEL_WEAPON, 0);
	StopSound( SND_CHANNEL_BODY, 0);
	StopSound( SND_CHANNEL_ITEM, 0);
	StopSound( SND_CHANNEL_ANY, false );
	
}

/*
================
rvWeaponBlaster::PostSave
================
*/
void rvWeaponBlaster::PostSave ( void ) {
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponBlaster )
	STATE ( "Raise",						rvWeaponBlaster::State_Raise )
	STATE ( "Lower",						rvWeaponBlaster::State_Lower )
	STATE ( "Idle",							rvWeaponBlaster::State_Idle)
	STATE ( "Charge",						rvWeaponBlaster::State_Charge )
	STATE ( "Charged",						rvWeaponBlaster::State_Charged )
	STATE ( "Fire",							rvWeaponBlaster::State_Fire )
	STATE ( "Flashlight",					rvWeaponBlaster::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponBlaster::State_Raise
================
*/
stateResult_t rvWeaponBlaster::State_Raise( const stateParms_t& parms ) {
	enum {
		RAISE_INIT,
		RAISE_WAIT,
	};	
	switch ( parms.stage ) {
		case RAISE_INIT:			
			SetStatus ( WP_RISING );
			PlayAnim( ANIMCHANNEL_ALL, "raise", parms.blendFrames );
			return SRESULT_STAGE(RAISE_WAIT);
			
		case RAISE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;	
}

/*
================
rvWeaponBlaster::State_Lower
================
*/
stateResult_t rvWeaponBlaster::State_Lower ( const stateParms_t& parms ) {
	enum {
		LOWER_INIT,
		LOWER_WAIT,
		LOWER_WAITRAISE
	};	
	switch ( parms.stage ) {
		case LOWER_INIT:
			SetStatus ( WP_LOWERING );
			PlayAnim( ANIMCHANNEL_ALL, "putaway", parms.blendFrames );
			return SRESULT_STAGE(LOWER_WAIT);
			
		case LOWER_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetStatus ( WP_HOLSTERED );
				return SRESULT_STAGE(LOWER_WAITRAISE);
			}
			return SRESULT_WAIT;
	
		case LOWER_WAITRAISE:
			if ( wsfl.raiseWeapon ) {
				SetState ( "Raise", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Idle
================
*/
stateResult_t rvWeaponBlaster::State_Idle ( const stateParms_t& parms ) {	
	enum {
		IDLE_INIT,
		IDLE_WAIT,
	};	
	switch ( parms.stage ) {
		case IDLE_INIT:			
			SetStatus ( WP_READY );
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( IDLE_WAIT );
			
		case IDLE_WAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			
			if ( UpdateFlashlight ( ) ) { 
				return SRESULT_DONE;
			}
			if ( UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Charge
================
*/
stateResult_t rvWeaponBlaster::State_Charge ( const stateParms_t& parms ) {
	enum {
		CHARGE_INIT,
		CHARGE_WAIT,
	};	
	switch ( parms.stage ) {
		case CHARGE_INIT:
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, chargeGlow[0] );
			StartSound ( "snd_charge", SND_CHANNEL_ITEM, 0, false, NULL );
			PlayCycle( ANIMCHANNEL_ALL, "charging", parms.blendFrames );
			return SRESULT_STAGE ( CHARGE_WAIT );
			
		case CHARGE_WAIT:	
			if ( gameLocal.time - fireHeldTime < chargeTime ) {
				float f;
				f = (float)(gameLocal.time - fireHeldTime) / (float)chargeTime;
				f = chargeGlow[0] + f * (chargeGlow[1] - chargeGlow[0]);
				f = idMath::ClampFloat ( chargeGlow[0], chargeGlow[1], f );
				viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, f );
				
				if ( !wsfl.attack ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
				
				return SRESULT_WAIT;
			} 
			SetState ( "Charged", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;	
}

/*
================
rvWeaponBlaster::State_Charged
================
*/
stateResult_t rvWeaponBlaster::State_Charged ( const stateParms_t& parms ) {
	enum {
		CHARGED_INIT,
		CHARGED_WAIT,
	};	
	switch ( parms.stage ) {
		case CHARGED_INIT:		
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 1.0f  );

			StopSound ( SND_CHANNEL_ITEM, false );
			StartSound ( "snd_charge_loop", SND_CHANNEL_ITEM, 0, false, NULL );
			StartSound ( "snd_charge_click", SND_CHANNEL_BODY, 0, false, NULL );
			return SRESULT_STAGE(CHARGED_WAIT);
			
		case CHARGED_WAIT:
			if ( !wsfl.attack ) {
				fireForced = true;
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
/*
Spawn Mon
*/

void spawnMon(const char* monType)
{
	const char* key, * value;
	int			i;
	float		yaw;
	idVec3		org;
	idPlayer* player;
	idDict		dict;

	canSpawnMon = false;
	player = gameLocal.GetLocalPlayer();

	yaw = player->viewAngles.yaw;

	value = monType;
	dict.Set("classname", value);
	dict.Set("angle", va("%f", yaw + 180));

	org = player->GetPhysics()->GetOrigin() + idAngles(0, yaw, 0).ToForward() * 80 + idVec3(0, 0, 1);
	dict.Set("origin", org.ToString());

	key = "spawn";
	value = monType;
	dict.Set(key, value);

	// RAVEN BEGIN
	// kfuller: want to know the name of the entity I spawned
	idEntity* newEnt = NULL;
	gameLocal.SpawnEntityDef(dict, &newEnt);
}

void killMon()
{
	idEntity* ent;
	idStrList	ignore;
	const char* name;
	int			i;
	canSpawnMon = true;
	if (!gameLocal.GetLocalPlayer() || !gameLocal.CheatsOk(false)) {
		return;
	}

	name = "KillMonsters";

	for (ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next()) {
		if (ent->IsType(idAI::GetClassType())) {
			for (i = 0; i < ignore.Num(); i++) {
				if (ignore[i] == ent->name && ent->name != "npc_anderson_outdoor") {
					break;
				}
			}

			if (i >= ignore.Num() && ent->name != "npc_anderson_outdoor") {
				ent->PostEventMS(&EV_Remove, 0);
			}
		}
	}
}

void UpdateBattleInfo(idUserInterface* _hud) {
	const char* temp;
	int temp2;
	shopRewards();
	assert(_hud);

	temp = _hud->State().GetString("battleUpdate", "-1");
	if (temp != message) {
		_hud->SetStateString("battleUpdate", message);
		_hud->HandleNamedEvent("UpdateBattleInfo");
	}

	temp2 = _hud->State().GetInt("enemyHealth_amount", "-1");
	if (temp2 != enemyHealth_amount) {
		_hud->SetStateInt("enemyHealth_amount", enemyHealth_amount);
		_hud->HandleNamedEvent("UpdateEnemyHealth");
	}
	temp2 = _hud->State().GetInt("wins", "-1");
	if (temp2 != wins) {
		_hud->SetStateInt("wins", wins);
		_hud->HandleNamedEvent("UpdateWins");
	}
}

void getStarter()
{
	if(g_skill.GetInteger() == 1)
		capturedMon = "monster_gladiator";
	if(g_skill.GetInteger() == 2)
		capturedMon = "monster_scientist";
	if (g_skill.GetInteger() == 3)
		capturedMon = "monster_turret";
	else
		capturedMon = "monster_grunt";

	start = false;

}
void UpdateEnemyBattleInfo(idUserInterface* _hud) {
	int temp2;
	const char* temp;
	shopRewards();
	assert(_hud);

	temp = _hud->State().GetString("enemyBattleUpdate", "-1");
	if (temp != enemyMessage) {
		_hud->SetStateString("enemyBattleUpdate", enemyMessage);
		_hud->HandleNamedEvent("UpdateEnemyBattleInfo");
	}
	temp2 = _hud->State().GetInt("enemyHealth_amount", "-1");
	if (temp2 != enemyHealth_amount) {
		_hud->SetStateInt("enemyHealth_amount", enemyHealth_amount);
		_hud->HandleNamedEvent("UpdateEnemyHealth");
	}
} 


void startQuakeBattle(const char* enemy)
{
	if (!inBattle)
	{
		
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "Battle Start";
		UpdateBattleInfo(player->hud);
		enemyHealth_amount = 100;
		inBattle = true;
		enemyAttack = 1;
		attackPower = 1;
		enemyProtection = false;
		Protection = false;
		brokenProtection = false;
	}
	else
	{
		inBattle = false;
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "Captured Mon!";
		UpdateBattleInfo(player->hud);
		enemyMessage = "";
		UpdateEnemyBattleInfo(player->hud);
		capturedMon = enemy;
		killMon();
	}

}

void shopRewards()
{
	idPlayer* player;
	player = gameLocal.GetLocalPlayer();
	if(wins == 1 && player->health == 100)
		player->GiveItem("weapon_nailgun");
	if (wins == 2 && player->health == 100)
		player->GiveItem("weapon_rocketlauncher");
	if (wins == 3 && player->health == 100)
		player->GiveItem("weapon_railgun");
	if (wins == 4 && player->health == 100)
		player->GiveItem("weapon_lightninggun");
	if (wins == 5 && player->health == 100)
		player->GiveItem("weapon_napalmgun");
}

void protectionBreaker()
{
	idPlayer* player;
	player = gameLocal.GetLocalPlayer();
	message = "Protection breaker used!";
	UpdateBattleInfo(player->hud);
	brokenProtection = true;
	enemyTurn();
	enemyTurn();
}

void willBreaker()
{
	idPlayer* player;
	player = gameLocal.GetLocalPlayer();
	message = "Will breaker used!";
	UpdateBattleInfo(player->hud);
	enemyAttack -= 25;
	enemyTurn();
	enemyTurn();
}

void attackEnhancer()
{
	idPlayer* player;
	player = gameLocal.GetLocalPlayer();
	message = "Attack enhancer used!";
	UpdateBattleInfo(player->hud);
	attackPower += 25;
	enemyTurn();
	enemyTurn();
}


void geminiSplit()
{
	idPlayer* player;
	player = gameLocal.GetLocalPlayer();
	message = "Gemini split used!";
	UpdateBattleInfo(player->hud);
	enemyHealth_amount -= abs((2 * (25 + attackPower)));
	enemyTurn();
	enemyTurn();
}

void olympicMead()
{
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "Olympic mead used!";
		UpdateBattleInfo(player->hud);
		player->health = 200;
		enemyTurn();
		enemyTurn();
}
void Tackle()
{
	if (inBattle) {
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "You used tackle!";
		UpdateBattleInfo(player->hud);
		if (enemyHealth_amount > 0 && !enemyProtection)
		{
			enemyHealth_amount -= abs(25 + attackPower);
			if (enemyHealth_amount <= 0)
			{
				message = "You won!";
				wins += 1;
				UpdateBattleInfo(player->hud);
				enemyMessage = "";
				UpdateEnemyBattleInfo(player->hud);
				inBattle = false;
				killMon();
				spawnWildMon();
				canSpawnMon = true;
			}
			enemyTurn();
		}
		else if (enemyProtection)
		{
			enemyProtection = false;
			enemyTurn();
		}
		else
		{
			idPlayer* player;
			player = gameLocal.GetLocalPlayer();
			message = "You won!";
			wins += 1;
			UpdateBattleInfo(player->hud);
			enemyMessage = "";
			UpdateEnemyBattleInfo(player->hud);
			inBattle = false;
			killMon();
			spawnWildMon();
			canSpawnMon = true;
		}
	}
}
void Protect()
{
	if (inBattle) {
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "You used protect!";
		UpdateBattleInfo(player->hud);
		Protection = true;
		enemyTurn();
	}
}
void SwordsDance()
{
	if (inBattle) {
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "You used swords dance!";
		UpdateBattleInfo(player->hud);
		attackPower += 10;
		enemyTurn();
	}
}
void Growl()
{
	if (inBattle)
	{
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		message = "You used growl!";
		UpdateBattleInfo(player->hud);
		enemyAttack -= 10;
		enemyTurn();
	}
}

void enemyTurn()
{
	if (inBattle)
	{
		idPlayer* player;
		player = gameLocal.GetLocalPlayer();
		int enemyDecision = rand() % 4;
		if (enemyDecision == 0)
		{
			enemyMessage = "They used tackle!";
			UpdateEnemyBattleInfo(player->hud);
			if (!Protection)
			{
				player->health -= abs((25 + enemyAttack));
			}
			else
				Protection = false;
		}
		if (enemyDecision == 1)
		{
			if (brokenProtection)
			{
				enemyMessage = "They couldn't use protect!";
				UpdateEnemyBattleInfo(player->hud);
			}
			else
			{
				enemyMessage = "They used protect!";
				UpdateEnemyBattleInfo(player->hud);
				enemyProtection = true;
			}
		}
		if (enemyDecision == 2)
		{
			enemyMessage = "They used swords dance!";
			UpdateEnemyBattleInfo(player->hud);
			enemyAttack += 10;
		}
		if (enemyDecision == 3)
		{
			enemyMessage = "They used growl!";
			UpdateEnemyBattleInfo(player->hud); 
			attackPower -= 10;
		}
	}
}

void spawnWildMon()
{
	int i = rand() % 10;
	if (i == 0)
		spawnMon("monster_gladiator");
	if (i == 1)
		spawnMon("monster_grunt");
	if (i == 2)
		spawnMon("monster_gunner");
	if (i == 3)
		spawnMon("monster_berserker");
	if (i == 4)
		spawnMon("monster_bossbuddy");
	if (i == 5)
		spawnMon("monster_fatty");
	if (i == 6)
		spawnMon("monster_scientist");
	if (i == 7)
		spawnMon("monster_sentry");
	if (i == 8)
		spawnMon("monster_turret");
	if (i == 9)
		spawnMon("monster_harvester");
}
/*
================
rvWeaponBlaster::State_Fire
================
*/
stateResult_t rvWeaponBlaster::State_Fire ( const stateParms_t& parms ) {
	enum {
		FIRE_INIT,
		FIRE_WAIT,
	};	
	switch ( parms.stage ) {
		case FIRE_INIT:	

			StopSound ( SND_CHANNEL_ITEM, false );
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 0 );
			//don't fire if we're targeting a gui.
			idPlayer* player;
			player = gameLocal.GetLocalPlayer();
		
			//make sure the player isn't looking at a gui first
			if( player && player->GuiActive() )	{
				fireHeldTime = 0;
				SetState ( "Lower", 0 );
				return SRESULT_DONE;
			}

			if( player && !player->CanFire() )	{
				fireHeldTime = 0;
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}


	
			if ( gameLocal.time - fireHeldTime > chargeTime ) {	
				if(start)
					getStarter();
				Attack ( true, 1, spread, 0, 0.0f );
				//
				if (canSpawnMon)
					spawnMon(capturedMon);
				//
				PlayEffect ( "fx_chargedflash", barrelJointView, false );
				PlayAnim( ANIMCHANNEL_ALL, "chargedfire", parms.blendFrames );
				player->GiveItem("weapon_machinegun");
				player->GiveItem("weapon_shotgun");
				player->GiveItem("weapon_hyperblaster");
				player->GiveItem("weapon_grenadelauncher");
			} else {
				//
				killMon();
				message = "";
				UpdateBattleInfo(player->hud);
				enemyMessage = "";
				UpdateEnemyBattleInfo(player->hud);
				inBattle = false;
				spawnWildMon();
				canSpawnMon = true;
				//
				Attack(false, 10, spread, 0, 0.0f);
				PlayEffect("fx_normalflash", barrelJointView, false);
				PlayAnim(ANIMCHANNEL_ALL, "fire", parms.blendFrames);
				
	
			}				
			fireHeldTime = 0;
			
			return SRESULT_STAGE(FIRE_WAIT);
		
		case FIRE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( UpdateFlashlight ( ) || UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}			
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Flashlight
================
*/
stateResult_t rvWeaponBlaster::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};	
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:			
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play		
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );
			
		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}
			
			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}
			
			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
