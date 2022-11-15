#include <linux/input.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "keypad.h"


typedef struct input_event EV;

EV ev;

extern int fdk; //keypad
extern char digit_display[10];


void * keypad_event(void *keypad_thread_id)
{
int n;
int *pstatus;

char fred;

while(0 ) //This does not work - as such
    {
    fred = 0x0f;
    ioctl (fdk, NULL, KDSETLED, fred);
    usleep(500000);
    fred = 0x00;
    ioctl (fdk, NULL, KDSETLED, fred);
    usleep(500000);
    }


while(1)
{
usleep(10000); //anti cpu hogger
//printf("looping \n");
n=read(fdk,&ev,sizeof (ev));
//printf (" n= %d\n",n);
if(n > 0)
	{
printf(" Data is rxd from the Keypad N: %d \n",n);
printf(" Data recd: *** %d \n",n);
printf(" Type: %d Code: %d  Value: %d \n\n",ev.type,ev.code,ev.value);
  

switch (ev.code)
    {

    case KEY_0 :
    digit_display[8] = '0';
    break;

case KEY_1 :
    digit_display[8] = '1';
    break;

case KEY_2 :
    digit_display[8] = '2';
    break;
case KEY_3 :
    digit_display[8] = '3';
    break;
case KEY_4 :
    digit_display[8] = '4';
    break;
case KEY_5 :
    digit_display[8] = '5';
    break;
case KEY_6 :
    digit_display[8] = '6';
    break;
case KEY_7 :
    digit_display[8] = '7';
    break;
case KEY_8 :
    digit_display[8] = '8';
    break;
case KEY_9 :
    digit_display[8] = '9';

    break;


        }



plot_freq_digits();



refresh_screen();

    }






     



}
//This handles the reading...
//QSocketNotifier *sn;
//sn = new QSocketNotifier(fds, QSocketNotifier::Read, parent );

//connect to a slot to handle the signal from device
//QObject::connect( sn, SIGNAL(activated(int)),
          //            ui->plotter, SLOT(dataReceived(int)) );


}


