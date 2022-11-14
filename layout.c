#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include "pi_gui.h"
#include "tslib.h"
#include "math.h"
#include <locale.h>


#define BUTTON_WIDTH 70
#define BUTTON_HEIGHT 50
#define BUTTON_POS_X 1080
#define BUTTON_POS_Y 10

char digit_display[10];
struct surface button[100];
struct surface slider[10];
//---

void set_active_digit(int d_act)
{

}

void plot_freq_digits()
{
int dg,dd;
dd=0;

char string[50] = "    MHz     KHz      Hz";

plot_large_string(&freq,10, 4, & string ,C_DIM_GRAY);

for(dg=11;dg>8;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],WHITE);
    }
plot_huge_numeral(&freq,dg*32,40,0x3a,WHITE);//8
dg--;

for(;dg>4;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],WHITE);
    }

plot_huge_numeral(&freq,dg*32,40,0x3a,WHITE);
dg--;

for(;dg>0;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],WHITE);
    }
//dg--;
//plot_huge_numeral(&freq,dg*32,40,0x3a,WHITE);

copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);
}
//---
/*
void set_freq_disp(int d_freq)
{
for(int n = 0; n<10;n++)
    {
    digit_display[n]=(d_freq%10) +0x30;
    d_freq/=10;
    }
plot_freq_digits();
}
*/
//---

void make_layout()
{
int bn;
int bb;

//allocate image array
pixbuf.data = malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 2);
pixbuf.sz_x = IMAGE_WIDTH;
pixbuf.sz_y = IMAGE_HEIGHT;

specanz.data = malloc(SPEC_WIDTH * SPEC_HEIGHT * 2);
specanz.sz_x = SPEC_WIDTH;
specanz.sz_y = SPEC_HEIGHT;

wfall.data = malloc(SPEC_WIDTH * SPEC_HEIGHT * 2);
wfall.sz_x = SPEC_WIDTH;
wfall.sz_y = SPEC_HEIGHT;

top_line.data = malloc(SPEC_WIDTH * SPEC_HEIGHT * 2);
top_line.sz_x = SPEC_WIDTH;
top_line.sz_y = 1;

finx.data = malloc(FINX_WIDTH * FINX_HEIGHT * 2);
finx.sz_x = FINX_WIDTH;
finx.sz_y = FINX_HEIGHT;

wfall.data = malloc(WFALL_WIDTH * WFALL_HEIGHT * 2);
wfall.sz_x = WFALL_WIDTH;
wfall.sz_y = WFALL_HEIGHT;

freq.data = malloc(FREQ_WIDTH * FREQ_HEIGHT * 2);
freq.sz_x = FREQ_WIDTH;
freq.sz_y = FREQ_HEIGHT;

meter.data = malloc(METER_WIDTH * METER_HEIGHT * 2);
meter.sz_x = METER_WIDTH;
meter.sz_y = METER_HEIGHT;


demod.data = malloc(DEMOD_WIDTH * DEMOD_HEIGHT * 2);
demod.sz_x=DEMOD_WIDTH;
demod.sz_y = DEMOD_HEIGHT;

samp_rate.data = malloc(SAMP_RATE_WIDTH * SAMP_RATE_HEIGHT * 2);
samp_rate.sz_x=SAMP_RATE_WIDTH;
samp_rate.sz_y = SAMP_RATE_HEIGHT;

active.data = malloc(ACTIVE_WIDTH * ACTIVE_HEIGHT * 2);
active.sz_x=ACTIVE_WIDTH;
active.sz_y = ACTIVE_HEIGHT;

af_gain.data = malloc(AF_GAIN_WIDTH * AF_GAIN_HEIGHT * 2);
af_gain.sz_x=AF_GAIN_WIDTH;
af_gain.sz_y = AF_GAIN_HEIGHT;

rf_gain.data = malloc(RF_GAIN_WIDTH * RF_GAIN_HEIGHT * 2);
rf_gain.sz_x=RF_GAIN_WIDTH;
rf_gain.sz_y = RF_GAIN_HEIGHT;


