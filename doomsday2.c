/*
 *  doomsday.c
 *  A program to test a function dayOfWeek which determines which
 *  day of the week a particular date falls on.
 *  (only for dates after the start of the Gregorian calendar).
 *
 *  Program stub created by Richard Buckland on 17/03/14
 *  This program by Kelvin Lau
 *  Freely licensed under Creative Commons CC-BY-3.0
 *
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
#define THURSDAY 0
#define FRIDAY   1
#define SATURDAY 2
#define SUNDAY   3
#define MONDAY   4
#define TUESDAY  5
#define WEDNESDAY 6
 
#define TRUE 1
#define FALSE 0
#define DAYS_PER_WEEK 7

 
int dayOfWeek (int doomsday, int leapYear, int month, int day);
 
int main (int argc, char *argv[]) {

   assert (dayOfWeek (THURSDAY,  FALSE,  4,  4) == THURSDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  6,  6) == FRIDAY);
   assert (dayOfWeek (MONDAY,    FALSE,  8,  8) == MONDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE, 10, 10) == WEDNESDAY);
   assert (dayOfWeek (FRIDAY,    FALSE, 12, 12) == FRIDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  9,  5) == THURSDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  5,  9) == FRIDAY);
   assert (dayOfWeek (SUNDAY,    FALSE,  7, 11) == SUNDAY);
   assert (dayOfWeek (TUESDAY,   FALSE, 11,  7) == TUESDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE,  3,  7) == WEDNESDAY);
 
   assert (dayOfWeek (THURSDAY,  FALSE,  4,  5) == FRIDAY);
   assert (dayOfWeek (SATURDAY,  FALSE,  6,  5) == FRIDAY);
   assert (dayOfWeek (MONDAY,    FALSE,  8,  9) == TUESDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE, 10,  9) == TUESDAY);
   assert (dayOfWeek (FRIDAY,    FALSE, 12, 20) == SATURDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  9,  9) == MONDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  5,  5) == MONDAY);
   assert (dayOfWeek (SUNDAY,    FALSE,  7,  7) == WEDNESDAY);
   assert (dayOfWeek (TUESDAY,   FALSE, 11, 11) == SATURDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  3, 30) == SATURDAY);
 
   assert (dayOfWeek (TUESDAY,   FALSE,  2,  28) == TUESDAY);
   assert (dayOfWeek (TUESDAY,   FALSE,  2,  27) == MONDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  1,  3)  == THURSDAY);
 
   printf ("all tests passed - You are awesome!\n");

   return EXIT_SUCCESS;
}
 
 
// given the doomsday for a year, and whether or not it is a
// leap year, this function return the day of the week for any
// given month and day in the year.
 int doomsdayOfWeek;
int dayOfWeek (int doomsday, int leapYear, int month, int day){
   

   // INSERT YOUR CODE HERE
   
      if (month == 1 && leapYear == FALSE){
         if (day <= 3){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (3 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (3 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 3)%7 + doomsday)%7;
         }
      } else if (month == 1 && leapYear == TRUE){
         if (day <= 4){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (4 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (4 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 4)%7 + doomsday)%7;
         }
      } else if (month == 2 && leapYear == FALSE){
         if (day <= 28){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (28 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (28 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 28)%7 + doomsday)%7;
         }
      } else if (month == 2 && leapYear == TRUE){
         if (day <= 29){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (29 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (29 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 29)%7 + doomsday)%7;
         }
      } else if (month == 3 && leapYear == FALSE){
         doomsdayOfWeek = ((day - 0)%7 + doomsday)%7;
      } else if (month == 4 && leapYear == FALSE){
         if (day <= 4){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (4 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (4 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 4)%7 + doomsday)%7;
         }
      } else if (month == 5 && leapYear == FALSE){
         if (day < 9){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (9 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (9 - day)%7)%7;
            }
         } else {
         doomsdayOfWeek = ((day - 9)%7 + doomsday)%7;
         }
      } else if (month == 6 && leapYear == FALSE){
         if (day <= 6){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (6 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (6 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 6)%7 + doomsday)%7;
         }
      } else if (month == 7 && leapYear == FALSE){
         if (day <= 11){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (11 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (11 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 11)%7 + doomsday)%7;
         }
      } else if (month == 8 && leapYear == FALSE){
         if (day <= 8){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (8 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (8 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 8)%7 + doomsday)%7;
         }
      } else if (month == 9 && leapYear == FALSE){
         if (day <= 5){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (5 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (5 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 5)%7 + doomsday)%7;
         }
      } else if (month == 10 && leapYear == FALSE){
         if (day <= 10){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (10 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (10 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 10)%7 + doomsday)%7;
         }
      } else if (month == 11 && leapYear == FALSE){
         if (day <= 7){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (7 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (7 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 7)%7 + doomsday)%7;
         }
      } else if (month == 12 && leapYear == FALSE){
         if (day <= 12){
            if (doomsday > day){
               doomsdayOfWeek = (doomsday - (12 - day)%7)%7;
            } else {
               doomsdayOfWeek = (doomsday + 7 - (12 - day)%7)%7;
         } else {
         doomsdayOfWeek = ((day - 12)%7 + doomsday)%7;
         }
      }
      return doomsdayOfWeek;
   }
   
