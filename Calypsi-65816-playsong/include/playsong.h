// -------------------------------------------
// playSong.h
// Simple Music Library 
// By Ernesto Contreras
//--------------------------------------------

// function prototypes
void cleanTracks(int rows,int cols,unsigned char (*Tracker)[cols]);
int  addTrack(char tune[], int trackNumber, int rows, int cols, unsigned char (*Tracker)[cols]);
void showTracks(int rows,int cols,unsigned char (*Tracker)[cols]);
void playSongSID(int rows,int cols, unsigned char (*Tracker)[cols], int tempo);
int  setInstrumentSID(int voice, int instrument);
void delay (float value);
void resetSID();

// Predefined SID_envelopes
/*
	// atk, dcy, sust, rel, wave, pulseWidth, note length [1-normal, 2-legato (long), 3-staccato (short)]
	{0,9,0,0,4,2048,2},  	// 1.Piano
	{8,2,6,1,2,0,3},    	// 2.Accordion
	{0,3,10,1,5,2048,1},  	// 3.Distorted Synth
	{0,5,5,0,8,0,1},     	// 4.GunShots
	{8,3,4,0,1,0,1},     	// 5.Flute
	{0,8,2,3,2,0,3},     	// 6.Guitar
	{0,9,0,0,4,512,2},   	// 7.Harpsichord
	{3,10,10,1,4,2560,2},  	// 8.Organ
	{7,7,3,1,4,768,3},   	// 9.Oboe
	{0,8,0,0,1,0,3}      	// 10.Xylophone
*/