// -------------------------------------------------------------
// Foenix C256 - Graphics Library VickyGraph.c
// Compiled by Ernesto Contreras
// May 2022
// -------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
// Foenix Includes
#include <vicky.h>
#include <dma.h>
#include "support.h"
#include "vickyGraph.h"


// Global Variables for Stack
#define STACK_MAXSIZE 128
int stack[128][2];
int stackTop;

// PI value used for arc function
#define PI 3.141592

// Global Variables that hold screen resolution / current bitmap page / maxVickyMemory 
long vickyResX;
long vickyResY;
int  vickyBitmapPage;
long maxVickyMemory;
bool vBitPlane[2];

// Define default palette
// Not used right now, need to implement a function to manually load the palette 


// A FEW ANSI FUNCTIONS - JUST BASIC SUPPORT!

// --------------------------------------------------
// ANSI Code Wrapper Functions to make code readable
// By Ernesto Contreras 
// --------------------------------------------------
void ANSILocate (int row, int col)
{
	printf("\033[%d;%dH",row,col);
}
	
void ANSIClrScr ()
{
	printf ("\033[2J"); // ANSI clear screen
}

// I'm not sure this has any effect...
void ANSIScrSize (int x, int y)
{
	printf("\033e[8;%d,%d;1H",x,y); // SET ANSI screen size to x,y
}

// ---------------------------------------------------
// Vicky II wrapper function to help inititate Graphics mode
// and do some really basic validations
// By Ernesto Contreras 
// --------------------------------------------------
int vickyVideomode (bool text,bool txtOverlay,bool bitmap,bool tiles,bool sprites,bool gammaFix,bool blank,int resolution)
{
	int length;
	// init graphic mode variables and stack
	// Valid resolution values 
	// --------------------------
	// 0 - 640 x 480
	// 1 - 800 x 600
	// 2 - 320 x 240
	// 3 - 400 x 300

	int intValue;
	int intResolution;

// Setup initial values 
	intValue=0;
	// global variables
  	vickyResX=0;
	vickyResY=0;
	vickyBitmapPage=0;

	if (FOENIX_MODEL == Model_U256) 
	{
		maxVickyMemory = 0x1fffff;
	}
	else 
	{
		maxVickyMemory = 0x3fffff;
	}
    printf ("Memory detected: %lx \n\r",maxVickyMemory); 

	if (text) {intValue+=1;}  // enable text mode 
	if (txtOverlay) {intValue+=2;} // enable text overlay 
	if (bitmap || tiles || sprites) {intValue+=4;} // enable graphic module
	if (bitmap) {intValue+=8;} // enable bitmap engine
	if (tiles) {intValue+=16;} // enable tiles engine
	if (sprites) {intValue+=32;} // enable sprites engine 
	if (gammaFix) {intValue+=64;} // use gammaFix 
	if (blank) {intValue+=128;} // turn off screen 
	if (resolution>=0 && resolution<4) // resolution selected (0-3)
	{
		intResolution=resolution*256;
		MASTER_CTRL_REG =  intResolution+intValue;

		vickyResX=0;
		vickyResY=0;

		switch (resolution)
		{
		case 0:
			vickyResX=640;
			vickyResY=480;
			ANSIScrSize (80, 60);
			TEXT_COLS_VISIBLE = 80;
			TEXT_ROWS_VISIBLE = 60;
			break;
		case 1:
			vickyResX=800;
			vickyResY=600;
			ANSIScrSize (100, 75);
			TEXT_COLS_VISIBLE = 100;
			TEXT_ROWS_VISIBLE = 75;
			break;
		case 2:
			vickyResX=320;
			vickyResY=240;
			ANSIScrSize (40, 30);
			TEXT_COLS_VISIBLE = 40;
			TEXT_ROWS_VISIBLE = 30;
			break;
		case 3:
			vickyResX=400;
			vickyResY=300;
			ANSIScrSize (50, 37);
			TEXT_COLS_VISIBLE = 50;
			TEXT_ROWS_VISIBLE = 38;
			break;
		default:
			vickyResX=0;
			vickyResY=0;
		}
		printf ("graphics mode %d x:%ld y:%ld\n\r",resolution,vickyResX,vickyResY);
		return intResolution+intValue;	
	}
	else {return -1;}
}

