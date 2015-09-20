#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
    unsigned short int armyDescritpion; //from 0 to 7: None (not explored or owned), Empty, Few, Little, Decent, Strong, Powerfull, Huge
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

struct sdParent world; //The structure is named world, just as the savefile

//I/O utilities
int Space(int n) { //prints the given number of empty lines
    int i;
    for(i=0;i<n;i++) printf("\n");
    return 0;
}
int ReadN(){ //Reads the text given and scans it for a number. The number is then returned, after being controlled for int range
    char input[80];
    long int number;
    int ctr=0; //a simple control

    //Long to Int
    while (ctr==0){
        fgets(input,80,stdin);
        number = strtol(input,NULL,0);
        if ((sizeof(int)==2)&&(number>32767)) printf("This number is too big. The maximum is 32767. Try Again\n");
        else if ((sizeof(int)==4)&&(number>2147483647)) printf("This number is too big. The maximum is 2147483647. Try Again\n");
        else ctr = 1;
        if (number<0) number=-1; //The program actually never requires a negative number, while it can handle it.
    }

    return number;
}
int ReadCoords(int *x, int *y) { //Reads coordinates and puts them in x and y
    char input[80];
    char * pEnd; //used to pass the pointer to the second number

    fgets(input, 80, stdin);
    *x = strtol(input, &pEnd, 0);
    *y = strtol(pEnd, NULL, 0);

    return 0;
}


//Coordinate calculations
int GetPindex() { //Makes fake index for player's position
    int fakeI;
    if (MAPSIZE%2 == 1) fakeI = (MAPSIZE + 1) * floor(MAPSIZE/2); //e.g. MAPSIZE=19, player index is 19*9 + 9 (or, 20*9)
    else fakeI =  (MAPSIZE + 1) * (MAPSIZE/2-1); //e.g. MAPSIZE = 10, player index is 10*4+4 (or, 11*4)

    return fakeI;
}

int XYtoIndex(int x, int y) { //Finds the index of any position, given by x and y coords
    int p=GetPindex(); //Player fake index
    int index;
    index = p+MAPSIZE*y+x-1; //this is the index for position that come after the player
    if (index<p) index++; //this is for those that come before the player

    //To understand this procedure, try values like (+1, 0), (-1, 0) and see what happens
    //The player fake index is 180, but 180 is the index for (+1,0), so we need the "-1" for positions after the player, while we don't need it for the others.
    return index;
}
int Ycoord(int zI) { //Gets the y coordinate of the position given by index
    int y;
    if (zI >= GetPindex()) zI++; //this way, the Player position is skipped
    if (MAPSIZE%2 == 1) y = floor(zI/MAPSIZE) - floor(MAPSIZE/2);
    else y = floor(zI/MAPSIZE) - (MAPSIZE/2 - 1);

    return y;
}
int Xcoord(int zI){ //Gets the x coordinate of the position given by index
    int x;
    if (zI >= GetPindex()) zI++; //this way, the Player position is skipped
    if (MAPSIZE%2 == 1) x = zI%MAPSIZE - floor(MAPSIZE/2);
    else x = zI%MAPSIZE - (MAPSIZE/2 - 1);

    return x;
}
int Distance(int zI) { //Calculates the Distance from the given position (by index) and the player.
    return floor(sqrt(pow(Ycoord(zI),2) + pow(Xcoord(zI),2))); //I think I heard Pythagoras sneezing
}

int Generate(int i) { //It creates the i zone (i is its index) by using a fixed seed (see Anew())
    world.zone[i].ogAction = 0;
    world.zone[i].rType = rand()%7;
    world.zone[i].rOwner = 1;
    world.zone[i].armyDescritpion = 0; //will be created when scouted
    world.zone[i].armyPower = (Distance(i)-rand()%2) * (20+rand()%11);
    if (world.zone[i].rType == 0) world.zone[i].armyPower*= (2+rand()%3); //if there is a Castle, its power is two, three or four times higher
    world.zone[i].defCondition = 0;
    world.zone[i].defOriginalArmy = 0;
    world.zone[i].daysLeft = 0;
    return 0;
}

