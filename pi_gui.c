#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>

#include "pi_gui.h"
#include "tslib.h"
#include "fb2-lib.h"
#include "network.c"
#include "waterfall.h"

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


char waterfall_buffer[1024][3];
int wfall_line_num;
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

void draw_grid()
{
int i,x,y;

for(i=0;i<7;i++)
    plot_dotted_line(&specanz,0,i*40,1000,i*40,GREEN);


for(i=0;i<11;i++)
    plot_dotted_line(&specanz,i*100,0,i*100,255,GREEN);


}


void draw_fft()
{
int loc_x,loc_y;
int dummy;

loc_x=100;
loc_y=50;
//animation test
int last;
last = 0;
for(int test=0;test<1000;test++)
    {
    fill_surface(&specanz,rgb565(0x01,0x07,0x01));
draw_grid();

//plot_dotted_line(&specanz,0,220,1000,220,GREEN);


    int yyy;
    //start animation
    for(int iii =32; iii<1000;iii+=1)
        {

 //      yyy = random()%100;

yyy=fft_video_buf[iii];

     //   plot_line(&specanz,iii,yyy-5,iii,yyy,WHITE);
        plot_line(&specanz,iii,255,iii,yyy+5,BLUE);

plot_line(&specanz,iii,last,iii,yyy,WHITE);
last = yyy;


        //plot_line(&specanz,iii,0,iii,200-yyy,WHITE);
        }

draw_waterfall();

 //   usleep(200*mS);
    ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
    copy_surface_to_image(&specanz,loc_x,loc_y);
    refresh_screen();
    } 


refresh_screen();
//ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync

}

void draw_waterfall()
{
uint16_t colour;
int point;
char wf_line[1024];
char fft_val;
int y;

y = 300 + wfall_line_num;

wfall_line_num ++;
if(wfall_line_num > 100) wfall_line_num = 0;


for(point=0;point<1024;point++)
    {
 
    fft_val = fft_video_buf[point];
    colour = rgb565(turbo[fft_val][0],turbo[fft_val][1],turbo[fft_val][2]);
    wf_line[point] = colour;



set_image_pix(point,y,colour);
  //  setPixel(&wfall,point,y, colour);
}


//for (i = left; i < wide; i++)
//        {
//        int inx = 50+outbuf[i]; //100+trace_buf[i];
//        painter_wf.setPen(QColor(turbo[inx][0],turbo[inx][1],turbo[inx][2]));
//        painter_wf.drawPoint(i,0);
//        }



}


//---

void setup_screen()
{
int i;
int x,y;
int loc_x,loc_y;

make_layout();

printf(" here %d \n",__LINE__);

refresh_screen();

plot_large_string(&meter,10,20,"METER",WHITE);
copy_surface_to_image(&meter,METER_POS_X,METER_POS_Y);
plot_large_string(&freq,10,20,"FREQUENCY",WHITE);
//plot_huge_numeral(&freq,50,50,'7',WHITE);
copy_surface_to_image(&freq,FREQ_POS_X,FREQ_POS_Y);
refresh_screen();

printf(" Send CF request \n");
update_pitaya_cf(909000);

draw_fft();

}



//===

int main(int argc, char* argv[])
{
unsigned int red,green,blue;
short rgba;
int screenbytes;
pthread_t thread_id;
pthread_t shuttle_thread_id;

wfall_line_num = 0;

start_server_stream();

printf(" SERVER STREAM HAS SETIP \n");

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

//openTouchScreen();
//pthread_create(&thread_id, NULL, touchscreen_event, NULL);
//open_shuttle();
//pthread_create(&shuttle_thread_id, NULL, shuttle_event, NULL);
// map framebuffer to user memory 
screenbytes = finfo.smem_len;
screensize=screenbytes/2; //2bytes per pixel
printf(" \n screensize=%d shorts\n",screensize);

frame_buf = (short*)mmap(0, screenbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
if ((int)frame_buf == -1) 
	    printf("Failed to mmap.\n");


clear_screen(31);
sleep(1);
	
setup_screen();


printf(" Send CF request \n");
update_pitaya_cf(909000);

printf(" Looping and sleeping%d \n",__LINE__);
while(1) sleep(1);

printf("\n ALL done, cleaning up \n");
// cleanup
munmap(frame_buf, screensize);
close(fbfd);
printf("    DONE     \n");
return 0;
}