// ---------------------------------------------------
// Vicky II wrapper functions to help setup bitmap
// and do some really basic validations
// By Ernesto Contreras 
// --------------------------------------------------
int vickyBitmap(int page, bool enable, long address, int lut, bool collision)
{
// page - Value between 0-1 to select one of the two bitmaps supported by Vicky II
// enable - true or false to show/hide bitmap 
// address - {0-0x1fffff} (2 Mb)| {0-0x3fffff} (4 Mb)
// lut - {0-7} select one of the 8 palettes
// 	collision - true | false  (Enable collision with the bitmap)

	int intEnable;
	int intCollision;
	int intLut;
	
	// Translate Control Bits value
	//Bit 0 = disable/enable
	//Bit 1-3 = Target LUT address located at AF:2000 and up.
	//Bit 6 = Bitmap Collision On
	
	if (enable) {intEnable=1;} else {intEnable=0;}
	if (collision) {intCollision=32;} else {intCollision=0;}
	if (lut>=0 && lut <8)   {intLut=lut*2;}
	
	if (address >= 0 && address < (maxVickyMemory-(vickyResX*vickyResY)))
	{
		if (page==0 || page ==1)
		{
			vickyBitmapPage=page; // set defined page as current Page
			bitplane[page].control = intEnable+intLut+intCollision;
			bitplane[page].start = (__far vram_ptr) address;
			bitplane[page].x_offset = 0;
			bitplane[page].y_offset = 0;
			bitplane[page].reserved = 0;
			vBitPlane[page]=enable;
			return 0;
		}
			else 
			{
				printf ("Bitmap page must be 0 or 1!");
				return -1;
			} // Page number must be between 1-2 Error
	}
		else  
		{
			printf ("Not enough memory at address %lx for a d% by d% screen!, %xl bytes required.\n\r",address,vickyResX,vickyResY,(long)vickyResX*vickyResY);
			printf ("VRAM detected: 0xb00000 - %lx\n\r",maxVickyMemory);
			return -2;
		} // out of memory range Error
}

void vickyDefPalette ()
{
	uint8_t __far *ptr;
    uint8_t palette[] ={
	0,0,0,255,0,0,0,255,35,35,34,104,73,69,67,139,
	113,104,98,255,152,139,130,255,186,174,166,93,200,200,200,117,
	84,93,98,255,101,117,133,255,121,140,158,175,137,161,174,195,
	164,175,187,255,177,195,204,255,201,219,234,49,214,243,255,61,
	38,49,88,255,59,61,115,255,65,80,136,110,76,98,154,141,
	81,110,173,255,107,141,213,255,132,170,251,39,127,206,255,56,
	53,39,0,255,80,56,0,255,94,77,0,111,127,102,11,140,
	137,111,0,255,167,140,50,255,214,174,36,43,255,214,136,54,
	41,43,102,255,58,54,148,255,70,77,182,120,70,94,205,155,
	64,120,227,255,78,155,249,255,78,188,255,43,73,233,255,69,
	74,43,40,255,104,69,58,255,132,95,97,144,153,119,122,178,
	178,144,134,255,217,178,150,255,255,214,199,34,255,236,198,50,
	25,34,0,255,33,50,0,255,27,74,23,105,24,89,34,136,
	12,105,47,255,34,136,81,255,45,164,125,31,52,204,166,50,
	47,31,24,255,77,50,35,255,107,70,37,142,138,107,54,178,
	184,142,49,255,227,178,65,255,255,210,82,51,253,245,116,63,
	44,51,26,255,56,63,47,255,64,81,56,116,64,92,50,137,
	85,116,65,255,96,137,73,255,125,182,85,7,161,218,145,33,
	17,7,94,255,29,33,130,255,53,60,182,118,95,92,228,155,
	118,118,255,255,168,155,255,255,199,187,255,49,255,219,255,71,
	54,49,45,255,77,71,72,255,105,92,91,135,127,115,115,174,
	149,135,132,255,190,174,171,255,219,199,186,48,246,240,235,60,
	60,48,59,255,69,60,90,255,88,82,138,130,96,107,174,159,
	108,130,199,255,117,159,216,255,129,197,236,34,171,250,255,53,
	42,34,49,255,60,53,74,255,70,70,94,108,81,90,114,138,
	84,108,126,255,110,138,158,255,136,165,192,16,154,191,221,40,
	38,16,46,255,61,40,73,255,89,54,102,109,117,84,151,120,
	145,109,185,255,170,120,193,255,191,153,219,46,218,198,248,64,
	73,46,0,255,81,64,0,255,98,81,0,130,109,107,0,160,
	121,130,0,255,135,160,0,255,163,191,0,255,218,222,0,74,
	37,49,69,255,60,74,97,255,68,97,126,144,81,121,153,169,
	98,144,178,255,110,169,204,255,130,203,232,9,163,234,251,36,
	38,9,95,255,52,36,110,110,71,70,144,100,87,96,167,112,
	100,125,189,189,112,151,206,206,124,182,237,237,147,212,237,128,
	88,53,50,50,128,82,74,74,157,101,100,100,193,119,120,239,
	226,140,142,142,239,155,156,156,255,174,184,184,255,212,220,59,
	41,23,67,67,59,43,113,113,82,59,159,159,105,74,217,175,
	128,93,248,248,175,125,255,255,197,166,255,255,255,205,255,50,
	28,37,73,73,50,52,99,99,71,75,124,124,90,89,152,122,
	110,111,172,172,122,126,193,193,122,141,210,210,124,154,229,8,
	0,41,32,32,8,79,47,47,0,93,73,73,8,115,97,38,
	30,131,124,124,38,154,150,150,51,170,180,180,50,204,208,9,
	0,42,98,98,9,59,117,117,18,79,133,133,32,101,158,57,
	46,136,186,186,57,170,209,209,75,210,232,232,79,246,255,85,
	61,35,38,38,85,56,59,59,111,80,86,86,110,104,117,131,
	123,122,145,145,131,151,179,179,142,175,207,207,177,223,254,71,
	67,44,29,29,71,61,46,255,60,77,57,57,51,95,76,45,
	44,113,88,88,45,132,107,197,36,158,120,120,57,189,127,58,
	35,36,55,55,58,57,83,83,73,76,120,120,79,93,148,99,
	88,109,169,169,99,126,191,191,116,147,215,215,128,163,244,90,
	71,75,45,45,90,101,71,71,105,123,91,91,125,149,113,150,
	142,174,135,135,150,193,138,138,193,209,169,169,235,250,224,95,
	64,27,0,0,95,49,3,3,124,72,7,7,162,93,16,234,
	192,118,20,20,234,151,64,64,241,177,85,85,255,204,109,115,
	105,71,85,85,115,93,118,118,136,116,151,151,147,140,185,157,
	154,163,213,213,157,189,235,235,155,213,255,255,134,247,253,81,
	33,29,29,29,81,49,60,60,127,74,88,88,186,100,121,236,
	241,133,149,149,236,150,169,169,247,171,186,186,254,189,209,93,
	80,36,38,38,93,51,40,40,114,61,45,45,131,80,61,197,
	174,101,81,81,197,116,82,82,196,130,108,108,195,147,131,74,
	41,33,73,73,74,65,94,94,91,83,119,119,106,96,145,148,
	132,121,173,173,148,139,181,181,170,174,212,212,207,226,255,39,
	3,28,114,114,39,51,156,156,62,90,191,191,39,134,233,5,
	8,177,255,255,5,207,255,255,43,240,255,255,255,255,255,65};

	int vi, index;
	vi=0;
	for (index=0;index<256;index++)
	{
		ptr= (long)GRPH_LUT0_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= palette[vi++]; // red
		*(ptr+1)= palette[vi++]; // green
		*(ptr)=   palette[vi++]; // blue 
		*(ptr+3)= palette[vi++]; // alpha
	}
}

int vickyPaletteCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	uint8_t __far *ptr;
// select palette address
	switch (palette)
	{
	case 0:
		ptr= (long)GRPH_LUT0_LONG +(index*4);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 1:
		ptr= (long)GRPH_LUT1_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 2:
		ptr= (long)GRPH_LUT2_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 3:
		ptr= (long)GRPH_LUT3_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 4:
		ptr= (long)GRPH_LUT4_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 5:
		ptr= (long)GRPH_LUT5_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 6:
		ptr= (long)GRPH_LUT6_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	case 7:
		ptr= (long)GRPH_LUT7_LONG +(index*4);
		//printf ("ptr:%lx r:%d g:%d b:%d a:%d \r",ptr, palette[vi], palette[vi+1], palette[vi+2], palette[vi+3]);
		// set rgb components at palette address
		*(ptr+2)= red; // red
		*(ptr+1)= green; // green
		*(ptr)=   blue; // blue 
		*(ptr+3)= alpha; // alpha
		break;
	default:
		// LUT table out of bounds (0-7)
		return -1;
	}
	return 0;
}

// int vickyBackCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
// int vickyBorderCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)


// -------------------------------------------------------------------
// Simple Read VRAM address by Ernesto Contreras
// Required for getPixel
// -------------------------------------------------------------------
static inline uint8_t __read_vram (uint8_t volatile __far *p) {
  // Force clear of FIFO
  VMEM2CPU_CTRL_REG = VMEM2CPU_Clear_FIFO;
  VMEM2CPU_CTRL_REG = 0;
	// Wait for Buffer to be clear
	while (__fifo_count > 0)
	;
	*p;  // dummy read
// Wait for fifo buffer 
	while (__fifo_count == 0) 
	;
  return __byte_port;
}

// -------------------------------------------------------------------
// Read VRAM Buffer Implementation 
// thx to hth313 - Calypsi creator for his support!
// Required for getScanLine
// -------------------------------------------------------------------
 int read_vram_buffer (uint8_t volatile __far *p, int length, uint8_t __far *buffer) 
 {
	int i; 
	VMEM2CPU_CTRL_REG = VMEM2CPU_Clear_FIFO; // clear fifo before reading data
	VMEM2CPU_CTRL_REG = 0;
	// Wait for fifo clear
	if (__fifo_count>0)
		while (__fifo_count>0)
		;
// Ask for as may bytes as the value - length
	i=0;
   while (i<length)
  {
	*p;  		// dummy read
	i++;		// increment pixel request counter
	p++;		// increment VRAM pointer
  }
// Wait for fifo buffer 
	while (__fifo_count == 00)
	;
	i=0;
// Move lenght number of bytes into array
	while (__fifo_count > 0) 
	  *(buffer+(i++)) = __byte_port;
   
	VMEM2CPU_CTRL_REG = VMEM2CPU_Clear_FIFO; // clear fifo before reading data
	VMEM2CPU_CTRL_REG = 0;
	return i;
}


