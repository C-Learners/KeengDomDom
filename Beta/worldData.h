#ifndef _WORLDDATA_H_
#define _WORLDDATA_H_


#define MAPSIZE 19      //It defines the side of the square map. If you put 1 here, you are really dumb
#define SCOUTSPEED 0.8  //Days per region taken by scouts to GO AND COME BACK
#define ARMYSPEED 1.1   //Days per region taken by army to GO

//SaveData: data that can and must be saved
//All the infos are saved in unsigned short int

//Region Structure
struct sdRegion
{
    //Region properties
    unsigned short int ogAction; //On going Action, 0: None, 1: Scout, 2: Attack, 3: Protection (protection is currently not used)
    unsigned short int rType; //The Type of the Region, from 0 to 6: Castle, Iron, Copper, Tin, Farm, Field, Pond
    unsigned short int rOwner; //the Owner of the Region. 0 is Player, 1 is Enemy (you may call it "Gaia")
    //Army
    unsigned short int armyDescritpion; //from 0 to 7: None (not explored or owned), Empty, Few, Little, Decent, Strong, Powerful, Huge
    unsigned short int armyPower; //The actual power of the army
    unsigned short int armyGoing; //The power of the player's army going to this region
    //Protection data
    unsigned short int defCondition; //0: army departed, 1: army arrived, 2: messenger departed, 3: army returning
    unsigned short int defOriginalArmy; //Army at the end of "0" stage
    //Time
    unsigned short int daysLeft; //The days left for current action. If 0, it means there is no action
};

//Parent Structure
struct sdParent
{
    //General
    unsigned short int day; //current day
    //Resources
    unsigned short int iron; //player's Iron caves number
    unsigned short int copper; //player's Copper caves number
    unsigned short int tin; //player's Tin caves number
    unsigned short int farms; //player's Farms number
    unsigned short int fields; //player's Fields number
    unsigned short int ponds; //player's Ponds number
    //Total collected Productivity Bonuses
    unsigned short int bFood;
    unsigned short int bWeapons;
    unsigned short int lostArmy; //The force that the player lost during wars. It gets restored using Productivity Bonuses
    //War costs (points spent in war)
    unsigned short int wFood;
    unsigned short int wWeapons;
    //Map
    struct sdRegion zone[MAPSIZE*MAPSIZE-1]; //The two-dimensional map is stored as a normal array (of structures). The coordinate of a zone is calculated by a function. Player position has no index
};

extern struct sdParent world; //The structure is named world, just as the savefile


#endif //_WORLDDATA_H_