#ifndef DARTH_VADER_H
#define DARTH_VADER_H

// notes for the buzzer
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784; //Only this note is needed if we don't use the darth vader song
const int gSH = 830;
const int aH = 880;

#define buzzerPin  8

void firstSection(); //first part of the Darth Vader theme
void secondSection(); //second part of the Darth Vader theme

void beep(int note, int duration); //Plays a note for a period of time on the Piezocrystal

#endif // DARTH_VADER_H