#include <stdio.h>
#include <stdlib.h>

void showDanish (void);

int main(int argc, char *argv[]){
    showDanish ();
return EXIT_SUCCESS;
}

void showDanish (void){
    int lines = 0;
    if (lines == 2){
        printf("\n");
    }
    while (lines < 4){
        int stars = 0;
        if (lines == 2){
            printf("\n");
        }
        while (stars < 11){
            if (stars == 2){
                printf(" ");
            }
            printf("*");
            if (stars == 10 && lines < 3){
                printf("\n");
            }
        stars++;
        }
    lines++;
    }
}