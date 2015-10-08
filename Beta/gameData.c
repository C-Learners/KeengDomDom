#include <math.h>
#include "worldData.h"

#include "gameData.h"

//Secondary Data ("Secondary" Informations are all those variables which depend on Parent and Region structures, and they are not saved because they are redundant)
int FoodProd(){ //Food productivity
    return (world.fields*world.farms*world.ponds);
}
int WeaponProd() { //Weapon productivity
    return (floor(FoodProd()/3) + world.iron*(world.copper + 3*world.tin));
}
int TotalForce() { //Get's the total player's army force, whether or not in the player's castle
    return (world.wFood * world.wWeapons - world.lostArmy);
}
int CastleForce() { //Get's the player's castle army force, which is the total without all the army on trip or defending other places
    int i, awayForce=0;

    for (i=0; i<MAPSIZE*MAPSIZE-1; i++) {
        awayForce+= world.zone[i].armyGoing; //takes the power of the army sent there by the player
        if (world.zone[i].rOwner == 0) awayForce+= world.zone[i].armyPower; //if that is a player's region, takes the army left there
    }

    return (TotalForce() - awayForce);
}
int GoldIncome() {
	return 8*(world.soldFood*3 + world.soldWeapons);
}
int FoodLeft() {
	return FoodProd() - world.wFood - world.soldFood;
}
int WeaponLeft() {
	return WeaponProd() - world.wWeapons - world.soldWeapons;
}