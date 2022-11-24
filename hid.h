#pragma once


#define PAD_ENTER 96
#define PAD_0 82
#define PAD_1 79
#define PAD_2 80
#define PAD_3 81
#define PAD_4 75
#define PAD_5 76
#define PAD_6 77
#define PAD_7 71
#define PAD_8 72 
#define PAD_9 73

#define PAD_SLASH 98
#define PAD_STAR 55
#define PAD_MINUS 74 
#define PAD_PLUS 78
#define PAD_BS 14
#define PAD_DEL 83
#define PAD_NUMLOCK 69

void plot_freq_digits();
void * keypad_event(void *keypad_thread_id);