// -----------------------------------------------------
// Simple getPixel Implementation by Ernesto Contreras
// -----------------------------------------------------
int getPixel (int x, int y) {
	void __far *ptr;
	uint8_t byte, length;
    if (vBitPlane[vickyBitmapPage]) // if Bitmap for the page is enabled
	{
		// check screen Boundaries
		if (x<vickyResX && y<vickyResY)
		{
			ptr = (void *)(0xb00000+x+y*vickyResX);
			ptr+= (long) bitplane[vickyBitmapPage].start;
			return __read_vram (ptr);
		}
	}
	return -1;
}

// -----------------------------------------------------
// Simple getScanLine Implementation by Ernesto Contreras
// Results in pixBuffer[]
// -----------------------------------------------------
int getScanLine (int y, uint8_t __far *buffer) {
	void __far *ptr,*ptr2;
	int st,length, length2, length3, bank;
    if (vBitPlane[vickyBitmapPage]) // if Bitmap for the page is enabled
	
	// initialize variables 
	st=0;
	// check screen Boundaries
	if (y<vickyResY)
	{
		ptr = (0xb00000+y*vickyResX); // calculate position of scanline in Memory
		ptr+= (long) bitplane[vickyBitmapPage].start;
		length = vickyResX; // read as many bytes as current vickyResX
		if (((long)ptr & 0xff0000)==((long)ptr+length & 0xff0000))
		// Test if bytes requested from FIFO cross a 64k Bank boundary
		{
			// of course they dont!
			while (st<length)
			{
				st = read_vram_buffer(ptr, length, buffer);
				if (st<length)
				{
					// recalculate all pointers and lengths for a retry
					ptr = (0xb00000+y*vickyResX); // calculate position of scanline in Memory
					ptr+= (long) bitplane[vickyBitmapPage].start;
				}
			}
		}
		else
		// Ooops this byte group just crossed the line!, let's do something about it! 
		// Let's do a FIFO request for the group of bytes in each bank
		{
			// Calculate boundaries for both FIFO requests
			bank=(long)(ptr+length) & (0xff0000);
			length2=(bank-0x10000)-(long)ptr-1; // calculate number of bytes to reach end of bank
			length3=vickyResX-length2;
			ptr2=(long)ptr+length & 0xff0000;
			// FIFO for 1st Bank
			st=0;
			while (st<length2)
			{
				st = read_vram_buffer(ptr, length2, buffer);
				if (st<length2)
				{
					// recalculate all pointers and lengths for a retry
					ptr = (0xb00000+y*vickyResX); // calculate position of scanline in Memory
					ptr+= (long) bitplane[vickyBitmapPage].start;
					bank=(long)(ptr+length) & (0xff0000);
					length2=(bank-0x10000)-(long)ptr-1; // calculate number of bytes to reach end of bank
				}
			}

			//FIFO for 2nd Bank
			st=0;
			while (st<length3)
			{
				st = read_vram_buffer(ptr2, length3, (void *)(buffer+length2+1));
				if (st<length3)
				{
					// recalculate all pointers and lengths for a retry
					ptr = (0xb00000+y*vickyResX); // calculate position of scanline in Memory
					ptr+= (long) bitplane[vickyBitmapPage].start;
					bank=(long)(ptr+length) & (0xff0000);
					length2=(bank-0x10000)-(long)ptr-1; // calculate number of bytes to reach end of bank
					length3=vickyResX-length2;
					ptr2=(long)ptr+length & 0xff0000;
				}
			}
		}
	}
	return length; // returns number of bytes read
}

// -----------------------------------------------------
// Simple setPixel Implementation by Ernesto Contreras
// -----------------------------------------------------
static inline void setPixel (unsigned long x, unsigned long y, uint8_t col)
{
uint8_t __far24 *ptr;
	if (vBitPlane[vickyBitmapPage]) // if Bitmap for the page is enabled
		{
			// check screen Boundaries
			if (x<vickyResX && y<vickyResY)
			{
					ptr = (0xb00000+x+y*vickyResX);
					ptr+= (long) bitplane[vickyBitmapPage].start;
					*ptr = col;
			}
		}
}

// -----------------------------------------------------
// Plot function
// wrapper for setPixel.. by Ernesto Contreras
// This function is callable directly from an external program, but can´t be inlined
// -----------------------------------------------------
void plot (unsigned long x, unsigned long y, uint8_t col)
{
	setPixel(x,y,col);
}


// ----------------------------------------------------------
// setPixel_fast by Ernesto Contreras
// removed boundary checking for speed optimization
// Warning: calling function is responsible that the x,y range is not out of bounds or the universe might collapse! (at least for the Foenix..)
// ----------------------------------------------------------
static inline void setPixel_fast (unsigned long x, unsigned long y, uint8_t col)
{
uint8_t __far24 *ptr;
		ptr = (0xb00000+x+y*vickyResX);
		ptr+= (long) bitplane[vickyBitmapPage].start;
		*ptr = col;
}
// ----------------------------------------------------------
// clrBitmap function by Ernesto Contreras
// Modified memset function from Calypsi
// thx to hth313 - Calypsi creator for his memset  support!
// ----------------------------------------------------------
void clrBitmap ()
{uint8_t __far24 *ptr;
	ptr= bitplane[vickyBitmapPage].start;
    VDMA_CONTROL_REG |= VDMA_CTRL_TRF_Fill + 1; //1=Enable!
	VDMA_DST_ADDY = ptr;
    VDMA_BYTE_2_WRITE = 0;
    VDMA_SIZE = (long) (vickyResX*vickyResY);

    // Start VDMA
    VDMA_CONTROL_REG |= VDMA_CTRL_Start_TRF;

    // When started, the 65816 is held, but it takes a few cycles.
    // Provide a short delay to ensure that we do not execute too far
    // before the transfer actually starts. Call a delay subroutine for
    // this.
    DMA_wait_delay();
    wait_for_DMA_to_finish();
}

