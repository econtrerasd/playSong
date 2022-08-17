// ------------------------------------------------
// playsong.c
// Functions to implement the Simple Music commands
// ------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function prototype
int addTrack(char tune[], int trackNumber, int rows, int cols, unsigned char (*Tracker)[cols]);
void cleanTracks(int rows,int cols,unsigned char (*Tracker)[cols]);
void showTracks(int rows,int cols,unsigned char (*Tracker)[cols]);
void playSongSID(int rows,int cols, unsigned char (*Tracker)[cols], int tempo);
int setInstrumentSID(int voice, int instrument);
void delay (float value);
void resetSID();

// SID DEFINITIONS (Foenix U+ C256)
#define SID0_V1_FREQ_LO          (*(volatile __far uint8_t *)0xafe400)  //voice 1 Freq Low (Write Only)
#define SID0_V1_FREQ_HI          (*(volatile __far uint8_t *)0xafe401)  //voice 1 Freq Hi  (Write Only)
#define SID0_V1_PW_LO            (*(volatile __far uint8_t *)0xafe402)  //voice 1 Pulse Width Low (Write Only)
#define SID0_V1_PW_HI            (*(volatile __far uint8_t *)0xafe403)  //voice 1 Pulse Width Hi (Write Only)   [Unused (bits 7-4), Pulse Hi Nibble (bits 0-3)]
#define SID0_V1_CTRL             (*(volatile __far uint8_t *)0xafe404)  //voice 1 Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc3->Osc1 (bit 2), Sync Osc1->Osc3 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
#define SID0_V1_ATCK_DECY        (*(volatile __far uint8_t *)0xafe405)  //voice 1 Attack/Decay (Write Only)     [Attack (bits 7-4), Decay (bits 0- 3)]
#define SID0_V1_SSTN_RLSE        (*(volatile __far uint8_t *)0xafe406)  //voice 1 Sustain/Release (Write Only)  [Sustain (bits 7-4), Release (bits 0- 3)]
#define SID0_V2_FREQ_LO          (*(volatile __far uint8_t *)0xafe407)  //voice 2 Freq Low (Write Only)
#define SID0_V2_FREQ_HI          (*(volatile __far uint8_t *)0xafe408)  //voice 2 Freq Hi  (Write Only)
#define SID0_V2_PW_LO            (*(volatile __far uint8_t *)0xafe409)  //voice 2 Pulse Width Low (Write Only)
#define SID0_V2_PW_HI            (*(volatile __far uint8_t *)0xafe40A)  //voice 2 Pulse Width Hi (Write Only)   [Unused (bits 7-4), Pulse Hi Nibble (bits 0-3)]
#define SID0_V2_CTRL             (*(volatile __far uint8_t *)0xafe40B)  //voice 2 Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc1->Osc2 (bit 2), Sync Osc1->Osc2 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
#define SID0_V2_ATCK_DECY        (*(volatile __far uint8_t *)0xafe40C)  //voice 2 Attack/Decay (Write Only)     [Attack (bits 7-4), Decay (bits 0- 3)]
#define SID0_V2_SSTN_RLSE        (*(volatile __far uint8_t *)0xafe40D)  //voice 2 Sustain/Release (Write Only)  [Sustain (bits 7-4), Release (bits 0- 3)]
#define SID0_V3_FREQ_LO          (*(volatile __far uint8_t *)0xafe40E)  //voice 2 Freq Low (Write Only)
#define SID0_V3_FREQ_HI          (*(volatile __far uint8_t *)0xafe40F)  //voice 2 Freq Hi  (Write Only)
#define SID0_V3_PW_LO            (*(volatile __far uint8_t *)0xafe410)  //voice 2 Pulse Width Low (Write Only)
#define SID0_V3_PW_HI            (*(volatile __far uint8_t *)0xafe411)  //voice 2 Pulse Width Hi (Write Only)   [Unused (bits 7-4), Pulse Hi Nibble (bits 0-3)]
#define SID0_V3_CTRL             (*(volatile __far uint8_t *)0xafe412)  //voice 2 Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc2->Osc3 (bit 2), Sync Osc2->Osc3 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
#define SID0_V3_ATCK_DECY        (*(volatile __far uint8_t *)0xafe413)  //voice 2 Attack/Decay (Write Only)     [Attack (bits 7-4), Decay (bits 0- 3)]
#define SID0_V3_SSTN_RLSE        (*(volatile __far uint8_t *)0xafe414)  //voice 2 Sustain/Release (Write Only)  [Sustain (bits 7-4), Release (bits 0- 3)]
#define SID0_FC_LO               (*(volatile __far uint8_t *)0xafe415)  //Filter Cutoff Low (Write Only)        [Unused (bits 7-4), Low-Nybble Bits (3-0)]
#define SID0_FC_HI               (*(volatile __far uint8_t *)0xafe416)  //Filter Cutoff Hi  (Write Only)        
#define SID0_RES_FILT            (*(volatile __far uint8_t *)0xafe417)  //Resonance/Filter Enable  (Write Only) [Resonance (bits 4-7), Filter voice 3 (bit 2), Filter voice 2 (bit 1), Filter voice 1 (bit 0)]
#define SID0_MODE_VOL            (*(volatile __far uint8_t *)0xafe418)  //Volume/Filter Type  (Write Only)      [High pass (bit 6), Bandpass (bit 5), Low pass (bit 4), Volume (bits 0- 3 )]
#define SID0_POT_X               (*(volatile __far uint8_t *)0xafe419)  //POT X (C256 - NOT USED)
#define SID0_POT_Y               (*(volatile __far uint8_t *)0xafe41A)  //POT Y (C256 - NOT USED)
#define SID0_OSC3_RND            (*(volatile __far uint8_t *)0xafe41B)  //OSC3 / RANDOM (Read Only)
#define SID0_ENV3                (*(volatile __far uint8_t *)0xafe41C)  //Misc (Read Only)  - ENV3
#define SID0_U1                  (*(volatile __far uint8_t *)0xafe41D)  //SID - L - NOT USED
#define SID0_U2                  (*(volatile __far uint8_t *)0xafe41E)  //SID - L - NOT USED
#define SID0_U3                  (*(volatile __far uint8_t *)0xafe41F)  //SID - L - NOT USED


