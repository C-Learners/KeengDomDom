#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "worldData.h"

#include "IOutilities.h"
#include "savefile.h"
#include "gameBattle.h"
#include "gameData.h"
#include "coord.h"

int column[MAPSIZE*MAPSIZE-1]; //Check function LinesArrayCreate()

int BonusRecovery() {
    int rec; //how much the force will recover
    //Bonus increase
    world.bFood=+ (world.farms + world.fields + world.ponds);
    world.bWeapons=+ (world.iron + world.copper + 5*world.tin);
    rec = floor((world.bFood*world.bWeapons)/100); //It's using one tenth of the total bonus
    Save(); //Game Saved
    //Army Recovery
    if ((world.lostArmy>0)&&(rec>0)) {
        world.lostArmy-= rec;
        world.bFood-= floor(world.bFood/10) + 1;
        world.bWeapons-= floor(world.bWeapons/10) + 1;
        Save(); //Game Saved again
        printf("Your army is partially recovered.\n");
        getchar();
    }
    return 0;
}
int ScoutResults(int index) { //When scouting happens

    world.zone[index].armyDescritpion = floor(((world.zone[index].armyPower*100)/TotalForce())/20)+1; //Percentage of user's total army / 20, cieled
    if (world.zone[index].armyDescritpion>=7) world.zone[index].armyDescritpion=7;
    world.zone[index].armyGoing=0;
    world.zone[index].ogAction = 0;
    Save(); //Game Saved

    printf("Your scouts returned from %d %d.\n", Xcoord(index), Ycoord(index));
    printf("They found ");
    switch (world.zone[index].rType) {
    case (0) :
        printf("a Castle");
        break;
    case (1) :
        printf("an Iron mine");
        break;
    case (2) :
        printf("a Copper mine");
        break;
    case (3) :
        printf("a Tin mine");
        break;
    case (4) :
        printf("a Farm");
        break;
    case (5) :
        printf("a Field");
        break;
    case (6) :
        printf("a Pond");
        break;
    default :
        printf("a funny looking tree"); //this happens if the region type is not acknowledged
        break;
    }
            printf(" protected by ");
    switch (world.zone[index].armyDescritpion) {
    case (1) :
        printf("no-one.\n");
        break;
    case (2) :
        printf("a few soldiers in disarray.\n");
        break;
    case (3) :
        printf("some soldiers.\n");
        break;
    case (4) :
        printf("a decent army.\n");
        break;
    case (5) :
        printf("a strong army.\n");
        break;
    case (6) :
        printf("a really powerful army.\n");
        break;
    case (7) :
        printf("a huge number of soldiers.\n");
        break;
    default :
        printf("... We are sorry, we lost the paper where we wrote it...\n"); //this happens if the army description is not acknowledged
    }
    getchar();

    return 0;
}
int AttackResults(int index) {
    int *playerSurvivors, *enemySurvivors, won;
    //Battle calculations
    playerSurvivors = (int*) malloc(sizeof(int));
    enemySurvivors = (int*) malloc(sizeof(int));
    *playerSurvivors= world.zone[index].armyGoing;
    *enemySurvivors= world.zone[index].armyPower;
    won = Battle(world.zone[index].armyGoing, world.zone[index].armyPower, playerSurvivors, enemySurvivors);

    //From worst scenario to best scenario
    if (*playerSurvivors==0) printf("An enemy messenger came from %d %d\nHe said we lost the battle.\nNone of our soldiers is back...\n", Xcoord(index), Ycoord(index));
    else if (*playerSurvivors <= floor(world.zone[index].armyGoing)/10) {
        if (won==0) printf("A few of our soldiers are back, covered with blood.\nWe lost the battle in %d %d.\nThis is what is left of our army.\n", Xcoord(index), Ycoord(index));
        else printf("A few of our soldiers are back, covered with blood.\nWe conquered %d %d.\nBut at what price...\n",Xcoord(index), Ycoord(index));
    }
    else if (*playerSurvivors <= floor(world.zone[index].armyGoing)/2) {
        if (won==0) printf("Our army is back from %d %d.\nIt has been a fierce battle, and we lost it.\nLuckily, some of them survived.\n", Xcoord(index), Ycoord(index)); //This will be triggered quite rarely
        else printf("Our army is back from %d %d.\nIt has been a fierce battle, but we won.\nHowever, we lost most of the army.\n", Xcoord(index), Ycoord(index));
    }
    else {
        if (won==0) printf("Our army is back from %d %d.\nIt has been a fierce battle, and we lost it.\nLuckily, most of them survived.\n", Xcoord(index), Ycoord(index)); //this condition is practically impossible
        else printf("Our army is back from %d %d.\nIt now belongs to the KeengDomDom\nIt was a good battle, we just lost some soldiers.\n", Xcoord(index), Ycoord(index));
    }

    //Data update
    world.zone[index].ogAction = 0;
    if (won==0) world.zone[index].armyPower = *enemySurvivors;
    else  {
        world.zone[index].armyPower = 0;
        world.zone[index].rOwner = 0;
        switch (world.zone[index].rType) {
        case 0:         //Well, there is actually nothing to do when you get a new castle...
            break;
        case 1:
            world.iron++;
            break;
        case 2:
            world.copper++;
            break;
        case 3:
            world.tin++;
            break;
        case 4:
            world.farms++;
            break;
        case 5:
            world.fields++;
            break;
        case 6:
            world.ponds++;
            break;
        default:
            break;
        }
    }
    world.lostArmy+= world.zone[index].armyGoing - *playerSurvivors;
    world.zone[index].armyGoing = 0;
    Save(); //Game Saved

    getchar();
    return 0;
}

