#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

#define BLASTER_SPARM_CHARGEGLOW		6

class rvWeaponFiveSeven : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponFiveSeven );

	rvWeaponFiveSeven ( void );

	virtual void		Spawn				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void				PreSave		( void );
	void				PostSave	( void );

protected:

	bool				fireHeld;
	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	stateResult_t		State_Raise				( const stateParms_t& parms );
	stateResult_t		State_Lower				( const stateParms_t& parms );
	stateResult_t		State_Idle				( const stateParms_t& parms );
	stateResult_t		State_Fire				( const stateParms_t& parms );
	stateResult_t		State_Reload			( const stateParms_t& parms );
	stateResult_t		State_Flashlight		( const stateParms_t& parms );
	
	CLASS_STATES_PROTOTYPE ( rvWeaponFiveSeven );
};

CLASS_DECLARATION( rvWeapon, rvWeaponFiveSeven )
END_CLASS

/*
================
rvWeaponFiveSeven::rvWeaponFiveSeven
================
*/
rvWeaponFiveSeven::rvWeaponFiveSeven ( void ) {
}

/*
================
rvWeaponFiveSeven::UpdateFlashlight
================
*/
bool rvWeaponFiveSeven::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponFiveSeven::Flashlight
================
*/
void rvWeaponFiveSeven::Flashlight ( bool on ) {
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
rvWeaponFiveSeven::Spawn
================
*/
void rvWeaponFiveSeven::Spawn ( void ) {
	fireHeld = false;

	SetState ( "Raise", 0 );
			
	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponFiveSeven::Save
================
*/
void rvWeaponFiveSeven::Save ( idSaveGame *savefile ) const {
	savefile->WriteBool(fireHeld);
}

/*
================
rvWeaponFiveSeven::Restore
================
*/
void rvWeaponFiveSeven::Restore ( idRestoreGame *savefile ) {
	savefile->ReadBool(fireHeld);
}

/*
================
rvWeaponFiveSeven::PreSave
================
*/
void rvWeaponFiveSeven::PreSave ( void ) {

	SetState ( "Idle", 4 );

	StopSound( SND_CHANNEL_WEAPON, 0);
	StopSound( SND_CHANNEL_BODY, 0);
	StopSound( SND_CHANNEL_ITEM, 0);
	StopSound( SND_CHANNEL_ANY, false );
	
}

/*
================
rvWeaponFiveSeven::PostSave
================
*/
void rvWeaponFiveSeven::PostSave ( void ) {
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponFiveSeven )
	STATE ( "Raise",						rvWeaponFiveSeven::State_Raise )
	STATE ( "Lower",						rvWeaponFiveSeven::State_Lower )
	STATE ( "Idle",							rvWeaponFiveSeven::State_Idle)
	STATE ( "Fire",							rvWeaponFiveSeven::State_Fire )
	STATE ( "Reload",						rvWeaponFiveSeven::State_Reload )
	STATE ( "Flashlight",					rvWeaponFiveSeven::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponFiveSeven::State_Raise
================
*/
stateResult_t rvWeaponFiveSeven::State_Raise( const stateParms_t& parms ) {
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
rvWeaponFiveSeven::State_Lower
================
*/
stateResult_t rvWeaponFiveSeven::State_Lower ( const stateParms_t& parms ) {
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
rvWeaponFiveSeven::State_Idle
================
*/
stateResult_t rvWeaponFiveSeven::State_Idle ( const stateParms_t& parms ) {	
	enum {
		IDLE_INIT,
		IDLE_WAIT,
	};
	switch (parms.stage) {
		case IDLE_INIT:
			if (!AmmoAvailable()) {
				SetStatus(WP_OUTOFAMMO);
			}
			else {
				SetStatus(WP_READY);
			}

			PlayCycle(ANIMCHANNEL_ALL, "idle", parms.blendFrames);
			return SRESULT_STAGE(IDLE_WAIT);

		case IDLE_WAIT:
			if (wsfl.lowerWeapon) {
				SetState("Lower", 4);
				return SRESULT_DONE;
			}
			if (UpdateFlashlight()) {
				return SRESULT_DONE;
			}

			if (fireHeld && !wsfl.attack) {
				fireHeld = false;
			}
			if (!clipSize) {
				if (!fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable()) {
					SetState("Fire", 0);
					return SRESULT_DONE;
				}
			}
			else {
				if (!fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState("Fire", 0);
					return SRESULT_DONE;
				}
				if (wsfl.attack && AutoReload() && !AmmoInClip() && AmmoAvailable()) {
					SetState("Reload", 4);
					return SRESULT_DONE;
				}
				if (wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable() > AmmoInClip())) {
					SetState("Reload", 4);
					return SRESULT_DONE;
				}
			}
			return SRESULT_WAIT;
		}
	return SRESULT_ERROR;
}

/*
================
rvWeaponFiveSeven::State_Fire
================
*/
stateResult_t rvWeaponFiveSeven::State_Fire ( const stateParms_t& parms ) {
	enum {
		FIRE_INIT,
		FIRE_WAIT,
	};	
	switch ( parms.stage ) {
		case FIRE_INIT:	

			StopSound ( SND_CHANNEL_ITEM, false );
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier(PMOD_FIRERATE));

			//don't fire if we're targeting a gui.
			idPlayer* player;
			player = gameLocal.GetLocalPlayer();

			//make sure the player isn't looking at a gui first
			if( player && player->GuiActive() )	{
				SetState ( "Lower", 0 );
				return SRESULT_DONE;
			}

			if( player && !player->CanFire() )	{
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			
			Attack(false, 1, spread, 0, 1.0f);
			PlayEffect("fx_normalflash", barrelJointView, false);
			fireHeld = true;
			PlayAnim(ANIMCHANNEL_ALL, "fire", parms.blendFrames);
			
			return SRESULT_STAGE(FIRE_WAIT);
		
		case FIRE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if (UpdateFlashlight()) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}			
	return SRESULT_ERROR;
}

/*
================
rvWeaponFiveSeven::State_Reload
================
*/
stateResult_t rvWeaponFiveSeven::State_Reload(const stateParms_t& parms) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch (parms.stage) {
	case STAGE_INIT:
		if (wsfl.netReload) {
			wsfl.netReload = false;
		}
		else {
			NetReload();
		}

		SetStatus(WP_RELOAD);
		PlayAnim(ANIMCHANNEL_ALL, "reload", parms.blendFrames);
		return SRESULT_STAGE(STAGE_WAIT);

	case STAGE_WAIT:
		if (AnimDone(ANIMCHANNEL_ALL, 4)) {
			AddToClip(ClipSize());
			SetState("Idle", 4);
			return SRESULT_DONE;
		}
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponFiveSeven::State_Flashlight
================
*/
stateResult_t rvWeaponFiveSeven::State_Flashlight ( const stateParms_t& parms ) {
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
