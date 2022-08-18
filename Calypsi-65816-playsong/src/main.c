// ----------------------------------------------------
// Demo Code for Simple Music Library
// 
// By Ernesto Contreras
// Aug 2022
// ----------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playsong.h"

int main ()
{
// variable to track usedRows for the song
int usedRows;
// strings variables for storing song in Music string format
char trk1[256], trk2[256], trk3[256];			

// Define Tracker Memory
int tracks=3;  		// # of tracks in music score (maximum 3 for SID) 
int rows=300;  		// rows = # of music bars * 16 
int cols=tracks*3;  // calculate required columns, each track requires 3 columns

// Malloc seems to have a limit of assigning 4K blocks (in calypso compiler)
// this limits the max # of cols for a 3 track score to 448 rows 
// 9 bytes * 448 rows = 4032 bytes
// If you have a longer song you can divide your score 
//    into 4K chunks (28 music bar chunks)

// Array that will hold all encoded tracks in memory
unsigned char (*Tracker)[cols];		

// Get enough memory to save all song data 
	Tracker = malloc (sizeof (unsigned char[rows][cols]));

// clean memory in array before adding the music data
	cleanTracks(rows,cols,Tracker);
					
// Add music data
	strcpy (trk1,"I1VfRwRhRB5qAtGtFtEtRhRwRwRhRqB5AtGtFtEtRhEwG5EEEAFFFAFAFBGBGRwRhRB5AtGtFtEtRFGhG6");
	strcpy (trk2,"I1D4B3BBD4B3BRRhD4CB3C4D4hD4B3BBD4B3BRRhD4qD#4RwRB3BBRD4D4D4F#3D4F#3D4G3BGBD4B3BBD4B3BRRhD4RRhG3");
	strcpy (trk3,"I1RG3GGRGGRRwRwRGGGRGGRRwRwRGGGRF#F#F#RwRwRGGGRGGRRwRh.");
// Show music encoded strings											
	printf ("\r\rTrack1: %s\r\rTrack2: %s\r\rTrack3: %s\r",trk1,trk2,trk3);
// Play Tune w/piano
	printf("\rPlaying with Piano\r");	
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Accordion
	printf("\rPlaying with Accordion\r");	
	trk1[1]='2';
	trk2[1]='2';
	trk3[1]='2';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Distorted Synth	

	printf("\rPlaying with Distorted Synth\r");	
	trk1[1]='3';
	trk2[1]='3';
	trk3[1]='3';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140

// Play Tune w/Gun Shots
	printf("\rPlaying with Gun Shots!\r");	
	trk1[1]='4';
	trk2[1]='4';
	trk3[1]='4';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Flute
	printf("\rPlaying with Flute\r");	
	trk1[1]='5';
	trk2[1]='5';
	trk3[1]='5';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Guitar
	printf("\rPlaying with Guitar\r");	
	trk1[1]='6';
	trk2[1]='6';
	trk3[1]='6';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Harpsichord
	printf("\rPlaying with Harpsichord\r");	
	trk1[1]='7';
	trk2[1]='7';
	trk3[1]='7';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Organ
	printf("\rPlaying with Organ\r");	
	trk1[1]='8';
	trk2[1]='8';
	trk3[1]='8';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Oboe
	printf("\rPlaying with Oboe\r");	
	trk1[1]='9';
	trk2[1]='9';
	trk3[1]='9';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140
// Play Tune w/Xylophone
	printf("\rPlaying with Xylophone\r");	
	trk1[1]='a';
	trk2[1]='a';
	trk3[1]='a';
	cleanTracks(rows,cols,Tracker); // Clean memory before rendering the music data
	usedRows = addTrack (trk1,1,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk2,2,rows,cols,Tracker); 	// Add simple tune in Track 1
	usedRows = addTrack (trk3,3,rows,cols,Tracker); 	// Add simple tune in Track 1
	resetSID(); 	
	playSongSID(usedRows,cols,Tracker,140);				// Play the song stored in Tracks @ Tempo 140

// Clean up audio from the SID Chip
	resetSID(); 										
// Show 'n' rows of Track music as is stored in memory	
//	showTracks (usedRows,cols,Tracker);						
// Return Memory requested to malloc	
	free (Tracker);	
// End Program	
	return (0);
}	
