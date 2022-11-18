#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>

#include "pi_gui.h"
#include "tslib.h"
#include "math.h"



char digit_display[16];
int digit_colour[16];

struct surface btn[100];
struct surface slider[10];
//---

void set_active_digit(int d_act)
{

}


void set_slider_value(int index,int value)
{
int thick;
fill_surface(&slider[index],rgb565(0x2,0x02,0x00));
plot_rectangle(&slider[index],2,2,SLDR_WIDTH-1,SLDR_HEIGHT-2,BLUE);

for(thick = 0;thick <11;thick++)
   {
    plot_line(&slider[index],2,4+(thick*3),value,4+(thick*3),RED);
    }
    copy_surface_to_image(&slider[index],50,600+(index*100)); //FIXME
}



void plot_freq_digits()
{
int dg,dd;
dd=0;

uint8_t string[50] = "    MHz     KHz      Hz";

plot_large_string(&freq,10, 4,string ,C_DIM_GRAY);

for(dg=11;dg>8;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],digit_colour[dg]);
    }
plot_huge_numeral(&freq,dg*32,40,0x3a,WHITE);//8
dg--;

for(;dg>4;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],digit_colour[dg]);
    }

plot_huge_numeral(&freq,dg*32,40,0x3a,WHITE);
dg--;

for(;dg>0;dg--,dd++)
    {
    plot_huge_numeral(&freq,dg*32,40,digit_display[dd],digit_colour[dg]);
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


//Make two columns of btns
//first 10
for(int bn=0; bn<10;bn++)
    {
    btn[bn].data = malloc(BTN_WIDTH * BTN_HEIGHT *2);
    btn[bn].sz_x = BTN_WIDTH;
    btn[bn].sz_y = BTN_HEIGHT;
    }

for(int bn=0;bn<10;bn++)
    fill_surface(&btn[bn],rgb565(0x04,15,0x06));

bb=0;
for(int bn=0;bn<10;bn++,bb++)
    {
    copy_surface_to_image(&btn[bn],BTN_POS_X,BTN_POS_Y+ (bb *BTN_Y_SPACE));
    }

//Second 10 to right
for(int bn=12; bn<22;bn++)
    {
    btn[bn].data = malloc(BTN_WIDTH * BTN_HEIGHT *2);
    btn[bn].sz_x = BTN_WIDTH;
    btn[bn].sz_y = BTN_HEIGHT;
    }

for(int bn=12;bn<22;bn++)
    fill_surface(&btn[bn],rgb565(0x04,0x15,0x03));

bb=0;
for(int bn=12;bn<22;bn++,bb++)
    {
    copy_surface_to_image(&btn[bn],BTN_POS_X+BTN_X_SPACE,BTN_POS_Y+(bb *BTN_Y_SPACE));
    }

//change colour of one btn as test
bn=4;bb=4; 
fill_surface(&btn[bn],rgb565(0x10,0x00,0x10));
copy_surface_to_image(&btn[bn],BTN_POS_X+100,BTN_POS_Y+ (bb *75));

//---

//Sliders -->
for(int sld=0; sld<4;sld++)
    {
    slider[sld].data = malloc(SLDR_HEIGHT * SLDR_WIDTH *2);
    slider[sld].sz_x = SLDR_WIDTH;
    slider[sld].sz_y = SLDR_HEIGHT;
    }

for(int bn=0;bn<4;bn++)
    fill_surface(&slider[bn],rgb565(0x2,0x02,0x00));

plot_rectangle(&slider[0],2,2,SLDR_WIDTH-1,SLDR_HEIGHT-2,BLUE);
copy_surface_to_image(&slider[0],SLDR_POS_X,SLDR_POS_Y);

plot_rectangle(&slider[1],2,2,SLDR_WIDTH-1,SLDR_HEIGHT-2,BLUE);
copy_surface_to_image(&slider[0],SLDR_POS_X,SLDR_POS_Y+SLDR_Y_SPACE);



//---


//Frequency display

//freq.data = malloc(SLDR_HEIGHT * SLDR_WIDTH *2);
//freq.sz_x = FREQ_WIDTH;
//freq.sz_y = FREQ_HEIGHT;

fill_surface(&freq,rgb565(0x4,0x4,0x08)); 
copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);

for(int c = 0;c<16; c++)
    digit_colour[c] = WHITE;

digit_display[0] = '1';
digit_display[1] = '1';
digit_display[2] = '1';
digit_display[3] = '1';
digit_display[4] = '1';
digit_display[5] = '1';
digit_display[6] = '1';
digit_display[7] = '1';
digit_display[8] = '1';

plot_freq_digits();



refresh_screen();


if(0)
{
for(int x=0;x<4;x++)
    {
set_slider_value(0,120);
set_slider_value(1,17);
refresh_screen();
sleep(1);

set_slider_value(0,40);
set_slider_value(1,50);
refresh_screen();
sleep(1);

set_slider_value(0,180);
set_slider_value(1,15);



refresh_screen();

sleep(1);
    }
}



} //make layout





//================ END OF ++++++++++
//LOOSE ALL BELOW - Here only as spares


//plot_large_string(&specanzanz,10,150,"Spectrum",WHITE);


/*
//Make btns -----------------------------------
for(int bn=0;bn<16;bn++)
    {
    btn[bn].data = malloc(BTN_WIDTH * BTN_HEIGHT *2);
    btn[bn].sz_x = BTN_WIDTH;
    btn[bn].sz_y = BTN_HEIGHT;
    }

plot_large_string(&btn[0],10,20,"ALPHA",WHITE);
plot_large_string(&btn[1],10,20,"BRAVO",WHITE);
plot_large_string(&btn[2],10,15,"CHARLY",WHITE);
plot_large_string(&btn[3],10,30,"DELTA",WHITE);

for(int bn=0;bn<4;bn++)
    {
  copy_surface_to_image(&btn[bn],BTN_POS_X+1160,BTN_POS_Y+(bn*BTN_WIDTH)+20*bn);
    plot_rectangle(&btn[bn],BTN_POS_X,BTN_POS_Y,BTN_WIDTH,BTN_HEIGHT,YELLOW);
    }
 
for(int bn=0;bn<6;bn++)
    fill_surface(&btn[bn],rgb565(0x06,0x0,0x06));
//end btns -----------------------------------    

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
//Make btns -----------------------------------
for(int bn=0;bn<16;bn++)
    {
    btn[bn].data = malloc(BTN_WIDTH * BTN_HEIGHT *2);
    btn[bn].sz_x = BTN_WIDTH;
    btn[bn].sz_y = BTN_HEIGHT;
    }

plot_large_string(&btn[0],10,20,"AM",WHITE);
plot_large_string(&btn[1],10,20,"USB",WHITE);
plot_large_string(&btn[2],10,15,"LSB",WHITE);
plot_large_string(&btn[3],10,30,"DSB",WHITE);

for(int bn=0;bn<4;bn++)
    {
  copy_surface_to_image(&btn[bn],BTN_POS_X+1160,BTN_POS_Y+(bn*BTN_WIDTH)+20*bn);
    plot_rectangle(&btn[bn],BTN_POS_X,BTN_POS_Y,BTN_WIDTH,BTN_HEIGHT,YELLOW);
    }
 
for(int bn=0;bn<6;bn++)
    fill_surface(&btn[bn],rgb565(0x06,0x0,0x06));
//end btns -----------------------------------    
*/


/*
plot_freq_digits();

copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);

*/
