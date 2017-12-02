#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char* argv[]){
    //convert arg into int val
    int keyLen = atoi(argv[1]);

    printf("Number is :%d\n", keyLen);
    int i = 0;
    //Seed rng
    srand(time(NULL));
    while(i < keyLen){
        int randInt = rand();
        randInt = randInt % 27;
        if(randInt == 26){
            printf(" ");
        }
        else{
            //print out character
            printf("%c", randInt + 65);
        }
        i++;
    }
    //print out newline
    printf("\n");

    return 0;
}