//File Operations
int Save() { //Saves all data in a binary file
    FILE *ptr_file;
    int output=1; //0 if file created, 1 if file not created

    ptr_file = fopen("world.kdd","wb");
    if (!ptr_file) {
        printf("I couldn't create the file. Check for write permission and try again.\n");
        output=1;
    }
    else {
        fwrite(&world, sizeof(world), 1, ptr_file);
        output=0;
    }
    fclose(ptr_file);
    return output;
}
int Anew() { //Confirms new file creation, creates a new game (all defaults are here!), then calls Save
    char input;
    int output=1; //0 if file created, 1 if file not created
    int seed, i;
    do {
        printf("Are you sure to create a new game? Y/n\n");
        scanf("%c", &input);
        getchar();
        if (input == 'n') output = 1; //New game is not created
        else if (input == 'Y') { //New game generation
                printf("Write an integer number to use as a seed: ");
                seed=ReadN();
                srand(seed); //random initialization with fixed seed
                //Default all
                world.day = 0;
                world.iron = 1;
                world.copper = 1;
                world.tin = 0;
                world.farms = 2;
                world.fields = 2;
                world.ponds = 2;
                world.bFood = 0;
                world.bWeapons = 0;
                world.wFood = 8;
                world.wWeapons = 5;
                for (i=0; i<(MAPSIZE*MAPSIZE-1); i++) Generate(i);
                printf("World has been generated.\n");
                output= Save();
        }
        else printf("Please write uppercase Y or lowercase n!\n");
    } while ((input!='n')&&(input!='Y'));

    return output;
}
int Load() { //Loads all data from a binary file. If not found, calls Anew
    FILE *ptr_file;
    int output=1; //0 if file loaded, 1 if file not loaded

    ptr_file = fopen("world.kdd","rb");
    if (!ptr_file) {
        printf("Failed to load save faile.");
        output=1;
    }
    else {
        fread(&world, sizeof(world), 1, ptr_file);
        output=0;
    }
    fclose(ptr_file);
    return output;
}


/*
  -------------------------------------------------------------------------
 -----------------------------GAME FUNCTIONS--------------------------------
-----------------------------------------------------------------------------
*/

//Battle

int BattleOne(int atkForce, int defForce, int *defSurvivors){
    //The attacker surprises the defender which, astonished, is unable to fight back
    *defSurvivors = defForce - (atkForce*(15+rand()%11))/100;
    if (*defSurvivors > 0) return -1;
    else { //if the defender force is cut to 0, the battle ends
        *defSurvivors = 0;
        return 1;
    }
}
int BattleTwo(int atkForce, int defForce, int *atkSurvivors, int *defSurvivors){
    //Fight happens. The most powerful will deal a huge damage, while taking a lot too
    if (atkForce>defForce) {
        *atkSurvivors = atkForce - (defForce*(20+rand()%21))/100;
        *defSurvivors = defForce - (atkForce*(30+rand()%41))/100;
    }
    else {
        *atkSurvivors = atkForce - (defForce*(30+rand()%71))/100;
        *defSurvivors = defForce - (atkForce*(20+rand()%21))/100;
    }

    if (*atkSurvivors<=0) {
            atkSurvivors=0;
            return 0;
    }
    else if (*defSurvivors<=0) {
        *defSurvivors = 0;
        return 1;
    }
    else return -1; //If there are survivors on both sides, the battle continues

}
int BattleThree(int atkForce, int defForce, int *atkSurvivors, int *defSurvivors){
    //This stage can be a Massacre or a retreat

    if (atkForce > 2*defForce) { //defender retreats
        *defSurvivors = defForce - (atkForce*(rand()%11))/100; //attacker deals a slight damage while defender retreats
        return 1;
    }

    else if (atkForce> defForce) { //defender recklessly goes for a suicide attack
        *atkSurvivors = atkForce - (defForce*(10+rand()%241))/100;
        if ((*atkSurvivors <= 0) && (defForce>5)) {
                defForce = 5; //If the defender happens to win, a bit of them is left (max 5)
                return 0;
        }
        else return 1;
    }

    else if (defForce >= 2*atkForce) { //attacker retreats
        *atkSurvivors = atkForce - (defForce*(rand()%11))/100;
        return 0;
    }
    else { //attacker recklessly goes for a suicide attack
        *defSurvivors = defForce - (atkForce*(10+rand()%241))/100;
        if ((*defSurvivors <= 0) && (atkForce>5)) {
            atkForce = 5;
            return 1;
        }
        else return 0;
    }


}