// ----------------------------------------------------------
// Vram2Dram_linearCopy function  - Experimental & not working yet
// The author is not responsible for any loss of bytes under any circumstance!!
// Author Alonso Contreras (my evil twin)
// ----------------------------------------------------------
void vram2dram_linearCopy ( long source, int __far *buffer, long length)
{uint8_t __far24 *ptr;
	//source+= (long) bitplane[vickyBitmapPage].start;
    VDMA_CONTROL_REG |= VDMA_CTRL_SysRAM_Dst + 1; //1=Enable!
	SDMA_CTRL_REG0 |= SDMA_CTRL0_SysRAM_Dst + 1;

	ptr=source + (long) bitplane[vickyBitmapPage].start;
	// Set source, destination and lenght of copy
	VDMA_SRC_ADDY = ptr;
	SDMA_DST_ADDY = buffer;
    VDMA_SIZE = length;
	SDMA_SIZE = length;
    printf ("source: %x dest: %x len:%ld\r",ptr,buffer,length);
    // Start VDMA
    VDMA_CONTROL_REG |= VDMA_CTRL_Start_TRF;
	SDMA_CTRL_REG0 |= SDMA_CTRL0_Start_TRF;
	printf ("Xfer inititated\r");
    // When started, the 65816 is held, but it takes a few cycles.
    // Provide a short delay to ensure that we do not execute too far
    // before the transfer actually starts. Call a delay subroutine for
    // this.
    DMA_wait_delay();
	printf ("waiting..\r");
    wait_for_DMA_to_finish();
	printf ("Finish..\r");
}

//----------------------------------------------------
// Super Simple Rectangle by Ernesto Contreras
// ---------------------------------------------------
void plot_rectangle (int x0, int y0, int x1, int y1, uint8_t col)
{
	plot_line (x0,y0,x1,y0,col);
	plot_line (x0,y1,x1,y1,col);
	plot_line (x0,y0,x0,y1,col);
	plot_line (x1,y0,x1,y1,col);
}

//-----------------------------------------
// Solid Rectangle by Ernesto Contreras
// Uses VDMA Fill function
//------------------------------------------
void plot_solid_rectangle  (int x0, int y0, int x1, int y1, uint8_t col)
{
uint8_t __far24 *ptr;
int swap;
long address;

	if (x0>x1) // Ensure x1 is greater than x0 
	{ 
	  swap=x0;
	  x0=x1;
	  x1=swap;
	}
	
	if (y0>y1) // Ensure y1 is greater than y0 
	{ 
	  swap=y0;
	  y0=y1;
	  y1=swap;
	}

	// one pixel rectangles are not allowed!
	if (x0==x1) {x1++;}
	if (y0==y1) {y1++;}
	
	// validate coordinates before memset
	if (x0<0) {x0=0;}
	if (y0<0) {y0=0;}
	if (x1<0) {x1=0;}
	if (y1<0) {y1=0;}
	
	if (x0>=vickyResX) {x0=vickyResX-1;}
	if (y0>=vickyResY) {y0=vickyResY;}
	if (x1>=vickyResX) {x1=vickyResX-1;}
	if (y1>=vickyResY) {y1=vickyResY;}

	ptr= bitplane[vickyBitmapPage].start;
	address=(x0 + vickyResX * y0);
	ptr= address + (long)bitplane[vickyBitmapPage].start;
    VDMA_CONTROL_REG |= VDMA_CTRL_TRF_Fill | VDMA_CTRL_1D_2D  + 1; //1 added to Enable!, I believe Calypsi memset function needs this change too!
	VDMA_DST_ADDY = ptr; // calculate destination address 
    VDMA_BYTE_2_WRITE = col;
	VDMA_X_SIZE = x1-x0;  
	VDMA_Y_SIZE = y1-y0;
    VDMA_DST_STRIDE = vickyResX;
	// printf ("x:%d y:%d x1:%d y1:%d address:%ld %ld\n\r",x0,y0,x1,y1,address,ptr); // debug
    // Start VDMA
    VDMA_CONTROL_REG |= VDMA_CTRL_Start_TRF;

    // When started, the 65816 is held, but it takes a few cycles.
    // Provide a short delay to ensure that we do not execute too far
    // before the transfer actually starts. Call a delay subroutine for
    // this.
    DMA_wait_delay();
    wait_for_DMA_to_finish();
}