const char notes[96][4]={
	"C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0",
	"C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1",
	"C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
	"C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3",
	"C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
	"C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
	"C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6",
	"C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7"
};

const float frequency[]  // Equal tempered scale values (Frequency)
={
	16.3520,17.3200,18.3500,19.4500,20.6000,21.8300,23.1200,24.5000,25.9600,27.5000,29.1352,30.8677,
	32.7032,34.6478,36.7081,38.8909,41.2034,43.6535,46.2493,48.9994,51.9131,55.0000,58.2705,61.7354,
	65.4064,69.2957,73.4162,77.7817,82.4069,87.3071,92.4986,97.9989,103.826,110.000,116.541,123.471,
	130.813,138.591,146.832,155.563,164.814,174.614,184.997,195.998,207.652,220.000,233.082,246.942,
	261.626,277.183,293.665,311.127,329.628,349.228,369.994,391.995,415.305,440.000,466.164,493.883,
	523.251,554.365,587.330,622.254,659.255,698.456,739.989,783.991,830.609,880.000,932.328,987.767,
	1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53,
	2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07
};

const unsigned char SidNotes[96][2]={  // Sid HI, Sid Low
	{1,12},{1,28},{1,45},{1,62},{1,81},{1,102},{1,123},{1,145},{1,169},{1,195},{1,221},{1,250}, //Octave0
	{2,24},{2,56},{2,90},{2,125},{2,163},{2,204},{2,246},{3,35},{3,83},{3,134},{3,187},{3,244}, //Octave1
	{4,48},{4,112},{4,180},{4,251},{5,71},{5,152},{5,237},{6,71},{6,167},{7,12},{7,119},{7,233}, //Octave2
	{8,97},{8,225},{9,104},{9,247},{10,143},{11,48},{11,218},{12,143},{13,78},{14,24},{14,239},{15,210}, //Octave3
	{16,195},{17,195},{18,209},{19,239},{21,31},{22,96},{23,181},{25,30},{026,156},{28,49},{29,223},{31,165}, //Octave4
	{33,135},{35,134},{37,162},{39,223},{42,62},{44,193},{47,107},{50,60},{53,57},{56,99},{59,190},{63,75}, //Octave5
	{67,15},{71,12},{75,69},{79,191},{84,125},{89,131},{94,214},{100,121},{106,115},{112,199},{119,124},{126,151}, //Octave6
	{134,30},{142,24},{150,139},{159,126},{168,250},{179,6},{189,172},{200,243},{212,230},{225,143},{238,248},{253,246}  //Octave7
};