int Battle(int atkForce, int defForce, int *atkSurvivors, int *defSurvivors){ //This is called when a battle is going to happen. Returns 0 if attacker lost, 1 if attacker won
    int won; //0 if attacker loses, 1 if attacker wins, -1 if battle continues
    srand(time(NULL)); //There is a lot of randomness in battles!!

    won = BattleOne(atkForce, defForce, defSurvivors); //First Round
    if (won<0) won = BattleTwo(*atkSurvivors, *defSurvivors, atkSurvivors, defSurvivors); //Second Round
    if (won<0) won = BattleThree(*atkSurvivors, *defSurvivors, atkSurvivors, defSurvivors); //Final Round

    return won;
}


//Get secondary Data ("Secondary" Informations are all those variables which depend on Parent and Region structires, and they are not saved because they are redundant)
int FoodProd(){ //Food productivity
    return (world.fields*world.farms*world.ponds);
}
int WeaponProd() { //Weapon productivity
    return (FoodProd()*world.iron*(world.copper + 5*world.tin));
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

// Actions (after each action, the game gets saved. DO NOT LET THE PLAYER EXIT WHILE THE GAME HAS NOT BEEN SAVED
// Game MUST be saved before asking the player for input!!!

int Properties() { //Prints a list of the player's Properties
    int i, cas=0, irn=0, cop=0, tin=0, frm=0, fld=0, pnd=0;
    int tot=0; //will be used to count zones of any kind
    printf("The total Force of Your while army is: %d\n", TotalForce());
    Space(1);
    for (i=0; i<MAPSIZE*MAPSIZE-1; i++)
        if (world.zone[i].rOwner == 0) {
            tot++;
            if (world.zone[i].rType == 0) cas++;
            else if (world.zone[i].rType == 1) irn++;
            else if (world.zone[i].rType == 2) cop++;
            else if (world.zone[i].rType == 3) tin++;
            else if (world.zone[i].rType == 4) frm++;
            else if (world.zone[i].rType == 5) fld++;
            else if (world.zone[i].rType == 6) pnd++;
        }
    printf("You currently own %d regions, respectively:\n", tot);
    printf("%d Castle", cas);
    if (cas!=1) printf("s");
    Space(1);
    printf("%d Iron mine", irn);
    if (irn!=1) printf("s");
    Space(1);
    printf("%d Copper mine", cop);
    if (cop!=1) printf("s");
    Space(1);
    printf("%d Tin mine", tin);
    if (tin!=1) printf("s");
    Space(1);
    printf("%d Farm", frm);
    if (frm!=1) printf("s");
    Space(1);
    printf("%d Field", fld);
    if (fld!=1) printf("s");
    Space(1);
    printf("%d Pond", pnd);
    if (pnd!=1) printf("s");
    Space(1);

    Space(14);
    printf("Press ENTER to go back to main menu...");
    getchar();

    return 0;
}
/* int Protect(int index){
}
int CallBack(int index){
} */
int GoScout(int index) {//Confirm and execute action
    int force; //The force the player is going to send to scout
    int days = 1 + floor(Distance(index)*SCOUTSPEED);   //the appraisal of the days needed to go and come back

    printf("SYou are going to send scouts to %d %d\n", Xcoord(index), Ycoord(index));
    printf("A big army is usualy not required.\n");
    printf("\n\n");
    printf("They will be back in %d days, if we are lucky\n", days);
    do {
        printf("Write the Force of the army to send there [0 to cancel]\n");
        Space(18);
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
        Space(21);
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
        Space(22);
        printf("Press ENTER to go back to main menu...");
        getchar();
        /*if (world.zone[index].defOriginalArmy == 0) {   //If no army in controlling the place
            printf("There is currently no Army controlling this place.\n");
            printf("If you what to send one, insert 0. Type any other number to cancel.\n");
            Space(19);
            printf("Choose, write and ENTER: ");
            scanf("%d", &input);
            getchar();
            if (input==0) Protect(index);
        }
        else if (world.zone[index].defCondition<2) { //If a legion is at this place
            printf("You already sent a legion with a total force of %d to this place\n", world.zone[index].defOriginalArmy);
            printf("If you want to call it back with a messenger, insert 1. Type any other number to cancel.\n");
            Space(19);
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
        Space(19);
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
        Space(10-spaces);
        printf("Your coordinates:");
        ReadCoords(&x, &y);
        if ((x!=0)||(y!=0)) ZoneMenu(XYtoIndex(x, y)); //opens this zone menu by getting its index

        return 0;
}
int ArmyIncrease() { //Checks if possible and let the player chose how to increare their indoor army force
    int input;
    printf("You are currently using %d of Food productivity \nand %d of Weapon productivity.\n", world.wFood, world.wWeapons);
    printf("So the total Force is %d x %d = %d\n", world.wFood, world.wWeapons, TotalForce());
    Space(2);
    if (WeaponProd()-world.wWeapons < 1) {printf("You need more mines to forge new weapons!\nPress ENTER to go back to main menu...\n"); getchar(); }
    else if ((FoodProd()-world.wFood < 2) && (WeaponProd()-world.wWeapons < 3)) {printf("A Weapon productivity of 3 or a Food productivity of 2 are needed to upgrade your army!\nPress ENTER to go back to main menu\n"); getchar();}
    else {
        if (WeaponProd()-world.wWeapons >= 3)                                    printf("0 - Upgrade your Weapons  [Cost: 3 WF, 0 FP] [Force + %d]\n", (world.wFood*(world.wWeapons+3) - TotalForce()));
        else Space(1);
        if ((WeaponProd()-world.wWeapons >= 1)&&(FoodProd()-world.wFood >= 2))   printf("1 - Train more Soldiers   [Cost: 1 WF, 2 FP] [Force + %d]\n", ((world.wFood+2)*(world.wWeapons+1) - TotalForce()));
        else Space(1);
        printf("Write any other number to cancel.");
        Space(17);
        printf("Choose, write and ENTER: ");
        input=ReadN();
        if (input==0) world.wWeapons+=3;
        else if (input==1) {
            world.wWeapons++;
            world.wFood+=2;
        }
        Save();     //Game Saved
    }
    return 0;
}


//Day Skipping
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

int NextDay() { //Increases the Day counter, decreases every zone "dayleft", checks for new imminent events and triggers them
    char ans;
    int i;
    printf("If you are sure about going to the next day, please insert Y.\n");
    Space(23);
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
        }
    Save();
    }
    Space(4);
    if (world.day==1) printf("We hope Your first day in the KeengDomDom has been a good day.\nThe real action starts now.\nPress ENTER to return to main menu...");
    else printf("Good morning. %d days have passed since You rule the KeengDomDom.\nPress ENTER to return to main menu...", world.day);
    getchar();
    return 0;
}

int ActionCenter() { //Main menu screen
    int input, output=1;
    char errmess;
    printf("==== Main Menu ====\n");
    printf("Day %d\n", world.day);
    printf("Current Food productivity: %d/%d\n", (FoodProd()-world.wFood), FoodProd());
    printf("Current Weapon productivity: %d/%d\n", (WeaponProd()-world.wWeapons), WeaponProd());
    printf("Current indoor Force: %d\n", CastleForce());
    Space(2);
    printf("0 - Check your properties\n");
    printf("1 - Open a map\n");
    printf("2 - Upgrade Your army Force\n");
    Space(2);
    printf("________________________________________________________________________________\n");
    printf("10 - New day\n");
    printf("11 - Exit game\n");
    Space(8);
    printf("Choose, write and ENTER: ");
    input=ReadN();

    switch (input) {
    case (0) :
        Properties();
        break;
    case (1) :
        MapActions();
        break;
    case (2) :
        ArmyIncrease();
        break;
    case (10) :
        NextDay();
        break;
    case (11) :
        output=0; //see main funcion
        break;
    default :
        printf("There is no option for this number! Press ENTER and try again.");
        do {
                errmess = getchar();
        } while (errmess != '\n');
        output=1; //actually does nothing
        break;
    }
    return output;
}

int main()  //Loads (or creates) the game and calls ActionCenter until it returns 0
{
    int input, output;
    do {
        printf("0 - Create new game\n");
        printf("1 - Load game\n");
        Space(22);
        printf("Choose, write and ENTER: ");
        input = ReadN();
        if (input == 0) output = Anew();
        else output = Load();

        if (output==1) {
                printf("\nTry again.");
                getchar();
        }
    } while (output==1);

    do {
        output = ActionCenter();
    } while (output==1);

    return 0;
}