// ----------------------------------------------
// 4 point Bezier curve 
// Credits to M G Channel
// https://www.youtube.com/watch?v=KiscPiPdLv4
// Adapted by Ernesto Contreras
// ----------------------------------------------
void plot_bezier (int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t col)
{
	float u, pxe, pye,pxi,pyi;
	pxi=0;
	pyi=0;
	for (u=0;u<=1.0;u=u+0.02)
	{
		pxe=fast_pow(1-u,3)*x0+3*u*fast_pow(1-u,2)*x1+3*fast_pow(u,2)*(1-u)*x2+fast_pow(u,3)*x3;
		pye=fast_pow(1-u,3)*y0+3*u*fast_pow(1-u,2)*y1+3*fast_pow(u,2)*(1-u)*y2+fast_pow(u,3)*y3;
		if (pxi!=0 || pyi!=0)
		{
			plot_line (pxi,pyi,pxe,pye,col);
		}
		pxi=pxe;
		pyi=pye;
	}
}	
		
// ---------------------------
// Simple pow function (power), to avoid adding complete Math.h
// Ernesto Contreras
// ---------------------------
float fast_pow(float value,int power)
{
	int c;
	float result;
	result=1;
	for (c=0;c<power;c++)
	{
		result*=value;
	}
	return result;
}

// ------------------------------------------
// Graphics Primitives
// Bresenham Algorithms in C
// Original Author: Bert Timmerman
// https://gist.github.com/bert/1085538
// Modified for Calypsi by Ernesto Contreras 
// ------------------------------------------


//----------------------------------------------------------
// Plot Line 
// Bresenham Algorithm
// Modified by Ernesto Contreras to optimize horizontal line cases (used to fill solid figures & floodfill)
// --------------------------------------------------------
void plot_line (int x0, int y0, int x1, int y1, uint8_t col)
{
long swap;
uint8_t __far24 *ptr;
	if (y0==y1) 
	// ------------------------------------------------
    // Optimizations for horizontal Lines
	// ------------------------------------------------
	{
		if (y0<=vickyResY && y0>=0)
		{
			if (x0>x1)
			{
				swap=x1;
				x1=x0;
				x0=swap;
			}
			
			// validate Coordinates
			if (x1>=vickyResX)
				x1=vickyResX-1;
			if (x0>=vickyResX)
				x0=vickyResX-1;
			if (x1<0)
				x1=0;
			if (x0<0)
				x0=0;
			
			swap=x1-x0;
			// ----------------------------------------------------
			//if line crosses a 64k page use setPixel (safer slower method) 
			// ----------------------------------------------------
			if (((swap+x0+y0*vickyResX) & 0xffff) < ((x0+y0*vickyResX) & 0xffff) )
			{
				do
				{
					setPixel_fast(x0+swap--,y0,col);
				}while (swap>=0);
			}
			else
				//----------------------------------
				// Use fast pixel laying method!
				//-----------------------------------
			{
				if (vBitPlane[vickyBitmapPage]) // if Bitmap for the page is enabled
				{
					ptr = 0xb00000+x0+y0*vickyResX;
					ptr+= (long)bitplane[vickyBitmapPage].start;
					do 
					{
							*(ptr+swap--) = col;
					}while (swap>=0);
				}
			}
		}
	}
	else
	// -------------------------------------
	// use regular Bresenham method for slanted lines
	// -------------------------------------
	{
		 int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
		 int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
		 int err = dx + dy, e2; /* error value e_xy */
		 
		 for (;;)
		 {  /* loop */
			setPixel (x0,y0, col);
			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		 }
	}
}


void plot_circle (int xm, int ym, int r, uint8_t col)
{
   int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */ 
   {
	   do {
			setPixel (xm-x, ym+y, col); /*   I. Quadrant */
			setPixel (xm-y, ym-x, col); /*  II. Quadrant */
			setPixel (xm+x, ym-y, col); /* III. Quadrant */
			setPixel (xm+y, ym+x, col); /*  IV. Quadrant */
		  r = err;
		  if (r >  x) err += ++x*2+1; /* e_xy+e_x > 0 */
		  if (r <= y) err += ++y*2+1; /* e_xy+e_y < 0 */
	   } while (x < 0);
   }
}


// -------------------------------------------------------------------
// Midpoint Ellipse Algorithm C++ Code
// X,y of center point
// a, b Horizontal, vertical axis length
// https://educativesite.com/midpoint-ellipse-algorithm-c-code/
// -------------------------------------------------------------------
void plot_ellipse(int xc, int yc, double a, double b, uint8_t col)
{
    double p=b*b-a*a*b+a*a/4;
    int x=0, y=b;
	
    while(2.0*b*b*x <= 2.0*a*a*y)
    {
        if(p < 0)
        {
            x++;
            p = p+2*b*b*x+b*b;
        }
        else
        {
            x++;
            y--;
            p = p+2*b*b*x-2*a*a*y-b*b;
        }
        setPixel(xc+x,yc+y,col);
        setPixel(xc+x,yc-y,col);
        setPixel(xc-x,yc+y,col);
        setPixel(xc-x,yc-y,col);
    }
 
    p=b*b*(x+0.5)*(x+0.5)+a*a*(y-1)*(y-1)-a*a*b*b;
    while(y > 0)
    {
        if(p <= 0)
        {
            x++;
            y--;
            p = p+2*b*b*x-2*a*a*y+a*a;
        }
        else
        {
            y--;
            p = p-2*a*a*y+a*a;
        }
        setPixel(xc+x,yc+y,col);
        setPixel(xc+x,yc-y,col);
        setPixel(xc-x,yc+y,col);
        setPixel(xc-x,yc-y,col);
    }
	// draw final pixels to close the ellipse - Modification Ernesto Contreras 
	setPixel(xc,(int)(yc+b),col);
	setPixel(xc,(int)(yc-b),col);
}

