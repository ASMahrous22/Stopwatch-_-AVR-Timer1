/*
 * sw.c
 *
 *  Created on: ٠٥‏/٠٢‏/٢٠٢٤
 *      Author: AS.Mahrous
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr/delay.h"

unsigned char sec=0 , min=0 , hour=0 ;

void INT0_Init(void);                /* interrupt0 initiation function "RESET interrupt" */
void INT1_Init(void);                /* interrupt1 initiation function "PAUSE interrupt" */
void INT2_Init(void);                /* interrupt2 initiation function "RESUME interrupt" */
void Timer1_CTC_Init(void);          /* timer1 compare mode initiation function */


ISR(INT0_vect)                        /* setting sec,min,hours to zero to RESET */
{
	sec=0;
	min=0;
	hour=0;
}

ISR(INT1_vect)                        /* when setting the TCCR1B register to 0 the timer will PAUSE */
{
	TCCR1B = 0;
}

ISR(INT2_vect)                        /* when re-setting back the TCCR1B register value the timer will RESUME */
{
	TCCR1B |= (1<<CS10) | (1<<CS11) | (1<<WGM12);
}

ISR(TIMER1_COMPA_vect)
{
	/* when the compare match value of OCR1A register reachws 15625, this means the timer has counted 1 second */
	sec++;
	if(sec==60)
	{
		sec=0;
		min++;
	}
	if(min==60)
	{
		min=0;
		hour++;
	}
	if(hour==24)
	{
		hour = 0;
	}
}

int main(void)
{
	DDRC |= 0x0F;                    /* setting first 4 pins in port C as out for the 7447-decoder */
	PORTC = 0x00;                    /* displaying zero on the 7-segments initially */

	DDRA = 0x3F;                     /* setting first 6 pins in port A as out for enabling the six 7-segments */
	PORTA = 0x3F;                    /* enabling the six 7-segments by putting 1 to the first 6 pins */

	INT0_Init();                     /* calling INT0 initiation funtion */
	INT1_Init();                     /* calling INT1 initiation funtion */
	INT2_Init();                     /* calling INT2 initiation funtion */
	Timer1_CTC_Init();               /* calling timer1 initiation funtion */

	for(;;)
	{
		PORTA = 0x20;                                    /* enabling the 1st 7-segment */
		PORTC = (PORTC & 0xF0) | ((sec % 10) & 0x0F);    /* sec%10 to increment the 1st 7-segment from 0 to 9 */
		_delay_ms(1);
		PORTA = 0x10;                                    /* enabling the 2nd 7-segment */
		PORTC = (PORTC & 0xF0) | ((sec/10) & 0x0F);      /* sec/10 to increment the 2nd 7-segment each 10 seconds */
		_delay_ms(1);

		PORTA = 0x08;                                    /* enabling the 3rd 7-segment */
		PORTC = (PORTC & 0xF0) | ((min % 10) & 0x0F);    /* min%10 to increment the 3rd 7-segment from 0 to 9 */
		_delay_ms(1);
		PORTA = 0x04;                                    /* enabling the 4th 7-segment */
		PORTC = (PORTC & 0xF0) | ((min/10) & 0x0F);      /* min/10 to increment the 4th 7-segment each 10 minutes */
		_delay_ms(1);

		PORTA = 0x02;                                    /* enabling the 5th 7-segment */
		PORTC = (PORTC & 0xF0) | ((hour % 10) & 0x0F);   /* hour%10 to increment the 5th 7-segment from 0 to 9 */
		_delay_ms(1);
		PORTA = 0x01;                                    /* enabling the 6th 7-segment */
		PORTC = (PORTC & 0xF0) | ((hour/10) & 0x0F);     /* hour/10 to increment the 2nd 7-segment each 10 hours */
		_delay_ms(1);
	}
}

void Timer1_CTC_Init(void)           /* timer1 compare mode initiation function */
{
    TCCR1A |= (1<<FOC1A) | (1<<FOC1B);         /* setting FOC1A & FOC1B as we are in non-pwm mode */

   /* setting CS11 & CS10 as we use 64-prescaler , also WGM12 for CTC-mode */
    TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);

	TCNT1 = 0 ;                                /* setting initial value to the timer/counter with zero */

   /* setting a compare match value in OCR1A = 15625 , when matching this value the timer will be 1 sec */
	OCR1A = 15625;

    TIMSK |= (1<<OCIE1A);                      /* enabling OCR1A compare match interrupt */

    SREG |= (1<<7);                            /* enabling interrupt bit "I-bit" */
}

void INT0_Init(void)                 /* interrupt0 initiation function "RESET interrupt" */
{
	DDRD &= ~(1<<PD2);                      /* setting pin2 at port D as an input pin to trigger interrupt 0 */

	PORTD |= (1<<PD2);                      /* activating internal pull up for pin2 at port D */

	MCUCR |= (1<<ISC01);                    /* the falling edge of INT0 generates an interrupt request */

	GICR |= (1<<INT0);                      /* enabling INT0 "Interrupt 0" */

	SREG |= (1<<7);                         /* enabling interrupt bit "I-bit" */
}

void INT1_Init(void)                 /* interrupt1 initiation function "PAUSE interrupt" */
{
	DDRD &= ~(1<<PD3);                      /* setting pin3 at port D as an input pin to trigger interrupt 1 */

	MCUCR |= (1<<ISC11) | (1<<ISC10);       /* the rising edge of INT1 generates an interrupt request */

	GICR |= (1<<INT1);                      /* enabling INT1 "Interrupt 1" */

	SREG |= (1<<7);                         /* enabling interrupt bit "I-bit" */
}

void INT2_Init(void)                 /* interrupt2 initiation function "RESUME interrupt" */
{
	DDRB &= ~(1<<PB2);                      /* setting pin2 at port B as an input pin to trigger interrupt 2 */

	PORTB |= (1<<PB2);                      /* activating internal pull up for pin2 at port B */

	MCUCSR &= ~(1<<ISC2);                   /* the falling edge of INT2 generates an interrupt request */

	GICR |= (1<<INT2);                      /* enabling INT2 "Interrupt 2" */

	SREG |= (1<<7);                         /* enabling interrupt bit "I-bit" */
}