int LinesArrayCreate() {
	int i, c=0;	
	
	for (i=0; i<MAPSIZE*MAPSIZE-1; i++) { //Creating a new array with numbers from 0 to 18, skipping the player's position
		if (i==GetPindex()) c++;
		column[i] = c%MAPSIZE;
		c++;	
	} //this way we know which zones are in a nearby column
	
return 0;
}

int EnemyAttacks(int castle, int target) {
	int *playerSurvivors, *enemySurvivors, lost;
    //Battle calculations
    playerSurvivors = (int*) malloc(sizeof(int));
    enemySurvivors = (int*) malloc(sizeof(int));
    *playerSurvivors= world.zone[target].armyPower;
    *enemySurvivors= world.zone[castle].armyPower;
    lost = Battle(world.zone[castle].armyPower, world.zone[target].armyPower, enemySurvivors, playerSurvivors);
    
   if (lost==1) {
    	if (*playerSurvivors > 0) printf("Our soldiers from %d %d returned exhausted.\nThey have been attacked by the enemy, who", Xcoord(target), Ycoord(target));
    	else printf("We received a letter from an enemy.\nThey wrote they completely destroyed our army at %d %d and ", Xcoord(target), Ycoord(target));
    	printf("conquered back that region.\nWe should be careful and send reinforcement to any nearby zone.\n");
    	world.zone[target].armyPower = floor(world.zone[castle].armyPower/2);
    	world.zone[target].rOwner = 1;
    	if (world.zone[target].ogAction==3) {
    		world.zone[target].ogAction=1; //If the player was going to protect that zone, the army sent will now scout it.
			world.zone[target].defOriginalArmy = 0;
			world.zone[target].defCondition = 0; //re-initializing and canceling protection values
				
		} 	
 	}
   else {
   	printf("We received a letter from %d %d.\nOur army has been attacked by an enemy castle nearby, and we won.\n Better keeping an eye on that region...\n", Xcoord(target), Ycoord(target));
		world.zone[target].armyPower = *playerSurvivors; 
   }
   
   Save();
	getchar();
	return 0;
}

int AtkCheck(int index) { //Check if this place may attack the player
	int toAtk=-1; //Index of the zone to attack
	int i, d;
	
	for (i= index-MAPSIZE-1; i<=index+MAPSIZE+1; i++) { //Checks only nearby lines
		if ((column[i] >= column[index]-1) && (column[i]<=column[index]+1)) { //Adds the column selection: now a square is selected 
			if (toAtk>=0) {
				if (world.zone[i].armyPower < world.zone[toAtk].armyPower) toAtk=i;
			}	
			else toAtk=i;
		}
	}
	// toAtk is now the weakest player's zone nearby
	
	if (toAtk>-1) {
		d = Distance(index); //Distance between enemy castle and player's position (center) 
		if (world.zone[index].atkDays < 1) world.zone[index].atkDays = d*d;
		else if (world.zone[index].atkDays > 1) world.zone[index].atkDays--;
		else EnemyAttacks(index, toAtk);
	}
}

int NextDay() { //Increases the Day counter, decreases every zone "dayleft", checks for new imminent events and triggers them
    char ans;
    int i;
    printf("If you are sure about going to the next day, please insert Y.\n");
    Space(22);
    printf("Your answer: ");
    scanf("%c", &ans);
    getchar();
    if (ans == 'Y') {
        //Next day
        world.day++;
        //Army recovery with bonusses
        if (world.day%2==1) BonusRecovery();
        //Maps Dayleft and event triggers
        for (i=0; i<MAPSIZE*MAPSIZE-1; i++) { //for each zone
            //Event check
            if (world.zone[i].daysLeft==1) {
                switch (world.zone[i].ogAction) {
                case (1) :
                    ScoutResults(i);
                    break;
                case (2) :
                    AttackResults(i);
                default :
                    break;
                }
            }
            //Dayleft scaling
            if (world.zone[i].daysLeft>0) world.zone[i].daysLeft--;
            LinesArrayCreate();
            if ((world.zone[i].rType==0) && (world.zone[i].rOwner == 1)) AtkCheck(i); //If it is an enemy castle
        }
    Save();
    }
    Space(4);
    if (world.day==1) printf("We hope Your first day in the KeengDomDom has been a good day.\nThe real action starts now.\nPress ENTER to return to main menu...");
    else printf("Good morning. %d days have passed since You rule the KeengDomDom.\nPress ENTER to return to main menu...", world.day);
    getchar();
    return 0;
}