//----------------------------------------------
// Modified MidPoint ellipse algorith to draw a "Faster" Solid Ellipse
// By Ernesto Contreras
// ---------------------------------------------
void plot_solid_ellipse(int xc, int yc, double a, double b, uint8_t col)
{
	// 1 pixel height or width ellipses are not allowed!!, force at least 2 pixels in either dimension
	if (a==1) {a++;}
	if (b==1) {b++;}
	
    double p=b*b-a*a*b+a*a/4;
    int x=0, y=b;
	int ra,rb;
	
	ra=(int) a*(0.70);
	rb=(int) b*(0.70);
	
	// plot solid square with VDMA inside ellipse to save precious time!
	plot_solid_rectangle(xc-ra,yc-rb,xc+ra,yc+rb,col);
	
    while(2.0*b*b*x <= 2.0*a*a*y)
    {
        if(p < 0)
        {
            x++;
            p = p+2*b*b*x+b*b;
        }
        else
        {
            x++;
            y--;
            p = p+2*b*b*x-2*a*a*y-b*b;
        }
		plot_line(xc-x,yc+y,xc+x,yc+y,col);
		plot_line(xc-x,yc-y,xc+x,yc-y,col);
    }
 
    p=b*b*(x+0.5)*(x+0.5)+a*a*(y-1)*(y-1)-a*a*b*b;
    while(y > 0)
    {
        if(p <= 0)
        {
            x++;
            y--;
            p = p+2*b*b*x-2*a*a*y+a*a;
        }
        else
        {
            y--;
            p = p-2*a*a*y+a*a;
        }
		if (a>b)
		{
			plot_line(xc-ra,yc+y,xc-x,yc+y,col);
			plot_line(xc+ra,yc+y,xc+x,yc+y,col);
			plot_line(xc-ra,yc-y,xc-x,yc-y,col);
			plot_line(xc+ra,yc-y,xc+x,yc-y,col);
		}
		else
		{
			if (yc-y<yc-rb)
			{
				plot_line(xc-x,yc-y,xc+x,yc-y,col);
				plot_line(xc-x,yc+y,xc+x,yc+y,col);
			}
			else
			{
				// special case - if you remove it a line wont be completely filled in vertical ellipses
				if (yc-y==yc-rb) {plot_line(xc-x,yc+y,xc+x,yc+y,col);}
				plot_line(xc-ra,yc+y,xc-x,yc+y,col);
				plot_line(xc+ra,yc+y,xc+x,yc+y,col);
				plot_line(xc-ra,yc-y,xc-x,yc-y,col);
				plot_line(xc+ra,yc-y,xc+x,yc-y,col);
			}
		}
    }
}

//--------------------------------------
// "Faster" Solid Circle
// By Ernesto Contreras 
//--------------------------------------
void plot_solid_circle(int xc,int yc, int r, uint8_t col) 
{
	int x;
	int y;
	int p;
	int r1;
	r1=(int)(r*0.70);
	
	// 1 pixel radius circles are not allowed!!, force 2 radius!
	if (r==1) {r++;}

	// plot solid square with VDMA inside circle to save precious time!
	plot_solid_rectangle (xc-r1,yc-r1,xc+r1,yc+r1,col);
	x = 0;
	y = r;
	p=3-(2*r);

	for(x=0;x<=y;x++)
	{
		if (p < 0)
			{
				p = p + (4 * x)+6;
			}
			else
			{
				y=y-1;
				p = p +4 *(x-y)+10;
			}
		plot_line(xc+x,yc-y,xc-x,yc-y,col);
		plot_line(xc+x,yc+y,xc-x,yc+y,col);
		plot_line(xc-y,yc-x,xc-r1,yc-x,col);
		plot_line(xc+r1,yc-x,xc+y,yc-x,col);
		plot_line(xc-y,yc+x,xc-r1,yc+x,col);
		plot_line(xc+r1,yc+x,xc+y,yc+x,col);
	}
	// special case if you remove it on certain circles a line won't be filled
	plot_line(xc-x,yc+y-1,xc+x,yc+y-1,col);
}

