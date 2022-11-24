
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "touch.h"
//#include "tslib.h"


#define KWHT  "\x1B[37m"
#define KYEL  "\x1B[33m"
#define SAMPLE_AMOUNT 2


int fds; //shuttle


extern int  xres,yres,x;
extern int Xsamples[20];
extern int Ysamples[20];
extern int screenXmax, screenXmin;
extern int screenYmax, screenYmin;
extern float scaleXvalue, scaleYvalue;
extern int rawX, rawY, rawPressure, scaledX, scaledY;
extern int Xaverage ;
extern int Yaverage ;
extern int h;
extern int sample;







extern int fd;




void getTouchScreenDetails(int *screenXmin,int *screenXmax,int *screenYmin,int *screenYmax)
{
	unsigned short id[4];
        unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
        char name[256] = "Unknown";
        int abs[6] = {0};

        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        printf("Input device name: \"%s\"\n", name);

        memset(bit, 0, sizeof(bit));
        ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
        printf("Supported events:\n");

        int i,j,k;
        for (i = 0; i < EV_MAX; i++)
                if (test_bit(i, bit[0])) {
                        printf("  Event type %d (%s)\n", i, events[i] ? events[i] : "?");
                        if (!i) continue;
                        ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
                        for (j = 0; j < KEY_MAX; j++){
                                if (test_bit(j, bit[i])) {
                                        printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
                                        if (i == EV_ABS) {
                                                ioctl(fd, EVIOCGABS(j), abs);
                                                for (k = 0; k < 5; k++)
                                                        if ((k < 3) || abs[k]){
                                                                printf("     %s %6d\n", absval[k], abs[k]);
                                                                if (j == 0){
                                                                        if (absval[k] == "Min  ") *screenXmin =  abs[k];
                                                                        if (absval[k] == "Max  ") *screenXmax =  abs[k];
                                                                }
                                                                if (j == 1){
                                                                        if (absval[k] == "Min  ") *screenYmin =  abs[k];
                                                                        if (absval[k] == "Max  ") *screenYmax =  abs[k];
                                                                }
                                                        }
                                                }

                                        }
                                }
                        }
}

void getTouchSample(int *rawX, int *rawY, int *rawPressure)
{
	int i;
        /* how many bytes were read */
        size_t rb;
        /* the events (up to 64 at once) */
        struct input_event ev[64];

	rb=read(fd,ev,sizeof(struct input_event)*64);
        for (i = 0;  i <  (rb / sizeof(struct input_event)); i++){
          //    if (ev[i].type ==  EV_SYN)
              //           printf("\nEvent type is %s%s%s = Start of New Event\n",KYEL,events[ev[i].type],KWHT);

               // else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 1)
                 //       printf("\nEvent type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s1%s = Touch Starting\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);

              //  else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 0)
			//printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s0%s = Touch Finished\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);

                if (ev[i].type == EV_ABS && ev[i].code == 0 && ev[i].value > 0){
              //          printf("Event type is %s%s%s & Event code is %sX(0)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawX = ev[i].value;
		        }
                else if (ev[i].type == EV_ABS  && ev[i].code == 1 && ev[i].value > 0){
                   //     printf("Event type is %s%s%s & Event code is %sY(1)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawY = ev[i].value;
		}
                else if (ev[i].type == EV_ABS  && ev[i].code == 24 && ev[i].value > 0){
                  //      printf("Event type is %s%s%s & Event code is %sPressure(24)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawPressure = ev[i].value;
		}

	}
}


void getVeboseTouchSample(int *rawX, int *rawY, int *rawPressure)
{
	int i;
        /* how many bytes were read */
        size_t rb;
        /* the events (up to 64 at once) */
        struct input_event ev[64];

	rb=read(fd,ev,sizeof(struct input_event)*64);
        for (i = 0;  i <  (rb / sizeof(struct input_event)); i++){
              if (ev[i].type ==  EV_SYN)
                         printf("\nEvent type is %s%s%s = Start of New Event\n",KYEL,events[ev[i].type],KWHT);

                else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 1)
                        printf("\nEvent type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s1%s = Touch Starting\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);

                else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 0)
			printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s0%s = Touch Finished\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);

                else if (ev[i].type == EV_ABS && ev[i].code == 0 && ev[i].value > 0){
                        printf("Event type is %s%s%s & Event code is %sX(0)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawX = ev[i].value;
		}
                else if (ev[i].type == EV_ABS  && ev[i].code == 1 && ev[i].value > 0){
                        printf("Event type is %s%s%s & Event code is %sY(1)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawY = ev[i].value;
		}
                else if (ev[i].type == EV_ABS  && ev[i].code == 24 && ev[i].value > 0){
                        printf("Event type is %s%s%s & Event code is %sPressure(24)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
			*rawPressure = ev[i].value;
		}

	}
}


int openTouchScreen()
{
char dev_name[256];
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
