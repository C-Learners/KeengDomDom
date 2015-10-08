#include <stdio.h>
#include <stdlib.h>

#include "worldData.h"

#include "IOutilities.h"
#include "gameNewDay.h"
#include "gameAction.h"
#include "gameData.h"
#include "savefile.h"

struct sdParent world;

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

    Space(13);
    printf("Press ENTER to go back to main menu...");
    getchar();

    return 0;
}

int ArmyIncrease() { //Checks if possible and let the player chose how to increare their indoor army force
    int input;
    printf("You are currently using %d of Food productivity \nand %d of Weapon productivity.\n", world.wFood, world.wWeapons);
    printf("So the total Force is %d x %d = %d\n", world.wFood, world.wWeapons, TotalForce());
    Space(2);
    if (WeaponLeft() < 1) {printf("You need more mines to forge new weapons!\nPress ENTER to go back to main menu...\n"); getchar(); }
    else if ((FoodLeft() < 2) && (WeaponLeft() < 3)) {printf("A Weapon productivity of 3 or a Food productivity of 2 are needed to upgrade your army!\nPress ENTER to go back to main menu\n"); getchar();}
    else {
        if (WeaponLeft() >= 3)             		      printf("0 - Upgrade your Weapons  [Cost: 3 WF, 0 FP] [Force + %d]\n", (world.wFood*(world.wWeapons+3) - TotalForce()));
        else Space(1);
        if ((WeaponLeft() >= 1)&&(FoodLeft() >= 2))   printf("1 - Train more Soldiers   [Cost: 1 WF, 2 FP] [Force + %d]\n", ((world.wFood+2)*(world.wWeapons+1) - TotalForce()));
        else Space(1);
        printf("Write any other number to cancel.");
        Space(16);
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

int BuyGold() {
	int inFood, inWeapon;


	printf("Here you can sell Food and Weapons for Gold.\n\n");
	do {	
		printf("First, write the Food you are going to change for Gold.\n");
		printf("1 Food = 24 Gold\n");
		Space(19);
		printf("Selling Food Productivity: ");
		inFood = ReadN();
		if (inFood > FoodLeft()) {
			printf("You can't afford all that Food right now, it's just %d\n\n", FoodLeft());
		}
	} while (inFood > FoodLeft());
	printf("Incoming %d Gold from Food selling...\n\n", 24*inFood);	
	do {
	printf("Now write the Weapon outcome.\n");
	printf("1 Weapon = 8 Gold\n");
	Space(19);
	printf("Selling Weapon Productivity: ");
	inWeapon = ReadN();
		if (inWeapon > WeaponLeft()) {
			printf("You can't afford all those Weapons right now, their productivity is just %d\n\n", WeaponLeft());
		}
	} while (inWeapon > WeaponLeft());
	
	world.soldFood += inFood;
	world.soldWeapons += inWeapon;
	Save();
	return 0;
}

int ActionCenter() { //Main menu screen
    int input, output=1;
    char errmess;
    printf("==== Main Menu ====\n");
    printf("Day %d\n", world.day);
    printf("Current Food productivity: %d/%d\n", FoodLeft(), FoodProd());
    printf("Current Weapon productivity: %d/%d\n", WeaponLeft(), WeaponProd());
    printf("Current indoor Force: %d\n", CastleForce());
    printf("Current Gold Income: %d\n", GoldIncome());
    Space(2);
    printf("0 - Check your properties\n");
    printf("1 - Open a map\n");
    printf("2 - Upgrade Your army Force\n");
    printf("3 - Buy Gold\n");
    Space(2);
    printf("________________________________________________________________________________\n");
    printf("10 - New day\n");
    printf("11 - Exit game\n");
    Space(6);
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
    case (3) :
       	BuyGold();
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
        Space(21);
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
