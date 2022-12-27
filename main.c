#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "fighters.h"

// Ohjelman pääfunktio
void main(){
    Fighter *allfighters;
    allfighters = calloc(1, sizeof(Fighter));
    if(!allfighters){

    }
    
    int exit = 0;
    while(!exit){
        char *buffer[80];
        scanf("%s", buffer);
        int res = read_command(buffer, allfighters);
        if(res == 0){
            printf("Invalid command.");
        }
        else if(res == -1){
            printf("Program shutting down.");
            exit = 1;
        }
    }
}