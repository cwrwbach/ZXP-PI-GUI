#include "fb2-lib.h"
#include "fb2-font.h"
#include "ArialNum32x50.c"

#define HORIZONTAL 0
#define VERTICAL 1

extern short * frame_buf;
extern int screensize;

//---

extern struct surface
{
uint16_t * data;
int sz_x;
int sz_y;
int pos_x;
int pos_y;
}pixbuf;

void create_surface(struct surface * cs_dest)
{
cs_dest->data = malloc(PANEL_WIDTH * PANEL_HEIGHT * 2);
cs_dest->sz_x=PANEL_WIDTH;
cs_dest->sz_y = PANEL_HEIGHT;
}

uint16_t get_image_pix(int x,int y)
{
uint16_t pixval;
if(x<0) x=1;
if(x > IMAGE_WIDTH-1) x=IMAGE_WIDTH;
if(y<0) y=0;
if(y>IMAGE_HEIGHT-1) IMAGE_HEIGHT-1;
pixval = frame_buf[x+y*IMAGE_WIDTH];
return pixval;
}

void clear_screen(uint16_t pixval)
{
for(int p=0;p<(1280*800);p++)
    frame_buf[p] = pixval;
}


void set_image_pix(int x,int y,uint16_t pixval)
{
if(x<0) x=1;
if(x > IMAGE_WIDTH-1) x=IMAGE_WIDTH;
if(y<0) y=0;
if(y>IMAGE_HEIGHT-1) IMAGE_HEIGHT-1;
frame_buf[x+y*IMAGE_WIDTH] = pixval;
}

void xor_image_pix(int x,int y,uint16_t pixval)
{
if(x<0) x=1;
if(x > IMAGE_WIDTH-1) x=IMAGE_WIDTH;
if(y<0) y=0;
if(y>IMAGE_HEIGHT-1) IMAGE_HEIGHT-1;
frame_buf[x+y*IMAGE_WIDTH] ^= pixval;
}

void set_pixel(struct surface * dest, int x, int y, uint16_t colour)
{
int sz_x,sz_y;
int location;

sz_x = dest->sz_x;
sz_y = dest->sz_y;

if(x<0) x=1;
if(x > sz_x-1) x=sz_x-1;
if(y<0) y=0;
if(y>sz_y-1) y=sz_y-1;

location = x + (y*sz_x) ; 
dest->data[location] = colour;
}

void refresh_screen()
{
//always copies from "pixbuf" to the FrameBuffer "image"
memcpy(frame_buf,pixbuf.data,screensize*2); //memcopy works in bytes
}

void copy_surface_to_image(struct surface * surf,int loc_x,int loc_y)
{
int x,y;

for(y=0;y<surf->sz_y;y++) //height
   for(x=0;x<surf->sz_x;x++) //width
        {
        pixbuf.data[x+loc_x + (loc_y*IMAGE_WIDTH)+(y*IMAGE_WIDTH)] = surf->data[x+y*surf->sz_x];
        }
}

void fill_surface(struct surface * surf,uint16_t colour)
{
for(int i = 0 ;i< surf->sz_x * surf->sz_y;i++) 
	    surf->data[i] = colour; 
}

void plot_line (struct surface * dest, int x0, int y0, int x1, int y1,uint16_t colour)
{
int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
int err = dx + dy, e2; //error value e_xy 

for (;;) //evah
    { 
    set_pixel (dest,x0,y0,colour);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } // e_xy+e_x > 0 
    if (e2 <= dx) { err += dx; y0 += sy; } // e_xy+e_y < 0 
    }
}

void plot_dotted_line (struct surface * dest, int x0, int y0, int x1, int y1,uint16_t colour)
{
int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
int err = dx + dy, e2; //error value e_xy 
int count;
count = 0;
for (;;) //evah
    { 
    if(count++ & 0x04)// miss out spaces
        set_pixel (dest,x0,y0,colour);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } // e_xy+e_x > 0 
    if (e2 <= dx) { err += dx; y0 += sy; } // e_xy+e_y < 0 
    }
}

void plot_thick_line (struct surface * dest, int x0, int y0, int x1, int y1,uint16_t colour)
{
int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
int err = dx + dy, e2; //error value e_xy 
for (;;) //evah
    { 
    set_pixel (dest,x0,y0,colour);
    set_pixel (dest,x0+1,y0,colour);
    set_pixel (dest,x0,y0+1,colour);
    set_pixel (dest,x0+1,y0+1,colour);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } // e_xy+e_x > 0 
    if (e2 <= dx) { err += dx; y0 += sy; } // e_xy+e_y < 0 
    }

}

void plot_circle (struct surface * dest, int xm, int ym, int r,uint16_t colour)
{
int sz_x,sz_y;
sz_x = dest->sz_x;
sz_y = dest->sz_y;

int x = -r, y = 0, err = 2-2*r; // II. Quadrant 
   do {
      set_pixel (dest,xm-x, ym+y,colour); //   I. Quadrant 
      set_pixel (dest,xm-y, ym-x,colour); //  II. Quadrant 
      set_pixel (dest,xm+x, ym-y,colour); // III. Quadrant 
      set_pixel (dest,xm+y, ym+x,colour); //  IV. Quadrant 
      r = err;
      if (r >  x) err += ++x*2+1; // e_xy+e_x > 0 
      if (r <= y) err += ++y*2+1; // e_xy+e_y < 0 
   } while (x < 0);
}

