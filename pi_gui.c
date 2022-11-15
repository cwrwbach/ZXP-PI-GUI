#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "pi_gui.h"
#include "tslib.h"
#include "fb2-lib.h"
#include "network.c"
#include "waterfall.h"
#include "keypad.h"

#define SAMPLE_AMOUNT 2

int xres,yres,x;
int Xsamples[20];
int Ysamples[20];
int screenXmax, screenXmin;
int screenYmax, screenYmin;
float scaleXvalue, scaleYvalue;
int rawX, rawY, rawPressure, scaledX, scaledY;
int Xaverage ;
int Yaverage ;
int h;
int sample;
int fd;

int fds; //shuttle
int fdk; //keypad
int fdt; //Touch
int n, shuttle;
char dev_name[256];
typedef struct input_event EV;

char waterfall_buffer[1024][3];
extern char digit_display[10];
//===

extern char fft_video_buf[FFT_SIZE];

EV ev;
void getTouchScreenDetails(int *,int *,int *,int *);
void getTouchSample(int *, int *, int *);

static void sig(int sig)
{
//	close_framebuffer();
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);
	exit(1);
}
//---

int openTouchScreen()
{
strcpy(dev_name,"/dev/input/by-id/usb-eGalax_Inc._USB_TouchController-event-if00");
fds = open(dev_name, O_RDONLY| O_NONBLOCK);
        if ((fd = open(dev_name, O_RDONLY)) < 0) 

        {
            printf("Touchscrren failed\n");        
                return 1;
        }

//FIXME
getTouchScreenDetails(&screenXmin,&screenXmax,&screenYmin,&screenYmax);
xres = 1280 ;//screenXmax - screenXmin;
yres = 800; //screenYmax; // - screenYmin;
	
scaleXvalue = ((float)screenXmax-screenXmin) / xres;
printf ("X Scale Factor = %f   %d %d %d \n", scaleXvalue,screenXmax,screenXmin,xres);
	
scaleYvalue = ((float)screenYmax-screenYmin) / yres;
printf ("Y Scale Factor = %f\n", scaleYvalue);
}

int open_shuttle()
{
int xx;
printf(" Hello shuttle open \n");

//open the shuttleXpress device
strcpy(dev_name,"/dev/input/by-id/usb-Contour_Design_ShuttleXpress-event-if00");
fds = open(dev_name, O_RDONLY| O_NONBLOCK);
if (fds < 0) 
	{
	printf(" Failed to open shuttle device\n");
	return -1;
    }            
// Flag it as exclusive access
if(ioctl( fds, EVIOCGRAB, 1 ) < 0) 
	{
    printf( "evgrab ioctl\n" );
	return 0;
    }

// if we get to here, we're connected to shuttleXpress
printf("Shuttle device connected. dv: %d\n",fds);
return 0;
}

int open_keypad()
{
int xx;
printf(" Hello Keypad open \n");

//open the keypad device
//strcpy(dev_name,"/dev/input/by-id/usb-SEM_HCT_Keyboard-event-if01");
strcpy(dev_name,"/dev/input/by-id/usb-SEM_HCT_Keyboard-event-kbd");

//usb-SEM_HCT_Keyboard-event-if01
//usb-SEM_HCT_Keyboard-event-kbd


fdk = open(dev_name, O_RDONLY| O_NONBLOCK);
if (fdk < 0) 
	{
	printf(" Failed to open keypad device\n");
	return -1;
    }            
// Flag it as exclusive access
if(ioctl( fdk, EVIOCGRAB, 1 ) < 0) 
	{
    printf( "evgrab not exclusive ioctl\n" );
	return -2;
    }

// if we get to here, we're connected to 
printf("Keypad device connected. dv: %d\n",fdk);
return 0;
}







void * shuttle_event(void *shuttle_thread_id)
{
int n;

while(1)
    {
    usleep(10000); //anti cpu hogger
    n=read(fds,&ev,sizeof (ev));
    if(n > 0)
	    {
    printf(" Data is rxd from the Shuttle N: %d \n",n);
    printf(" Data recd: *** %d \n",n);
    printf(" Type: %d Code: %d  Value: %d \n\n",ev.type,ev.code,ev.value);
	    }
    }
}




void * touchscreen_event(void *thread_id)
{

while(1)
    { 
    usleep(10000); //anti cpu hogger

	for (sample = 0; sample < SAMPLE_AMOUNT; sample++)
        {
		getTouchSample(&rawX, &rawY, &rawPressure);
		Xsamples[sample] = rawX;
    	Ysamples[sample] = rawY;
		}

		Xaverage  = 0;
		Yaverage  = 0;

	for ( x = 0; x < SAMPLE_AMOUNT; x++ )
        {
		Xaverage += Xsamples[x];
		Yaverage += Ysamples[x];
		}

	Xaverage = Xaverage/SAMPLE_AMOUNT;
	Yaverage = Yaverage/SAMPLE_AMOUNT;

	scaledX = 	Xaverage / scaleXvalue;
	scaledY = 	Yaverage / scaleYvalue;
	printf(" X=%d =%d \n",scaledX,scaledY);
	}
}

//---

void create_freq_digits_display(unsigned int hertz)
{
int hg,mh,mt,mu,kh,kt,ku,hh,ht,hu;
//hg=hertz/ 
//put_big_digit(freq, herz/1 
}

