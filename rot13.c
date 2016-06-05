// Name: Jiayi Li, Rachel, z5042598 and Kelvin Lau (z5112691)
// Tutor & Class: Jade Giacoppo, Tue 09 sitar
// Date: 22/03/2016
// This file is to convert secreat code to original file.

#include <stdio.h>
#include <stdlib.h>

#define FIRST_LC_LETTER 'a'
#define LAST_LC_LETTER 'z'
#define FIRST_UC_LETTER 'A'
#define LAST_UC_LETTER 'Z'
#define ROT 13

int isLower (char letter);
int isUpper (char letter);
char encode (char plainText);

int main (int argc, char *argv[]) {
    
    char plainChar;
    char encodedChar;
    
    printf ("Enter the message to be encoded: \n");
    scanf ("%c", &plainChar);
    printf ("Encoded is: \n");
    
    while (plainChar != '@') {
        if (isLower (plainChar) || isUpper (plainChar)) {
            encodedChar = encode (plainChar);
        } else {
            encodedChar = plainChar;
        }
        printf ("%c", encodedChar);
        scanf ("%c", &plainChar);
    }
    
    printf("\n");
    
    return EXIT_SUCCESS;
}

int isLower (char letter) {
    return (letter >= FIRST_LC_LETTER) && (letter <= LAST_LC_LETTER);
}

int isUpper (char letter) {
    return (letter >= FIRST_UC_LETTER) && (letter <= LAST_UC_LETTER);
}

char encode (char plainText) {
    
    unsigned char cipherText;
    char exclude;
    int LowEncoded = isLower(plainText);
    int UpEncoded = isUpper(plainText);
    
    if (LowEncoded) {
        cipherText = plainText + ROT;
        if (cipherText > LAST_LC_LETTER) {
            exclude = cipherText - LAST_LC_LETTER;
            cipherText = FIRST_LC_LETTER + exclude - 1;
        }
    } else if (UpEncoded) {
        cipherText = plainText + ROT;
        if (cipherText > LAST_UC_LETTER) {
            exclude = cipherText - LAST_UC_LETTER;
            cipherText = FIRST_UC_LETTER + exclude - 1;
        }
    } else {
        cipherText = plainText;
    }
    
    return cipherText;
}