// list of Instruments from c128 user manual
const int SID_envelope[16][8]={
	// atk, dcy, sust, rel, wave [8-Noise Wave, 4-Pulse Wave, 2-Saw Wave, 1-Triangle Wave], pulseWidth, note length [1-normal, 2-legato (long), 3-staccato (short)]
	// note it's rarely used but you can add wave numbers to produce a more complex wave!! - see Distorted Synth, it has 5 as wave value (4 pulse + 1 triangle)
	{0,9,0,0,4,2048,2},  // 1.Piano
	{8,2,6,1,2,0,3},    // 2.Accordion
	{0,3,10,1,5,2048,1},  // 3.Distorted Synth
	{0,5,5,0,8,0,1},     // 4.GunShots
	{8,3,4,0,1,0,1},     // 5.Flute
	{0,8,2,3,2,0,3},     // 6.Guitar
	{0,9,0,0,4,512,2},   // 7.Harpsichord
	{3,10,10,1,4,2560,2},  // 8.Organ
	{7,7,3,1,4,768,3},   // 9.Oboe
	{0,8,0,0,1,0,3}      // 10.Xylophone
};

const float noteLen[4]={0.75, 1.00 ,0.51}; // notelength for voices 1-3 (normal, legatto, staccatto)

int addTrack(char tune[], int trackNumber, int rows, int cols, unsigned char (*Tracker)[cols])
{
	int tuneSize, c, t, pos, octave, octaveSubs, instrument, duration, volume, index;
	bool noteFlag, modFlag;
	char note[5];
	int gateOff, lengthNote;
	
	// set default value for parameters
	
	octave=4; // set default octave
	instrument=1; // set default Instrument (SID Piano)
	volume=10; // set default volume 
	trackNumber-=1; // First Tracks for users is track 1, but for functions it is track 0
	
	// Initialize counters
	c=0;t=0; octaveSubs=0;
	
	tuneSize = strlen(tune);
	
	while (c<tuneSize)
	{
		pos=0;
		octaveSubs=0;
		modFlag=false;
		noteFlag=false;
		// is code a note (A-G) or rest (R)?, if it is check the following: 
		// 1. is it sharp or flat? 
		// 2. has a specified Octave?
		// 3. has a duration?
		if ((tune[c]>='A' && tune[c]<='G') || (tune[c]=='R'))
		{
			noteFlag=true;
			// copy base note
			note[0]=tune[c];
			pos++;
			c++;
			if (tune[c] =='#')
			{
				// Verify it's a valid Sharp note!
				if (note[0]!='B' && note[0]!='E')
				{
					// Make it Sharp!	
					note[1]=tune[c];
					pos++;
				}
				c++;
			}
			else
			if (tune[c] =='$')
				// is it a flat note?
			{
				// modify note to be represented with a Sharp
				if (tune[c-1]==65)
					{
						note[0]='G';
						if (octave > 0)
							{octaveSubs=1;}
					}
				else
					{note[0]=tune[c-1]-1;}
				// Add Sharp # symbol if needed
				if (note[0]!='E' && note[0]!='B' && note[0]!='R')
				{
					note[1]='#';
					pos++;
				}
				c++;
			}
			// ---------------
			// OCTAVE
			// ---------------
			if (tune[c] >='0' && tune[c]<='7') 
			{
			// verify if Octave was provided after note
				octave = tune[c]-48; // modify current octave number
				note[pos]=48+octave-octaveSubs; // get octave & add OctaveSubs just in case!
				c++;
			}
			else
			{
				// add Octave to note
				note[pos]=48+octave-octaveSubs;
			}
			// ---------------
			// LENGTH
			// ---------------
			switch (tune[c])
			{
				case 'w': // whole
					duration=16;
					c++;
					break;
				case 'h': // half
					duration=8;
					c++;
					break;
				case 'q': // quarter
					duration=4;
					c++;
					break;
				case 't': // eight
					duration=2;
					c++;
					break;
				case 's': // sixteenth
					duration=1;
					c++;
					break;
				default:
					// if no duration is specified use quarter
					duration=4;
			}
			// Check for dotted duration
			if (tune[c]=='.')
			{
				duration+=duration/2;
				c++;
			}
		}
		else
		{
		//--------------------------
		// OTHER OPTIONS
		//--------------------------
			// no note was provided, only parameter changed!
			note[0]='\0';
			noteFlag=false;
			// Modify Volume
			if (tune[c]=='V')
			{
				modFlag=true;
				c++;
				if (tune[c]>='0' && tune[c]<='9')
					{
						volume=tune[c]-48;
						c++;
					}
				if (tune[c]>='a' && tune[c]<='f')
					{	
						volume=tune[c]-87;
						c++;
					}
			}
			// Modify Octave
			if (tune[c]=='O')
			{
				modFlag=true;
				c++;
				if (tune[c]>='0' && tune[c]<='7')
				{
					octave=tune[c]-48;
					c++;
				}
			}
			// Modify Instrument
			if (tune[c]=='I')
			{
				modFlag=true;
				c++;
				if (tune[c]>='0' && tune[c]<='9')
				{
					instrument=tune[c]-48;
					c++;
				}
				if (tune[c]>='a' && tune[c]<='f')
				{
					instrument=tune[c]-87;
					c++;
				}
			}
		}
			note[++pos]='\0';
		//-------------------------------
		// Output Note in Track notation
		//-------------------------------
		if (noteFlag==true)
		{	
			t++;
			
			// Calculate normal, legatto or staccato duration of note 
			lengthNote=SID_envelope[instrument-1][6];
			gateOff= (int) (duration*noteLen[lengthNote-1]);
			if (gateOff<=1)
				gateOff=2; // if gateoff is calculated on the same 1/16th period as gateon we won't use parameter.
			//printf("Duration:%d lengthNote:%d factor:%f calc:%d GateOff: %d\r",duration,lengthNote,noteLen[lengthNote-1],(int) (duration*noteLen[lengthNote-1]),gateOff);
			
			index=0;
			if (note[0]=='R') // Rest
			{
				Tracker[t][(trackNumber*3)]=100;
				Tracker[t][(trackNumber*3)+1]=volume;
				Tracker[t][(trackNumber*3)+2]=instrument;
				while (--duration>0)
				{
					t+=1;
					Tracker[t][(trackNumber*3)]=0;
					Tracker[t][(trackNumber*3)+1]=0;
					Tracker[t][(trackNumber*3)+2]=0;
				}
			}
			else 
			{ 
				// search for Note
				while (index<=96)
				{
					if (strcmp(note,notes[index])==0)
						break;
					index++;
				}
				Tracker[t][(trackNumber*3)]=index;
				Tracker[t][(trackNumber*3)+1]=volume;
				Tracker[t][(trackNumber*3)+2]=instrument;
				
				while (--duration>0)
				{
					t+=1;
					if (gateOff==duration)
					{
						Tracker[t][(trackNumber*3)]=101;
						Tracker[t][(trackNumber*3)+1]=volume;
						Tracker[t][(trackNumber*3)+2]=instrument;
					}
					else
					{
						Tracker[t][(trackNumber*3)]=0;
						Tracker[t][(trackNumber*3)+1]=0;
						Tracker[t][(trackNumber*3)+2]=0;
					}
				}
			}
		}
		// Check for Errors on Song string
		if (noteFlag==false && modFlag==false && c!=tuneSize)
		{
			// invalid character
			for (index=0;index<=c;index++)
				printf("%c",tune[index]);
			printf ("<- Unexpected character\r");
			exit(-1);
		}
	}
	return t;
}

