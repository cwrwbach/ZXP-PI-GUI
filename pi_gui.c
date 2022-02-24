#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include "pi_gui.h"
#include "tslib.h"
#include <locale.h>

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

int fds;
int n, shuttle;
char dev_name[256];
typedef struct input_event EV;


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

void open_shuttle()
{
int xx;
printf(" Hello shuttle open \n");

//open the shuttleXpress device
strcpy(dev_name,"/dev/input/by-id/usb-Contour_Design_ShuttleXpress-event-if00");
fds = open(dev_name, O_RDONLY| O_NONBLOCK);
if (fds < 0) 
	{
	printf(" Failed to open shuttle device\n");
	return;
    }            
// Flag it as exclusive access
if(ioctl( fds, EVIOCGRAB, 1 ) < 0) 
	{
    printf( "evgrab ioctl\n" );
	return;
    }

// if we get to here, we're connected to shuttleXpress
printf("Shuttle device connected. dv: %d\n",fds);;
}





void * shuttle_event(void *shuttle_thread_id)
{
int n;

while(1)
{
usleep(10000); //anti cpu hogger
//printf("looping \n");
n=read(fds,&ev,sizeof (ev));
//printf (" n= %d\n",n);
if(n > 0)
	{
printf(" Data is rxd from the Shuttle N: %d \n",n);
printf(" Data recd: *** %d \n",n);
printf(" Type: %d Code: %d  Value: %d \n\n",ev.type,ev.code,ev.value);
	}

}
//This handles the reading...
//QSocketNotifier *sn;
//sn = new QSocketNotifier(fds, QSocketNotifier::Read, parent );

//connect to a slot to handle the signal from device
//QObject::connect( sn, SIGNAL(activated(int)),
          //            ui->plotter, SLOT(dataReceived(int)) );


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

//---

void setup_screen()
{
int i;
int x,y;
int loc_x,loc_y;

int dummy;

make_layout();

printf(" here %d \n",__LINE__);

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

return;


refresh_screen();

plot_large_string(&meter,10,20,"METER",WHITE);
copy_surface_to_image(&meter,METER_POS_X,METER_POS_Y);

plot_large_string(&freq,10,20,"FREQUENCY",WHITE);


//plot_large_string(&freq,50,50,"0,145.707.289",WHITE);

plot_huge_numeral(&freq,50,50,'7',WHITE);

copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);




refresh_screen();

//sleep(4);


/*
//animation test
for(int test=0;test<200;test++)
    {
    fill_surface(&spec,rgb565(0x01,0x07,0x01));
 
    int yyy;
    //start animation
    for(int iii =0; iii<1024;iii+=1)
        {
        yyy = random()%100;
        plot_line(&spec,iii,200,iii,200-yyy,WHITE);
        //plot_line(&spec,iii,0,iii,200-yyy,WHITE);
        //plot_line(&spec,iii,0,iii,200-yyy,WHITE);
        }

    usleep(200*mS);
    ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
    copy_surface_to_image(&spec,loc_x,loc_y);
    refresh_screen();
    } 
*/

refresh_screen();
//ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
}

//===

int main(int argc, char* argv[])
{
unsigned int red,green,blue;
short rgba;
int screenbytes;
pthread_t thread_id;
pthread_t shuttle_thread_id;

// Open the framebuffer device file for reading and writing
fbfd = open("/dev/fb0", O_RDWR);
if (fbfd == -1) 
        printf("Error: cannot open framebuffer device.\n");
 // Get fixed screen information
if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) 
	    printf("Error reading fixed information.\n");
// Get variable screen information
if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) 
	    printf("Error reading variable screen info.\n");
printf("Display info %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

openTouchScreen();
pthread_create(&thread_id, NULL, touchscreen_event, NULL);
open_shuttle();
pthread_create(&shuttle_thread_id, NULL, shuttle_event, NULL);
// map framebuffer to user memory 
screenbytes = finfo.smem_len;
screensize=screenbytes/2; //2bytes per pixel
printf(" \n screensize=%d shorts\n",screensize);

image = (short*)mmap(0, screenbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
if ((int)image == -1) 
	    printf("Failed to mmap.\n");
	
setup_screen();

printf(" Looping and sleeping%d \n",__LINE__);
while(1) sleep(1);

printf("\n ALL done, cleaning up \n");
// cleanup
munmap(image, screensize);
close(fbfd);
printf("    DONE     \n");
return 0;
}

