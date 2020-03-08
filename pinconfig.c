#include<stdint.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define SET_BIT(PORT,BIT) PORT|= (1<<BIT)
#define CLR_BIT(PORT,BIT) PORT&= ~(1<<BIT)

struct
{
    volatile uint8_t duty;
    uint8_t low;

} FLAG_BIT;
//uint16_t light;

void pin_config()
{
    SET_BIT(DDRD,PD6); //servo motor o/p
    CLR_BIT(DDRD,PD2); //to show left turn i/p
    CLR_BIT(DDRD,PD3); //to show right turn i/p
    SET_BIT(DDRC,PC5); //power o/p port for servomotor
    SET_BIT(DDRC,PC1); //high beam LED o/p
    //SET_BIT(DDRB,PB5); //high beam LED o/p
    SET_BIT(DDRC,PC2); //Low beam LED o/p
}
