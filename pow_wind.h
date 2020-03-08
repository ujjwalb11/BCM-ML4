#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<stdint.h>

#define SET_BIT(PORT,BIT) PORT|=(1<<BIT)
#define CLR_BIT(PORT,BIT) PORT&=~(1<<BIT)

struct control{
    volatile unsigned int lock : 1;
    volatile unsigned int up_d : 1;
    volatile unsigned int down_d : 1;
    volatile unsigned int up_p : 1;
    volatile unsigned int down_p : 1;
}cont;
