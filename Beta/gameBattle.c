#include <time.h>

#include "gameBattle.h"

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