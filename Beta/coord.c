#include <math.h>

#include "worldData.h"

#include "coord.h"



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