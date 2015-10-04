#include <stdio.h>
#include <stdlib.h>

#include "IOutilities.h"



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