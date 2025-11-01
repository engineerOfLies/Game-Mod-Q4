
# Quake 4 Final Fantasy VII Remake Mod

This mod takes some features from the game Final Fantasy VII Remake and implements them into the game Quake 4. These features include the semi-active combat system, some character/weapon abilites, and some items. 


## Authors

- [@Frokey17](https://www.github.com/Frokey17)


## How To Play

1. Go to my git hub account which should be linked in Authors
2. Click on Q4-Project_GameMod and then click on the branch named fantasy2_mod
3. Download or extract the mod folder and implement it as a new folder into your Quake 4 file
4. Launch Quake 4 and from the main menu, select:
Mods → name of your mod folder → Load Mod

5. Start a new game


    
## Features

- Weapons
    - All weapons other than rochet launcher and gauntlets were changed into abilities from the game Final Fantasy VII Remake
    - Blaster (regular fire / Charge)
        - Changed the charged blast to a bust fire
        - Tweaked damage
    - Dark Matter Gun (Chi Trap)
        - Projectile now stays in place instead of moving
        - Tweaked damage
    - Hyperblaster (Focused Shot)
        - Lowered firerate 
        - Lowered speed of Projectile
        - Tweaked damage
    - Machine Gun (Maximum Furry)
        - Changed from fully automatic to burst fire
        - Increased spread
        - Tweaked damage
    - Grenade Launcher (Blade Burst)
        - Now fires 5 grenandes in a large spread
        - Decreased explosion radius
        - Tweaked damage
    - Shotgun (Point Blank)
        - Decreased spread
        - Tweaked damage
    - Railgun (Soul Drain)
        - Attacks now heal player for 20 health points
        - Tweaked damage
    - Lightning Gun (Ray Of Judgement)
        - Changed the constent fire to a burst fire
        - Tweaked damage
    - Napalm Launcher (Sorcerous Storm)
        - The attack now lands right at the players feat 
        - Attacks no longer do damage to the player
        - Tweaked damage
    - Nailgun (Steel Skin)
        - Atacks now restore player armor to 100
        - Tweaked damage
- GUI Changes
    - Added 2 new activly changing bars in hud
        - One named Energy which increases and decreases depending on player variable energy
        - One named Item which increases and decreases depending on player variable itemCount
    - Added a new list in hud named Abilities which shows all possible abilities (weapons) and items that the player can use, can fully appear and disapear
    - Added a new box in hud named Level which shows the players current level
    - All of these addition and their subparts have show and hide events in hud.gui that change their visibility
- Combat System
    - The combat system starts once you click L to initiate "Battle Mode". Once clicked you hud will change and you will gain the ability to activate your abilites which are linked to Q
    - Shows Energy bar
    - Energy is determined by the player variable energy
    - Each ability has their own energy requirment and keybind which should be shown in the hud changes
    - Once an ability is used your enrgy will decrease/increase accordingly and time will be put back to normal, the player will also go back to their default pistol and time will return to normal
    - You can gain energy by fireing your default blaster, using certain abilites, or using certain items
- Item System
    - Once in "Battle Mode" your hud will be updated to show the Items bar
    - Item points is determined by the player variable itemCount
    - The Items bar can hold up to 3 item points which are gained by killing an enemy
    - Each item has its own item point requirment
    - Once an item is used your item points will decrease accordingly and time will be put back to normal, time will aslo return to normal
- Level Up System
    - Level is determined by player variable currentLevel
    - XP is determined by player variable currentXP
    - Once you make a new game you will start at level 1
    - You gain xp by dealing the final blow to enemies
    - Once you reach a certain amount xp you will level up, level 5 being the max, and your xp will be reset to 0
    - As you level up your damage will increase for all abilities
- Commands
    - Added new command "spawnteam"
        - Linked to L key
        - On first click, activates "Battle Mode" on click, which shows the Energy and Item bars, and allows the player to input the abilities command (Q)
             - Sets player variable energy to 0
             - Spawns 2 friendly marine NPCs
        - On second click, deactivates "Battle Mode" on click, which hides the Energy and Item bars, and removes the players ability to input the abilities command (Q)
             - Despawns all friendly marine NPCs
    - Added new command "abilities" 
        - Slows down game time with one click, and then sets it back to normal with another
        - Linked to Q key
        - Shows the Abilities list in HUD with one click, hides it with a second click
        - Allows the player to input commands related to abilites (weapons) and items with one click, removes this ability with a second click
    - Added new command "modHelp" 
        - Displays text in the game console that can help the player with small common problems
        - Explains how to access the commands helpCombat, helpItem, helpLevel
    - Added new command "helpCombat" 
        - Explains the combat system
        - Explains how to get back to modHelp
    - Added new command "helpItem" 
        - Explains the Item system
        - Explains how to get back to modHelp
    - Added new command "helpLevel" 
        - Explains the level up system
        - Explains how to get back to modHelp
    - Added new commands for each ability (weapon)
        - Each command will give and make the player select the weapon that is linkied to the ability
        - They will also slow down time 