void plot_rectangle(struct surface * dest, int x0, int y0,int sz_x, int sz_y, uint16_t colour)
{
plot_line(dest, x0, y0, x0+sz_x, y0,colour);
plot_line(dest, x0, y0+sz_y, x0+sz_x, y0+sz_y,colour);
plot_line(dest, x0, y0, x0, y0+sz_y,colour);
plot_line(dest, x0+sz_x, y0, x0+sz_x, y0+sz_y,colour);
}

void plot_thick_rectangle(struct surface * dest, int x0, int y0,int sz_x, int sz_y,int thickness, uint16_t colour)
{
for(int n=0 ; n<thickness;n++)
    {
    plot_thick_line(dest, x0, y0, x0+sz_x, y0,colour);
    plot_thick_line(dest, x0, y0+sz_y, x0+sz_x, y0+sz_y,colour);
    plot_thick_line(dest, x0, y0, x0, y0+sz_y,colour);
    plot_thick_line(dest, x0+sz_x, y0, x0+sz_x, y0+sz_y,colour);
    }
}

void plot_button(struct surface  * dest,int x0,int y0,int sz_x, int sz_y,uint16_t backround_col,uint16_t border_col,uint16_t text_col,char text[40])
{
 fill_surface(dest,backround_col);
plot_rectangle(dest,0,0,sz_x,sz_y,border_col);
plot_large_string(dest,10,20,text,text_col);

}

void plot_small_character(struct surface * dest, int x, int y,uint8_t char_num,uint16_t colour)
{
int horiz,vert;
uint8_t test_byte;
for(vert=0; vert<16;vert++)
    {
    test_byte = defaultFontData[(char_num * 16)+vert];

    for(horiz = 0; horiz<8;horiz++)
        {
        if(test_byte & 0x80) 
             set_pixel(dest,x+horiz,y+vert,colour);
        test_byte <<=1; 
        }
    }
}

void plot_huge_numeral(struct surface * dest, int x, int y,uint8_t numeral,uint16_t colour)
{
//This uses characters which are 32 X 50 (only does 0 to 9 and colon)
//The colon character is 0x3a
int horiz,vert,byt,bn;
int xx,yy;
unsigned char test_byte;

numeral -= 0x30; //ascii to digit
numeral *=4; //4 bytes per row, 

for(vert=0,yy=0; vert<50;vert++,yy+=1) //50 rows *4 bytes = 200 bytes per numeral
    {
    for(byt=0;byt<4;byt++)
        {
        bn=(numeral*50)+byt+(vert*4);
        test_byte = ArialNumFontPlus[bn]; 
        for(horiz =0,xx=0; horiz<8;horiz++,xx++)
            {
            if(test_byte & 0x80) //MSB only
                { 
                set_pixel(dest,x+xx+(byt*8),y+yy,colour); 
                }
            test_byte <<=1; //shifts 8 bits per byte
            }
        }
    }
}

void plot_large_character(struct surface * dest, int x, int y,uint8_t char_num,uint16_t colour)
{
int horiz,vert;
int xx,yy;
unsigned short test_word;

for(vert=0,yy=0; vert<24;vert++,yy+=1)
    {
    test_word = ASCII_16x24[((char_num-32) * 24)+vert]; 
    for(horiz =0,xx=0; horiz<16;horiz++,xx++)
        {
        if(test_word & 0x0001)
            { 
            set_pixel(dest,x+xx,y+yy,colour);
            }
        test_word >>=1; 
        }
    }
}

void plot_large_string(struct surface * dest, int x, int y,uint8_t * string ,uint16_t colour)
{
int len = strlen(string);
char char_num;
int xp = 0;

for(int s=0;s<len;s++,xp++)
    {
    char_num = string[s];
    plot_large_character(dest,x+xp*15,y,char_num,colour);
    }
}

short rgb565(short red,short green,short blue)
{
short colour= 0;
colour = (red & 0x1f) <<11;
colour = colour | (green & 0x3f) <<6;
colour = colour | blue & 0x1f;
return colour;
}

static struct timespec timediff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) 
	{
    dif.tv_sec = end.tv_sec - start.tv_sec - 1;
    dif.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
  else 
	{
    dif.tv_sec = end.tv_sec - start.tv_sec;
    dif.tv_nsec = end.tv_nsec - start.tv_nsec;
	}	
return temp;
}

void show_time()
{
clock_gettime(CLOCK_REALTIME, &end);
timediff(start, end);
dif.tv_nsec/=1000;
usec = (float) dif.tv_sec + (float) dif.tv_nsec/1000000;
printf("done %2.3f mS = %2.2f fps\n",usec*1000,1/usec);
}

void UiLcdHy28_DrawStraightLine(int x,int y,int sz,int dir ,short colour)
{
x=x+80;
y=y-50;

if(dir == HORIZONTAL)
    {
    plot_line(&pixbuf,x,y,x+sz,y,WHITE);
    }
else
    {
    plot_line(&pixbuf,x,y,x,y+sz,WHITE);
    }
}

void UiLcdHy28_DrawEmptyRect(int x,int y,int sz_y,int sz_x,short colour)
{
//(" der \n");
}

void UiLcdHy28_PrintText(int x,int y,uint8_t num,short foregnd,short backgnd,int dunno)
{
x=x+80;
y=y-50;
plot_small_character(&pixbuf,x,y,0x32,WHITE);
}



//================