// Clean Track Memory
void cleanTracks(int rows,int cols,unsigned char (*Tracker)[cols])
{
	int i,j;
	for (i=0;i<rows;i++)
		for (j=0;j<cols;j++)
			Tracker[i][j]=0;
}

// Print Track Memory
void showTracks(int rows,int cols, unsigned char (*Tracker)[cols])
{
	int i,j;
	for (i=0;i<=rows;i++)
	{
		printf("%d-",i);
		for (j=0;j<cols;j++)
		{
			printf("%d ",Tracker[i][j]);
		}
		printf("\r");
	}
}

// Play Trak in Track Memory
void playSongSID(int rows,int cols, unsigned char (*Tracker)[cols], int tempo)
{
	int i,j,note,volume,instrument,wave1,wave2,wave3;
	unsigned char hiFreq, loFreq;
	int tracks=cols/3;
	
	// Initialize SID CHIP
	resetSID();
	
	// set the Default Instrument to Piano for the 3 voices
	 wave1=setInstrumentSID(1, 1);
	 wave2=setInstrumentSID(2, 1);
	 wave3=setInstrumentSID(3, 1);
	
	// Init Music playing
	for (i=0;i<=rows;i++)
	{
		for (j=0;j<tracks;j++)
		{
			// read parameters to play from Memory
			note=Tracker[i][(j*3)]; 
			volume = Tracker[i][(j*3)+1];
			instrument=Tracker[i][(j*3)+2];
			
			// If there is a note to play
			if (note != 0)
			{
				hiFreq=SidNotes[note][0];
				loFreq=SidNotes[note][1];
				switch (j)
				{
					case 0: // play voice 1
						// Gate Out previous note
						SID0_V1_CTRL = wave1-1;  
						// Set Volume 
						SID0_MODE_VOL=volume;  // Set sid chip volume, note! volume can only be controlled in track 1! 
						// if there's a new instrument change it before playing note
						wave1=setInstrumentSID(1, instrument);

						if (note<=100)
						{
							// play new note 							
							SID0_V1_FREQ_LO = loFreq; 
							SID0_V1_FREQ_HI = hiFreq;
						}
							SID0_V1_CTRL=wave1; // Gate in New note!						
					break;
					case 1:  // play voice 2
						// Gate Out previous note
						SID0_V2_CTRL = wave2-1;
						// if there's a new instrument change it before playing note
						wave2=setInstrumentSID(2, instrument);
						if (note<=100)
						{
							// play new note 
							SID0_V2_FREQ_LO = loFreq; 
							SID0_V2_FREQ_HI = hiFreq;
						}
							SID0_V2_CTRL=wave2; // Gate in New note!						
					break;
					case 2: // play voice 3
						// Gate Out previous note
						SID0_V3_CTRL = wave3-1;  
						// if there's a new instrument change it before playing note
						wave3=setInstrumentSID(3, instrument);
						if (note<100)
						{
							// play new note 
							SID0_V3_FREQ_LO = loFreq; 
							SID0_V3_FREQ_HI = hiFreq;
						}
							SID0_V3_CTRL=wave3; // Gate in New note!
					break; 
				}
			}
		}
		delay( (65535*5)/tempo); // wait for 1/16th
	}
	SID0_V1_CTRL=wave1-1; // Gate out last note!
	SID0_V2_CTRL=wave2-1; // Gate out last note!
	SID0_V3_CTRL=wave3-1; // Gate out last note!
}


