#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include "pi_gui.h"
#include "tslib.h"
#include <locale.h>







void make_layout()
{

//allocate image array
pixbuf.data = malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 2);
pixbuf.sz_x = IMAGE_WIDTH;
pixbuf.sz_y = IMAGE_HEIGHT;

spec.data = malloc(SPEC_WIDTH * SPEC_HEIGHT * 2);
spec.sz_x = SPEC_WIDTH;
spec.sz_y = SPEC_HEIGHT;

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

af_gain.data = malloc(AF_GAIN_WIDTH * AF_GAIN_HEIGHT * 2);
af_gain.sz_x=AF_GAIN_WIDTH;
af_gain.sz_y = AF_GAIN_HEIGHT;

rf_gain.data = malloc(RF_GAIN_WIDTH * RF_GAIN_HEIGHT * 2);
rf_gain.sz_x=RF_GAIN_WIDTH;
rf_gain.sz_y = RF_GAIN_HEIGHT;

//fill image backround
for(int i = 0 ;i<screensize;i++) 
	pixbuf.data[i] = rgb565(0x03,0x07,0x03); 

//fill spec backround
fill_surface(&spec,rgb565(0x07,0x00,0x00)); 
//fill finx backround
fill_surface(&finx,rgb565(0x0f,0x03,0x0f)); 
//fill wfall backround
fill_surface(&wfall,rgb565(0x00,0x0f,0x0f)); 
//fill freq backround
fill_surface(&freq,rgb565(0x07,0x00,0x00)); 

//fill meter backround
fill_surface(&meter,rgb565(0x00,0x00,0x07)); 

plot_huge_numeral(&freq,32,40,'1',WHITE);
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



copy_surface_to_image(&spec,SPEC_POS_X,SPEC_POS_Y);
copy_surface_to_image(&finx,FINX_POS_X,FINX_POS_Y);
copy_surface_to_image(&wfall,WFALL_POS_X,WFALL_POS_Y);
copy_surface_to_image(&meter,METER_POS_X,METER_POS_Y);
copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);

//copy_surface_to_image(&demod,DEMOD_POS_X,DEMOD_POS_Y);
//copy_surface_to_image(&samp_rate,SAMP_RATE_POS_X,SAMP_RATE_POS_Y);
copy_surface_to_image(&rf_gain,RF_GAIN_POS_X,RF_GAIN_POS_Y);
copy_surface_to_image(&af_gain,AF_GAIN_POS_X,AF_GAIN_POS_Y);


//Make DEMOD buttons
for(int bn=0;bn<4;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

for(int bn=0;bn<4;bn++)
    fill_surface(&button[bn],rgb565(0x06,15,0x06));

plot_large_string(&button[0],10,20,"DSB",WHITE);
plot_large_string(&button[1],10,20,"USB",WHITE);
plot_large_string(&button[2],10,20,"LSB",WHITE);
plot_large_string(&button[3],10,20,"NFM",WHITE);

for(int bn=0;bn<4;bn++)
    {
  copy_surface_to_image(&button[bn],BUTTON_POS_X+1150,BUTTON_POS_Y+ 500+(bn*BUTTON_WIDTH)+1*bn);
    plot_rectangle(&button[bn],BUTTON_POS_X,BUTTON_POS_Y,BUTTON_WIDTH,BUTTON_HEIGHT,YELLOW);
    }

//Make SAMPLE buttons

for(int bn=4; bn<8;bn++)
    {
    button[bn].data = malloc(BUTTON_WIDTH * BUTTON_HEIGHT *2);
    button[bn].sz_x = BUTTON_WIDTH;
    button[bn].sz_y = BUTTON_HEIGHT;
    }

for(int bn=4;bn<8;bn++)
    fill_surface(&button[bn],rgb565(0x04,15,0x06));

plot_large_string(&button[4],10,20,"SR1",WHITE);
plot_large_string(&button[5],10,20,"SR2",WHITE);
plot_large_string(&button[6],10,20,"SR3",WHITE);
plot_large_string(&button[7],10,20,"SR4",WHITE);

int bb=0;
for(int bn=4;bn<8;bn++,bb++)
    {
  copy_surface_to_image(&button[bn],BUTTON_POS_X+1150,BUTTON_POS_Y+ 20+(bb*BUTTON_WIDTH)+1*bb);
  plot_rectangle(&button[bn],BUTTON_POS_X,BUTTON_POS_Y,BUTTON_WIDTH,BUTTON_HEIGHT,YELLOW);
    }











//plot_large_string(&spec,10,150,"Spectrum",WHITE);





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

refresh_screen();

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

refresh_screen();


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



}