void draw_grid()
{
int i,x,y;

for(i=0;i<7;i++)
    plot_dotted_line(&specanz,0,i*40,1000,i*40,YELLOW);

for(i=0;i<11;i++)
    plot_dotted_line(&specanz,i*100,0,i*100,255,GREEN);
}



void draw_waterfall()
{
uint16_t colour;
int point;
unsigned char fft_val;
int loc_x,loc_y;
unsigned int wf_ln;


loc_x = 10;
loc_y = 300;

wf_ln++;
if(wf_ln > WFALL_HEIGHT)
    wf_ln = 1;

//Draw first line of waterfall
for(point=0;point<FFT_SIZE;point++)
    {
    fft_val = 255-(fft_video_buf[point]);
    fft_val*=1 ;
    colour = rgb565(turbo[fft_val][0]/8,turbo[fft_val][1]/4,turbo[fft_val][2]/8);

    colour = C_DIM_GRAY;
    if (fft_val > 20) colour = GREEN;
    if (fft_val > 80) colour = RED;    

    set_pixel(&wfall,point , 0, colour);
    set_pixel(&wfall,point+1 , 0, colour);
    set_pixel(&wfall,point+2 , 0, colour);
    set_pixel(&wfall,point+3, 0, colour);
    set_pixel(&wfall,point+4 , 0, colour);
    set_pixel(&wfall,point+5 , 0, colour);
    }

copy_surface_to_image(&wfall,loc_x,loc_y);

//Scroll all lines up, starting from the bottom
for(int ll = WFALL_HEIGHT; ll >=0 ; ll--)
    {
    for(int pp = 0;pp<WFALL_WIDTH;pp++)
        {
        wfall.data[((ll+1)*WFALL_WIDTH)+WFALL_WIDTH+pp] = wfall.data[((ll)* WFALL_WIDTH)+pp];
        }
    }
copy_surface_to_image(&wfall,loc_x,loc_y);
refresh_screen();
}

void draw_fft()
{
int loc_x,loc_y;
int dummy;
int last;
int db_lev;

loc_x=10;
loc_y=10;
last = 255;
db_lev = 255;

fill_surface(&specanz,rgb565(0x01,0x07,0x01));
draw_grid();

for(int iii = 4; iii<1040;iii+=1)
    {
    db_lev=fft_video_buf[iii];
    plot_line(&specanz,iii,255,iii,db_lev+5,BLUE);

    plot_line(&specanz,iii,last,iii,db_lev,WHITE);
    last = db_lev;
    }

usleep(100*mS); //FIXME - rate should be controled elsewhere

ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
copy_surface_to_image(&specanz,loc_x,loc_y);
refresh_screen();
//ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
}

int main_loop(int quit)
{
while(quit)
    {
    draw_fft();
    draw_waterfall();

update_pitaya_cf(5505000); //stay alive

    }
}

//===

int main(int argc, char* argv[])
{
unsigned int red,green,blue;
short rgba;
int screenbytes;
int quit_request;
pthread_t thread_id;
pthread_t shuttle_thread_id;
pthread_t keypad_thread_id;
int err;

start_server_stream();
printf(" SERVER STREAM HAS SETUP \n");

fbfd = open("/dev/fb0", O_RDWR); // Open the framebuffer device file for reading and writing
if (fbfd == -1) 
    printf("Error: cannot open framebuffer device.\n");
 
if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) // Get fixed screen information
    printf("Error reading fixed information.\n");

if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) // Get variable screen information
	    printf("Error reading variable screen info.\n");
printf("Display info %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

//openTouchScreen();
//pthread_create(&thread_id, NULL, touchscreen_event, NULL);

err = open_shuttle();
    pthread_create(&shuttle_thread_id, NULL, shuttle_event, NULL);

err = open_keypad();
    pthread_create(&keypad_thread_id, NULL, keypad_event, NULL);


screenbytes = finfo.smem_len;
screensize=screenbytes/2; //2bytes per pixel
printf(" \n screensize=%d shorts\n",screensize);

// map framebuffer to user memory 
frame_buf = (short*)mmap(0, screenbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
if ((int)frame_buf == -1) 
	    printf("Failed to mmap.\n");

clear_screen(rgb565(0x0,0x0,0x04));

make_layout();


fill_surface(&wfall,rgb565(0x00,0x08,0x00));

printf(" Looping in control loop: %d \n",__LINE__);

quit_request=main_loop(1); //loop in here until quit recieved

// cleanup
printf("\n ALL done, cleaning up \n");
munmap(frame_buf, screensize);
close(fbfd);
printf("    DONE     \n");
return 0;
}

/*
void setup_screen()
{
int i;
int x,y;
int loc_x,loc_y;

make_layout();

refresh_screen();
//plot_large_string(&meter,10,20,"METER",WHITE);
//copy_surface_to_image(&meter,METER_POS_X,METER_POS_Y);

plot_large_string(&freq,10,20,"FREQUENCY",WHITE);
//plot_huge_numeral(&freq,50,50,'7',WHITE);
//copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);
//refresh_screen();

printf(" Send C.Freq request \n");
update_pitaya_cf(5505000);

fill_surface(&wfall,GREY);
}
*/
