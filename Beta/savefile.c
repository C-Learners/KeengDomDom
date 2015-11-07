#include <stdio.h>
#include <stdlib.h>

#include "worldData.h"

#include "savefile.h"

int Generate(int i) { //It creates the i zone (i is its index) by using a fixed seed (see Anew())
    world.zone[i].ogAction = 0;
    world.zone[i].rType = rand()%7;
    world.zone[i].rOwner = 1;
    world.zone[i].armyDescritpion = 0; //will be created when scouted
    world.zone[i].armyPower = (Distance(i)-rand()%2) * (20+rand()%11);
    if (world.zone[i].rType == 0) world.zone[i].armyPower*= (2+rand()%3); //if there is a Castle, its power is two, three or four times higher
    world.zone[i].defCondition = 0;
    world.zone[i].defOriginalArmy = 0;
    world.zone[i].atkDays = 0;
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
                world.copper = 0;
                world.tin = 1;
                world.farms = 2;
                world.fields = 2;
                world.ponds = 2;
                world.bFood = 0;
                world.bWeapons = 0;
                world.wFood = 8;
                world.wWeapons = 5;
                world.soldFood = 0;
                world.soldWeapons = 0;
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