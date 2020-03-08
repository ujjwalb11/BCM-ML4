#include "pow_wind.h"

void windowp_down()
{
    PORTB = 0x0C;
    _delay_ms(100);
    PORTB = 0x06;
    _delay_ms(100);
    PORTB = 0x03;
    _delay_ms(100);
    PORTB = 0x09;
    _delay_ms(100);
}

void windowd_down()
{

    SET_BIT(PORTD, PD7);
    SET_BIT(PORTD, PD5);
    CLR_BIT(PORTB, PB7);
    CLR_BIT(PORTB, PB6);
    _delay_ms(100);

    CLR_BIT(PORTD, PD7);
    SET_BIT(PORTD, PD5);
    SET_BIT(PORTB, PB7);
    CLR_BIT(PORTB, PB6);
    _delay_ms(100);

    CLR_BIT(PORTD, PD7);
    CLR_BIT(PORTD, PD5);
    SET_BIT(PORTB, PB7);
    SET_BIT(PORTB, PB6);
    _delay_ms(100);

    SET_BIT(PORTD, PD7);
    CLR_BIT(PORTD, PD5);
    CLR_BIT(PORTB, PB7);
    SET_BIT(PORTB, PB6);
    _delay_ms(100);

}