//Make two columns of buttons
//first 10
for(int bn=0; bn<10;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

for(int bn=0;bn<10;bn++)
    fill_surface(&button[bn],rgb565(0x04,15,0x06));

bb=0;
for(int bn=0;bn<10;bn++,bb++)
    {
    copy_surface_to_image(&button[bn],BUTTON_POS_X,BUTTON_POS_Y+ (bb *75));
    }

//Second 10 to right
for(int bn=12; bn<22;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

for(int bn=12;bn<22;bn++)
    fill_surface(&button[bn],rgb565(0x04,0x15,0x03));

bb=0;
for(int bn=12;bn<22;bn++,bb++)
    {
    copy_surface_to_image(&button[bn],BUTTON_POS_X+100,BUTTON_POS_Y+ (bb *75));
    }

//change colour of one button
bn=4;bb=4; 
fill_surface(&button[bn],rgb565(0x10,0x00,0x10));
copy_surface_to_image(&button[bn],BUTTON_POS_X+100,BUTTON_POS_Y+ (bb *75));

//---


//Sliders -->
for(int sld=0; sld<4;sld++)
    {
    slider[sld].data = malloc(H_SLIDER_HEIGHT * H_SLIDER_WIDTH *2);
    slider[sld].sz_x = H_SLIDER_WIDTH;
    slider[sld].sz_y = H_SLIDER_HEIGHT;
    }

//for(int sld=4; sld<8;sld++)
//    {
//    slider[sld].data = malloc(V_SLIDER_HEIGHT * V_SLIDER_WIDTH *2);
//    slider[sld].sz_x = V_SLIDER_WIDTH;
//    slider[sld].sz_y = V_SLIDER_HEIGHT;
//    }

for(int bn=0;bn<4;bn++)
    fill_surface(&slider[bn],rgb565(0x15,0x15,0x00));


copy_surface_to_image(&slider[0],50,600);
copy_surface_to_image(&slider[1],50,700);


//---


//Frequency display

//freq.data = malloc(H_SLIDER_HEIGHT * H_SLIDER_WIDTH *2);
//freq.sz_x = FREQ_WIDTH;
//freq.sz_y = FREQ_HEIGHT;

fill_surface(&freq,rgb565(0x4,0x4,0x08)); 
copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);



digit_display[0] = '0';
digit_display[1] = '0';
digit_display[2] = '0';
digit_display[3] = '0';
digit_display[4] = '5';
digit_display[5] = '4';
digit_display[6] = '5';
digit_display[7] = '0';
digit_display[8] = '0';

plot_freq_digits();



refresh_screen();

} //make layout




//================ END OF ++++++++++
//LOOSE ALL BELOW - Here only as spares


//plot_large_string(&specanzanz,10,150,"Spectrum",WHITE);


/*
//Make buttons -----------------------------------
for(int bn=0;bn<16;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

plot_large_string(&button[0],10,20,"ALPHA",WHITE);
plot_large_string(&button[1],10,20,"BRAVO",WHITE);
plot_large_string(&button[2],10,15,"CHARLY",WHITE);
plot_large_string(&button[3],10,30,"DELTA",WHITE);

for(int bn=0;bn<4;bn++)
    {
  copy_surface_to_image(&button[bn],BUTTON_POS_X+1160,BUTTON_POS_Y+(bn*BUTTON_WIDTH)+20*bn);
    plot_rectangle(&button[bn],BUTTON_POS_X,BUTTON_POS_Y,BUTTON_WIDTH,BUTTON_HEIGHT,YELLOW);
    }
 
for(int bn=0;bn<6;bn++)
    fill_surface(&button[bn],rgb565(0x06,0x0,0x06));
//end buttons -----------------------------------    

*/

//draw various shapes to test
//plot_rectangle(&pixbuf,630,400,10,10,WHITE);

/*
plot_large_string(&panel,50,50,"PANEL",WHITE);

plot_circle(&panel,100,100,20,RED);
plot_circle(&panel,150,100,30,GREEN);
plot_circle(&panel,200,100,40,BLUE);
plot_circle(&panel,250,100,50,CYAN);
plot_rectangle(&panel,300,100,20,20,WHITE);

plot_huge_numeral(&panel,350,150,'3',WHITE);
plot_huge_numeral(&panel,400,150,'4',WHITE);
plot_huge_numeral(&panel,450,150,'5',WHITE);

plot_line(&panel,300,101,500,101,GREEN);
plot_thick_line(&panel,300,97,500,97,GREEN);
plot_thick_rectangle(&panel,400,100,60,60,9,GREEN);
*/

/*
//copy panel to pixbuf
loc_x = PANEL_POS_X; 
loc_y = 0; 
copy_surface_to_image(&panel,loc_x,loc_y);
*/





/*
plot_huge_numeral(&freq,32,40,0x36,WHITE);
plot_huge_numeral(&freq,64,40,'4',WHITE);
plot_huge_numeral(&freq,96,40,'5',WHITE);
plot_huge_numeral(&freq,128,40,':',WHITE);
plot_huge_numeral(&freq,160,40,'6',WHITE);
plot_huge_numeral(&freq,192,40,'5',WHITE);
plot_huge_numeral(&freq,224,40,'0',WHITE);
plot_huge_numeral(&freq,256,40,':',WHITE);
plot_huge_numeral(&freq,288,40,'0',WHITE);
plot_huge_numeral(&freq,320,40,'0',WHITE);
plot_huge_numeral(&freq,352,40,'0',WHITE);
*/
/*
//Make buttons -----------------------------------
for(int bn=0;bn<16;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

plot_large_string(&button[0],10,20,"AM",WHITE);
plot_large_string(&button[1],10,20,"USB",WHITE);
plot_large_string(&button[2],10,15,"LSB",WHITE);
plot_large_string(&button[3],10,30,"DSB",WHITE);

for(int bn=0;bn<4;bn++)
    {
  copy_surface_to_image(&button[bn],BUTTON_POS_X+1160,BUTTON_POS_Y+(bn*BUTTON_WIDTH)+20*bn);
    plot_rectangle(&button[bn],BUTTON_POS_X,BUTTON_POS_Y,BUTTON_WIDTH,BUTTON_HEIGHT,YELLOW);
    }
 
for(int bn=0;bn<6;bn++)
    fill_surface(&button[bn],rgb565(0x06,0x0,0x06));
//end buttons -----------------------------------    
*/


/*
plot_freq_digits();

copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);

*/
