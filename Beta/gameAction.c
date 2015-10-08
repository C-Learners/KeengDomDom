#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "worldData.h"

#include "IOutilities.h"
#include "savefile.h"
#include "gameData.h"
#include "coord.h"


// Actions (after each action, the game gets saved. DO NOT LET THE PLAYER EXIT WHILE THE GAME HAS NOT BEEN SAVED
// Game MUST be saved before asking the player for input!!!

/*
 int Protect(int index){
}
int CallBack(int index){
} 
*/
int GoScout(int index) {//Confirm and execute action
    int force; //The force the player is going to send to scout
    int days = 1 + floor(Distance(index)*SCOUTSPEED);   //the appraisal of the days needed to go and come back

    printf("You are going to send scouts to %d %d\n", Xcoord(index), Ycoord(index));
    printf("A big army is usualy not required.\n");
    printf("\n\n");
    printf("They will be back in %d days, if we are lucky\n", days);
    do {
        printf("Write the Force of the army to send there [0 to cancel]\n");
        Space(17);
        printf("Write your answer as a number and press ENTER: ");
        force=ReadN();
        if (force > CastleForce()) printf("We don't have that much Force!\nCurrent indoor Force is %d!\nTry again or write 0 to cancel\n\n\n", CastleForce());
    } while (force > CastleForce());
    if (force > 0) {
        srand(time(NULL)); //random initialize for days
        world.zone[index].ogAction = 1;
        world.zone[index].armyGoing = force;
        world.zone[index].daysLeft = days + rand()%3;
        Save(); //Game Saved
    }
    return 0;
}
int Attack(int index) { //Confirm and execute action
    int force; //The force the player is going to send to attack
    int days = 1 + floor(2*Distance(index)*ARMYSPEED);   //the appraisal of the days needed to go and come back

    printf("You are going to Attack %d %d\n", Xcoord(index), Ycoord(index));
    do {
        printf("Your army will be back in about %d days.\n", days);
        printf("What will be the sent Force? [0 to cancel]\n");
        Space(20);
        printf("Write your answer as a number and press ENTER: ");
        force=ReadN();
        if (force > CastleForce()) printf("We don't have that much Force!\nCurrent indoor Force is %d!\nTry again or write 0 to cancel\n\n", CastleForce());
    } while (force > CastleForce());
    if (force>0) {
        srand(time(NULL)); //random initialize for days
        world.zone[index].ogAction = 2;
        world.zone[index].armyGoing = force;
        world.zone[index].daysLeft = days + rand()%6;
        Save(); //Game Saved
    }
    return 0;
}
int ZoneMenu(int index) { //Check this zone status and asks for actions
    int input;
    if (world.zone[index].rOwner == 0) { //If the player is the Owner
        printf("This land belongs to Your KeengDomDom.\n");
        printf("It is known for its ");
        switch (world.zone[index].rType) {
        case (0) :
            printf("Castle.\n");
            break;
        case (1) :
            printf("Iron mine.\n");
            break;
        case (2) :
            printf("Copper mine.\n");
            break;
        case (3) :
            printf("Tin mine.\n");
            break;
        case (4) :
            printf("Farm.\n");
            break;
        case (5) :
            printf("Field.\n");
            break;
        case (6) :
            printf("Pond.\n");
            break;
        default :
            printf("funny looking tree.\n"); //this happens if the region type is not acknowledged
            break;
        }
        Space(21);
        printf("Press ENTER to go back to main menu...");
        getchar();
        /*if (world.zone[index].defOriginalArmy == 0) {   //If no army in controlling the place
            printf("There is currently no Army controlling this place.\n");
            printf("If you what to send one, insert 0. Type any other number to cancel.\n");
            Space(18);
            printf("Choose, write and ENTER: ");
            scanf("%d", &input);
            getchar();
            if (input==0) Protect(index);
        }
        else if (world.zone[index].defCondition<2) { //If a legion is at this place
            printf("You already sent a legion with a total force of %d to this place\n", world.zone[index].defOriginalArmy);
            printf("If you want to call it back with a messenger, insert 1. Type any other number to cancel.\n");
            Space(18);
            printf("Choose, write and ENTER: ");
            scanf("%d", &input);
            getchar();
            if (input==1) CallBack(index);
        }
        else { //If there is some army, but it's returning, or you already sent a messenger
                printf("You called back your army from there. Just wait some days.Press ENTER to return to Main Menu...");
                getchar();
        }*/
    }
    else { //If the player isn't the Owner
        if (world.zone[index].armyDescritpion > 0) { //If it has already been scouted
            printf("This region has been explored. There is ");
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
            printf(" defended by ");
            switch (world.zone[index].armyDescritpion) {
            case (1) :
                printf("no-one.");
                break;
            case (2) :
                printf("a few soldiers in disarray.");
                break;
            case (3) :
                printf("some soldiers.");
                break;
            case (4) :
                printf("a decent army.");
                break;
            case (5) :
                printf("a strong army.");
                break;
            case (6) :
                printf("a really powerful army.");
                break;
            case (7) :
                printf("a huge number of soldiers.");
                break;
            default :
                printf("... We are sorry, we lost the paper where we wrote it..."); //this happens if the army description is not acknowledged
            }
        }
        else printf("We don't know much about this place.\n"); //If it is yet to be scouted
        Space(1);
        printf("0 - Scout this region\n");
        printf("1 - Attack this region\n");
        printf("Write any other number to cancel.\n");
        Space(18);
        printf("Choose, write and ENTER: ");
        input = ReadN();
        if (input==0) GoScout(index);
        else if (input==1) Attack(index);
    }
    return 0;
}
int MapActions() { //From here, the user can chose a position and do actions to that
        int i, c=0, spaces=0, owned=0; //spaces couts used lines, owned counts all owned zones
        int min, max; //Range of coordinates
        int x, y; //chosen coords

        for (i=0; i<MAPSIZE*MAPSIZE-1; i++) if (world.zone[i].rOwner == 0) owned++;
        min = floor(MAPSIZE/2);
        if (MAPSIZE%2 == 1) max=min;
        else max=min+1;

        printf("==== Map menu ====\n");
        printf("You own %d zones of %d\n", owned, MAPSIZE*MAPSIZE-1);
        printf("Current indoor Force: %d\n", CastleForce());
        Space(2);
        printf("Currently Exploring...\n");
        for (i=0; i<MAPSIZE*MAPSIZE-1; i++) if (world.zone[i].ogAction==1) {
                printf("| %d %d |", Xcoord(i), Ycoord(i));
                c++;
                if (c%8==0) {printf("\n"); spaces++;}
                                            }
        printf("\nCurrently Attacking...\n");
        c=0;
        for (i=0; i<MAPSIZE*MAPSIZE-1; i++) if (world.zone[i].ogAction==2) {
                printf("| %d %d |", Xcoord(i), Ycoord(i));
                c++;
                if (c%8==0) {printf("\n"); spaces++;}
                                            }
        Space(2);
        printf("Write coordinates between -%d and +%d\n", min, max);
        printf("Example: +1 -3\n");
        Space(1);
        printf("Write 0 0 to cancel and go back to main menu.\n");
        Space(9-spaces);
        printf("Your coordinates:");
        ReadCoords(&x, &y);
        if ((x!=0)||(y!=0)) ZoneMenu(XYtoIndex(x, y)); //opens this zone menu by getting its index

        return 0;
}