//------------------------------------------------------------------------------
// Scanline floodfill algorithm using stack instead of recursion, more robust
// Modified floodFillScanlineStack(int x, int y, int newColor, int oldColor)
// Credits to Lode Vandevenne.
// https://lodev.org/cgtutor/floodfill.html
// modified by Ernesto Contreras 
// -----------------------------------------------------------------------------
void floodFill(int x, int y, uint8_t newColor)
{
// Global Array for Pixel buffer read
  int st, dbcount;
  uint8_t oldColor;
  int x1;
  int xInit;
  
  // Use of static to ask for this buffer memory only Once!
  uint8_t BufferAbove[1024];
  uint8_t BufferCurrent[1024];
  uint8_t BufferBelow[1024];

  // init variables
  x1=0;
  xInit=0;
  dbcount=0;
  stackTop=0;
  st=0;
 
 // Init Floodfill Algorithm
  st=getScanLine(y,BufferCurrent);  // read "current" scan line y into bufferCurrent[]
  oldColor=BufferCurrent[x];
  if(oldColor == newColor) return;
  // Global Buffers for FloodFill
  bool spanAbove, spanBelow;
  stackPush(x, y);
  while(stackPop(&x, &y))
  {
	if (y>=1)
		st=getScanLine(y-1,BufferAbove);  // read scan line "above" line y into bufferAbove[]
	if (y>=0)
		st=getScanLine(y,BufferCurrent);  // read "current" scan line y into bufferCurrent[]
	if (y < vickyResY-1)
		st=getScanLine(y+1,BufferBelow);  // read scan line "below" line y into bufferBelow[]

	x1 = x;
	while((x1 >= 0) && BufferCurrent[x1] == oldColor) 
		x1--; //find leftmost pixel with color != oldcolor to begin fill
	xInit=++x1;
	spanAbove = spanBelow = 0;
	while(x1 < (vickyResX) && BufferCurrent[x1] == oldColor)
	{
	  if(!spanAbove && (y > 0) && BufferAbove[x1] == oldColor)
	  {
		stackPush(x1, y - 1);
		spanAbove = 1;
	  }
	  else if(spanAbove && (y > 0) && BufferAbove[x1] != oldColor)
	  {
		spanAbove = 0;
	  }
	  if(!spanBelow && (y < (vickyResY - 1)) && BufferBelow[x1]== oldColor)
	  {
		stackPush(x1, y + 1);
		spanBelow = 1;
	  }
	  else if(spanBelow && (y < (vickyResY - 1)) && BufferBelow[x1]!= oldColor)
	  {
		spanBelow = 0;
	  }
	  x1++;
	}
	// Fill horizontal Line (optimization)
	if (BufferCurrent[x1]!=oldColor)
		x1--;
	plot_line(xInit,y,x1,y,newColor);
  }
}


// ----------------------------------------------------------------------------------
// Arc rendering algorithm - EXPERIMENTAL FOENIX PORT!!!
// credits to Harsh Jain
// https://codingee.com/computer-graphics-program-to-implement-circular-arc-algorithm/
// ----------------------------------------------------------------------------------

void plot_arc( int x_cen, int y_cen, int rad, int start_ang, int end_ang, uint8_t col)
{
       float ang=(((start_ang<=end_ang)?start_ang:end_ang)*(PI/180));
       float range=(((end_ang>start_ang)?end_ang:start_ang)*(PI/180));
       float x=(rad*cos(ang));
       float y=(rad*sin(ang));
	   printf ("ang: %f range: %f\r",ang, range);
       do
       {
		   printf ("ang: %f x: %d y: %d",ang,(int)(x_cen+x+0.5), (int)(y_cen-y+0.5));
            setPixel((int)(x_cen+x+0.5),(int)(y_cen-y+0.5),col);
            ang+=0.01;
            x=(rad*cos(ang));
            y=(rad*sin(ang));
       }
       while(ang<=range);
}
//--------------------------------------------------------------------------------------------
// SUPPORTING FUNCTIONS
// -------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
// Custom Stack Functions (needed for Flood fill Implementation)
// Original Version: https://www.tutorialspoint.com/data_structures_algorithms/stack_program_in_c.htm
// Modified by Ernesto Contreras 
// ----------------------------------------------------------------------------------

bool isStackEmpty() {
   if(stackTop == 0)
      return true;
   else
      return false;
}
   
bool isStackFull() {
   if(stackTop == STACK_MAXSIZE )
   {
	   //printf ("stacklevel: %d MaxSize: %d\n\r",stackTop,STACK_MAXSIZE);
       return true;
   }
   else
   {
      return false;
   }
}

bool stackPeek(int *x, int *y) {
   *x=stack[stackTop][0];
   *y=stack[stackTop][1];
   return true;
}

bool stackPop(int *x, int *y) {
   int data;
   if(isStackEmpty()==false) {
	  *x = stack[stackTop][0];
	  *y = stack[stackTop][1];
      stackTop--;   
      return true;
   } else {
      //printf("Could not insert data, Stack is full. stackSize:%d \n\r",stackTop);
	  return false;
   }
}

bool stackPush(int x, int y) {

   if(isStackFull()==false) {
      stackTop++;   
	  stack[stackTop][0] = x;
	  stack[stackTop][1] = y;
	  return true;
   } else {
      //printf("Could not insert data, Stack is full. stackSize:%d \n\r",stackTop);
	  return false;
   }
}

