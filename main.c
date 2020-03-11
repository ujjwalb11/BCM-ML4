#include<stdint.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define SET_BIT(PORT,BIT) PORT|= (1<<BIT)
#define CLR_BIT(PORT,BIT) PORT&= ~(1<<BIT)
#include "pow_wind.h"

struct
{
    volatile unsigned int FLAG_0:1;
        volatile unsigned int FLAG_1:1;
        volatile uint8_t duty;
        uint8_t low;
    } FLAG_BIT;

    uint16_t value;
    int i=0;
    uint8_t count1 = 0;
    uint8_t count2 = 0;

    int main()
{
    cont.lock = 0;
    cont.up_p = 0;
    cont.down_p = 0;
    cont.up_d = 0;
    cont.down_d = 0;


    CLR_BIT(DDRD,PD4);      //VENTILATION MODE
    SET_BIT(PORTD,PD4);
    SET_BIT(DDRD,PD6);
    CLR_BIT(DDRD,PD3);
    SET_BIT(PORTD,PD3);
    CLR_BIT(DDRD,PD2);
    SET_BIT(PORTD,PD2);
    EIMSK|=(1<<INT0 | 1<<INT1);       //CONFIGURING INTERRUPTS
    EICRA|=(1<<ISC00 | 1<<ISC01);
    EICRA|=(1<<ISC11 | 1<<ISC10);
    sei();
    pin_config(); //I/P and O/P pin configuration function call


    TCCR0A =0x83; //Fast PWM mode
    TCNT0=0x00;// Timer counter register
    OCR0A=128;
    TCCR0B=0x05; //clock pre-scalar set to 1024

    CLR_BIT(PORTC,PC5); // default position

    CLR_BIT(DDRC,PC0);          //ADC FOR RAIN SENSOR
    SET_BIT(PORTC,PC0);

    SET_BIT(DDRB,PB4);
    DDRB = 0x0F;            //STEPPER MOTOR
    PORTB = 0x09;

    config_switch();    // power window initialization
    config_Stepper();

    while(1) //super loop
    {
        if(FLAG_BIT.FLAG_1==1)
        {
            OCR0A+=13;
            FLAG_BIT.FLAG_1=0;
        }//PWM WAVE +5% DUTY_CYCLE
        ADMUX&=0x00;
        ADMUX|= (1<<REFS0);
        ADCSRA|= (1<< ADEN);
        ADCSRA|=(1<<ADSC);          //START CONVERSION
        while(ADCSRA & (1<<ADSC));
        value=ADC;

        if(value<221)//DISTANCE IS LESS TRHAN A CERTAIN VALUE
        {
            if(OCR0A>128 && OCR0A<142)//UNIQUE KEY ie.141
            {
                SET_BIT(PORTB,PB4);//DOOR OPEN

            }
            else
            {
                CLR_BIT(PORTB,PB4);//DOOR CLOSE
            }
        }


        if(value>222 && value<512)
        {
            if(value>300 && value<400)
            {
                for(i=0; i<8; i++)      //CLOCKWISE
                {
                    PORTB = 0x09;
                    _delay_ms(100);
                    PORTB = 0x03;
                    _delay_ms(100);
                    PORTB = 0x06;
                    _delay_ms(100);
                    PORTB = 0x0C;
                    _delay_ms(100);
                }
                PORTB = 0x09;
                _delay_ms(100);
                _delay_ms(1000);
            }
            uint8_t PIN_READ=0x00;
            PIN_READ=PIND;
            if(FLAG_BIT.FLAG_0==1)         //WHEN CLOSE MODE IS SELECTED
            {
                for(i=0; i<8; i++)
                {
                    PORTB = 0x09;       //CLOCKWISE
                    _delay_ms(100);
                    PORTB = 0x03;
                    _delay_ms(100);
                    PORTB = 0x06;
                    _delay_ms(100);
                    PORTB = 0x0C;
                    _delay_ms(100);
                }
                PORTB = 0x09;
                _delay_ms(100);
                _delay_ms(1000);
                FLAG_BIT.FLAG_0=0;
            }
            else if(FLAG_BIT.FLAG_1==1)    //WHEN OPEN MODE IS SELECTED
            {
                for(i=0; i<8; i++)
                {
                    PORTB = 0x09;       //ANTICLOCKWISE
                    _delay_ms(100);
                    PORTB = 0x0C;
                    _delay_ms(100);
                    PORTB = 0x06;
                    _delay_ms(100);
                    PORTB = 0x03;
                    _delay_ms(100);
                }
                PORTB = 0x09;		/* Last step to initial position */
                _delay_ms(100);
                _delay_ms(1000);
                FLAG_BIT.FLAG_1=0;
            }
            else if(~PIN_READ & (1<<PD4))    //WHEN VENTILATION MODE IS SELECTED
            {
                for(i=0; i<4; i++)
                {
                    PORTB = 0x09;           //ANTICLOCKWISE
                    _delay_ms(100);
                    PORTB = 0x0C;
                    _delay_ms(100);
                    PORTB = 0x06;
                    _delay_ms(100);
                    PORTB = 0x03;
                    _delay_ms(100);
                }
                PORTB = 0x09;		/* Last step to initial position */
                _delay_ms(100);
                _delay_ms(1000);
            }
        }


        if (value>513 && value<750)
        {
            if(PINC & (1<<PC6))
                cont.lock = 1;
            else
                cont.lock = 0;

            if(PIND & (1<<PD0))
                cont.up_p = 1;
            else
                cont.up_p = 0;

            if(PIND & (1<<PD1))
                cont.down_p = 1;
            else
                cont.down_p = 0;

            if(PINC & (1<<PC3))
                cont.up_d = 1;
            else
                cont.up_d = 0;

            if(PINC & (1<<PC4))
                cont.down_d = 1;
            else
                cont.down_d = 0;

            if(value < 592)
            {
                if(cont.up_d == 1 && count1 < 6)
                {
                    windowd_up();
                    count1++;
                }
                else if(cont.down_d == 1 && count1 > 0)
                {
                    windowd_down();
                    count1--;
                }

                if(cont.lock == 0)
                {
                    if(cont.up_p == 1 && count2 < 6)
                    {
                        windowp_up();
                        count2++;
                    }

                    else if(cont.down_p == 1 && count2 > 0)
                    {
                        windowp_down();
                        count2--;
                    }
                }
            }

            else if(value > 592 && value < 671) // obstacle
            {
                while(count1 > 0)
                {
                    windowd_down();
                    count1--;
                }
                while(count2 > 0)
                {
                    windowp_down();
                    count2--;
                }
            }
            else // rain
            {
                while(count1 < 6)
                {
                    windowd_up();
                    count1++;
                }
                while(count2 < 6)
                {
                    windowp_up();
                    count2++;
                }
            }
        }




        if(value>750 && value<1024)
        {

            if(value>=750 && value<830) //light threshold as day time
            {
                CLR_BIT(PORTC,PC1);
                CLR_BIT(PORTC,PC2); //headlight OFF
            }
            else if(value>=831 && value<1024) //light threshold as night time
            {

                if(value>=925 && value<1024) //speed threshold
                {
                    CLR_BIT(PORTC,PC2);
                    SET_BIT(PORTC,PC1); //high beam
                }

                if(value>=832 && value<925)
                {
                    CLR_BIT(PORTC,PC1);
                    SET_BIT(PORTC,PC2); //low beam
                }
            }

            if(!(PIND & (1<<PD0)) && !(PIND & (1<<PD1))) //if both are on due to defect then
                //default position of headlights
            {
                CLR_BIT(PORTC,PC5); //default position
            }

            if(!(PIND & (1<<PD0))) //for left turning path
            {
                FLAG_BIT.duty=128;
                OCR0A=FLAG_BIT.duty;
                SET_BIT(PORTC,PC5); //headlights turn left
            }

            if(!(PIND & (1<<PD1))) //for right turning path
            {
                FLAG_BIT.duty=0;
                OCR0A=FLAG_BIT.duty;
                SET_BIT(PORTC,PC5); //headlights turn right
            }
        }
    }
}


void config_switch()
{
    CLR_BIT(DDRC,PC6);  // Child Lock
    CLR_BIT(DDRD,PD7);  // window driver up/ down
    CLR_BIT(DDRD,PB0);
    CLR_BIT(DDRD,PD0);  // window passenger up/ down
    CLR_BIT(DDRD,PD1);
}

void config_Stepper()
{
    SET_BIT(DDRD,PD7);  // Setting pins as output for stepper motor
    SET_BIT(DDRD,PD5);
    SET_BIT(DDRB,PB7);
    SET_BIT(DDRB,PB6);

    SET_BIT(DDRB,PB3);
    SET_BIT(DDRB,PB2);
    SET_BIT(DDRB,PB1);
    SET_BIT(DDRB,PB0);
}

ISR(INT0_vect)
{
    FLAG_BIT.FLAG_0=1;
}
ISR(INT1_vect)
{
    FLAG_BIT.FLAG_1=1;
}

