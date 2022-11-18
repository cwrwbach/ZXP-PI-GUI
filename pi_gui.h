#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <time.h>
#include <inttypes.h>
#include "fb2-colours.h"


#define PPDIV 10


#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 800



//#define PANEL_WIDTH 1032
//#define PANEL_HEIGHT 768
//#define PANEL_POS_X 128
//#define PANEL_POS_Y 0

#define SPEC_WIDTH 1032
#define SPEC_HEIGHT 256
#define SPEC_POS_X 10
#define SPEC_POS_Y 10

#define FINX_WIDTH 1032
#define FINX_HEIGHT 50
#define FINX_POS_X 10
#define FINX_POS_Y 210


#define WFALL_WIDTH 1032
#define WFALL_HEIGHT 256
#define WFALL_POS_X 10
#define WFALL_POS_Y 260


#define METER_WIDTH 400
#define METER_HEIGHT 100
#define METER_POS_X 80
#define METER_POS_Y 500

#define FREQ_WIDTH 400
#define FREQ_HEIGHT 100
#define FREQ_POS_X 600
#define FREQ_POS_Y 600

#define AF_GAIN_WIDTH 400
#define AF_GAIN_HEIGHT 50
#define AF_GAIN_POS_X 100
#define AF_GAIN_POS_Y 700

#define RF_GAIN_WIDTH 400
#define RF_GAIN_HEIGHT 50
#define RF_GAIN_POS_X 570
#define RF_GAIN_POS_Y 700

#define SAMP_RATE_WIDTH 200
#define SAMP_RATE_HEIGHT 300
#define SAMP_RATE_POS_X 860
#define SAMP_RATE_POS_Y 50

#define DEMOD_WIDTH 200
#define DEMOD_HEIGHT 300
#define DEMOD_POS_X 760
#define DEMOD_POS_Y 400



#define SLDR_WIDTH 200
#define SLDR_HEIGHT 40


#define ACTIVE_WIDTH 32
#define ACTIVE_HEIGHT 140









#define mS 1000





typedef struct surface  * surf_ptr;

int fbfd; // framebuffer file descriptor
short * frame_buf;
struct fb_fix_screeninfo finfo;
long int screensize ;
struct timespec start;
struct timespec end;
struct timespec dif;
float usec;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;


void UiDriverCreateSMeter(void);



struct surface
{
uint16_t * data;
int sz_x;
int sz_y;
int pos_x;
int pos_y;
uint16_t backround_col;
uint16_t border_col;
uint16_t text_col;
char text[40];
}pixbuf,freq,specanz,finx,wfall,meter,demod,samp_rate,af_gain,rf_gain,active,top_line;

void create_surface(struct surface *);
void clear_screen(uint16_t);

void make_layout();
void set_image_pix(int,int,uint16_t);
void xor_image_pix(int,int,uint16_t);
uint16_t get_image_pix(int,int);

short rgb565(short red,short green,short blue);
void plot_circle (struct surface *dest, int xm, int ym, int r,uint16_t colour);
void plot_line (struct surface *, int , int , int , int ,uint16_t );
void plot_small_character(struct surface * , int, int ,uint8_t ,uint16_t );
void plot_large_character(struct surface * , int , int ,uint8_t,uint16_t );
void plot_large_string(struct surface * , int , int ,uint8_t * ,uint16_t );

void plot_thick_line (struct surface *, int, int , int , int,uint16_t );
void plot_thick_rectangle(struct surface * , int, int,int , int ,int , uint16_t );
void plot_huge_numeral(struct surface * , int, int ,uint8_t ,uint16_t);

void plot_rectangle(struct surface * , int, int, int , int , uint16_t );
void copy_surface_to_image(struct surface *,int,int);
void refresh_screen();
void fill_surface(struct surface *,uint16_t);
void plot_button(struct surface *,int,int,int,int,uint16_t,uint16_t,uint16_t,char *);

int open_shuttle();

void show_time();

void draw_waterfall();



