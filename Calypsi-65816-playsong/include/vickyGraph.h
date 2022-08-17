// ----------------------------------------------------------
// vickyGraph.h Library
// By Ernesto Contreras
// ----------------------------------------------------------

// Foenix Model Defines
#define FOENIX_MODEL        (*(volatile __far uint8_t *)0xafe887)
#define Model_FMX			0x0   
#define Model_U256			0x1 
#define Model_FMX_C2560		0x3 
#define Model_FMX_C5A		0x4   
#define Model_U256Plus  	0x5 
#define Model_FMX_C2560K	0x7

// -----------------------------------------------
// read_vram_buffer Defines
// -----------------------------------------------

#define VMEM2CPU_CTRL_REG (*(int16_t __far*)0xAF0902)
#define VMEM2CPU_Clear_FIFO 0x01 
	// Setting this to 1 will flush (Reset) the FIFO, clear it after to return to normal function                                
	// Prolly a good idea to reset this before starting to fetch data from VMEM in your application 
#define __fifo_count (*(int16_t __far*)0xAF0902)
#define VMEM2CPU_Fifo_Empty  0x8000
#define __byte_port (*(uint8_t __far*)0xAF0901)

// ----------------------
// Memory Address to set Text Rows & Colums
// ----------------------
#define TEXT_COLS_VISIBLE (*(volatile __far uint16_t *)0x11)
#define TEXT_ROWS_VISIBLE (*(volatile __far uint16_t *)0x15)

//-------------------------------------------------------
// Border Control Defines
//-------------------------------------------------------
#define Border_Ctrl_Enable       0x01
#define BORDER_CTRL_REG          (*(volatile __far uint8_t *)0xaf0004)  // bit[0] - enable (1 by default)
                                                                        // bit[4..6]: X scroll offset
                                                                        // ( will scroll left) (acceptable value: 0..7)
#define BORDER_COLOR_B           (*(volatile __far uint8_t *)0xaf0005)
#define BORDER_COLOR_G           (*(volatile __far uint8_t *)0xaf0006)
#define BORDER_COLOR_R           (*(volatile __far uint8_t *)0xaf0007)
#define BORDER_X_SIZE            (*(volatile __far uint8_t *)0xaf0008)  // X-  values: 0 - 32 (default: 32)
#define BORDER_Y_SIZE            (*(volatile __far uint8_t *)0xaf0009)  // Y- values 0 -32 (default: 32)

// LUT Palette Defines
#define GRPH_LUT0_LONG           ((volatile __far uint8_t *)0xaf2000)
#define GRPH_LUT1_LONG           ((volatile __far uint8_t *)0xaf2400)
#define GRPH_LUT2_LONG           ((volatile __far uint8_t *)0xaf2800)
#define GRPH_LUT3_LONG           ((volatile __far uint8_t *)0xaf2c00)
#define GRPH_LUT4_LONG           ((volatile __far uint8_t *)0xaf3000)
#define GRPH_LUT5_LONG           ((volatile __far uint8_t *)0xaf3400)
#define GRPH_LUT6_LONG           ((volatile __far uint8_t *)0xaf3800)
#define GRPH_LUT7_LONG           ((volatile __far uint8_t *)0xaf3c00)


// ----------------------
// Minimal ANSI APIs
// ----------------------
void ANSIClrScr();  
void ANSIScrSize (int x, int y); // I'm not certain this works at all
void ANSILocate(int row,int col);

// ----------------------
// Vicky II APIs
// ----------------------
int vickyVideomode (bool text,bool txtOverlay,bool bitmap,bool tiles,bool sprites,bool gammaFix,bool videoOff,int resolution);
int vickyBitmap(int page, bool enable, long address, int lut, bool collision);
int vickyPaletteCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
int vickyBackCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
int vickyBorderCol (uint8_t palette, uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
void vickyDefPalette ();

// ----------------------
// Graphics Primitives
// ----------------------

void clrBitmap();
void plot (unsigned long x, unsigned long y, uint8_t col);
void plot_line (int x0, int y0, int x1, int y1, uint8_t col);
void plot_bezier (int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t col);
void plot_rectangle (int x0, int y0, int x1, int y1, uint8_t col);
void plot_circle (int xm, int ym, int r, uint8_t col);
void plot_ellipse(int xc, int yc, double a, double b, uint8_t col);
void plot_solid_rectangle  (int x0, int y0, int x1, int y1, uint8_t col);
void plot_solid_circle(int xc,int yc, int r, uint8_t col);
void plot_solid_ellipse(int xc, int yc, double a, double b, uint8_t col);
void plot_arc( int x_cen, int y_cen, int rad, int start_ang, int end_ang, uint8_t col);
void floodFill(int x, int y, uint8_t newColor);

// Reading Graphic screen 
int  read_vram_buffer (uint8_t volatile __far *p, int length, uint8_t __far * buffer);
int  getPixel (int x, int y);
int  getScanLine (int y, uint8_t __far * buffer);

// Experimental!!!
void vram2dram_linearCopy ( long source, int __far *buffer, long length); // Experimental (more like non working!), the universe might colapse unto itself if you invoke this!!

// ---------------------------
// Stack support Functions for FloodFill
// ---------------------------
// stack functions used for Flood Fill
bool isStackEmpty(); 
bool isStackFull();
bool stackPeek(int *x, int *y);
bool stackPop(int *x, int *y);
bool stackPush(int x, int y);

// Math functions (used for spline)
float fast_pow(float value,int power);



