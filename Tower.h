

#ifndef __TOWER__
#define __TOWER__


class rvTower : public idAI {
public:

	CLASS_PROTOTYPE(rvTower);

	rvTower(void);

	void				InitSpawnArgsVariables(void);
	void				Spawn(void);
	void				Think(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);


private:

	int					healthRegen;
	bool				healthRegenEnabled;
	int					healthRegenNextTime;
	int					maxHealth;

	int					nextWallTraceTime;
	rvAIAction            action;

};

extern const idEventDef AI_ForcePosture;

#endif /* !__AI_TACTICAL__ */