int setInstrumentSID(int voice, int instrument)
{
	int attack,decay,sustain,release,waveform,pulseWidth;
	
	instrument-=1;
	
	attack = SID_envelope[instrument][0];
	decay=SID_envelope[instrument][1];
	sustain=SID_envelope[instrument][2];
	release=SID_envelope[instrument][3];
	waveform=SID_envelope[instrument][4];
	pulseWidth=SID_envelope[instrument][5];		

	//printf(".");
	//printf("Voice %d - A:%d D:%d S:%d R:%d WF:%d PWL:%d PWH:%d\r",voice,attack,decay,sustain,release,waveform,pulseWidth & 0x00FF, (pulseWidth - (pulseWidth & 0x00FF))/256);
	
	// configure selected Voice
	switch (voice)
	{
		case 1:
			// SET VOICE 1 Parameters
			// set Attack/Decay /Sustain/release parameters	
			SID0_V1_ATCK_DECY = attack*16+decay;
			SID0_V1_SSTN_RLSE = sustain*16+release;
			// Set PulseWidth
			SID0_V1_PW_LO = pulseWidth & 0x00FF;
			SID0_V1_PW_HI = (int) pulseWidth /256;
			//Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc1->Osc2 (bit 2), Sync Osc1->Osc2 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
			return waveform*16+1;
			break;
		case 2:
			// SET VOICE 2 Parameters
			// set Attack/Decay /Sustain/release parameters	
			SID0_V2_ATCK_DECY = attack*16+decay;
			SID0_V2_SSTN_RLSE = sustain*16+release;
			// Set PulseWidth
			SID0_V2_PW_LO = pulseWidth & 0x00FF;
			SID0_V2_PW_HI = (int) pulseWidth /256;
			//Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc1->Osc2 (bit 2), Sync Osc1->Osc2 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
			return waveform*16+1;
			break;
		case 3:
			// SET VOICE 3 Parameters
			// set Attack/Decay /Sustain/release parameters	
			SID0_V3_ATCK_DECY = attack*16+decay;
			SID0_V3_SSTN_RLSE = sustain*16+release;
			// Set PulseWidth
			SID0_V3_PW_LO = pulseWidth & 0x00FF;
			SID0_V3_PW_HI = (int) pulseWidth /256;
			//Control Register (Write Only) [Noise Wave (bit 7), Pulse Wave (bit 6), Saw Wave (bit 5), Triangle Wave (bit 4), Test (bit 3), Ring Modulate Osc1->Osc2 (bit 2), Sync Osc1->Osc2 (bit 1), Gate Bit (bit 0, 1-initiate Atk/sus/Dec, 0 - Initiate Rel)]
			return waveform*16+1;
			break;
	}
	return 0;
}

void delay (float value)
{
	// Just a Busy loop to waste time!
	float i;
	for (i=0;i<value;i++)
		;
}

void resetSID()
{
	SID0_V1_FREQ_LO=0;
	SID0_V1_FREQ_HI=0;
	SID0_V1_PW_LO=0;
	SID0_V1_PW_HI=0;
	SID0_V1_CTRL=0;
	SID0_V1_ATCK_DECY=0;
	SID0_V1_SSTN_RLSE=0;
	SID0_V2_FREQ_LO=0;
	SID0_V2_FREQ_HI=0;
	SID0_V2_PW_LO=0;
	SID0_V2_PW_HI=0;
	SID0_V2_CTRL=0;
	SID0_V2_ATCK_DECY=0;
	SID0_V2_SSTN_RLSE=0;
	SID0_V3_FREQ_LO=0;
	SID0_V3_FREQ_HI=0;
	SID0_V3_PW_LO=0;
	SID0_V3_PW_HI=0;
	SID0_V3_CTRL=0;
	SID0_V3_ATCK_DECY=0;
	SID0_V3_SSTN_RLSE=0;
	SID0_FC_LO=0;
	SID0_FC_HI=0;
	SID0_RES_FILT=0;
	SID0_MODE_VOL=0;
}