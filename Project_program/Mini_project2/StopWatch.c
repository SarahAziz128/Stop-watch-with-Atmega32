/*
 * StopWatch.c
 *
 *  Created on: Sep 19, 2022
 *      Author: Sara
 */

#include <avr\io.h>
#include <avr\interrupt.h>
#include "util\delay.h"

unsigned char tick;
unsigned char SEC1 ;
unsigned char SEC2 ;
unsigned char MIN1 ;
unsigned char MIN2 ;
unsigned char HR1 ;
unsigned char HR2 ;


void INT0_Init(void){ /*INTERRUPT 0 is set to falling edge and has input pin PD2*/
	DDRD  &=~(1<<PD2);
	GICR  |= (1<<INT0);
	MCUCR |= (1<<ISC01);
	PORTD|=(1<<2);
	GIFR|=(1<<INTF0);

}
void INT1_Init(void){ /*INTERRUPT 1 is set to rising edge and has input pin PD3*/
	DDRD  &=~(1<<PD3);
	GICR  |= (1<<INT1);
	MCUCR |= (1<<ISC11) | (1<<ISC10);

}

void INT2_Init(void){ /*INTERRUPT 2 is set to rising edge and has input pin PB2*/
	DDRB  &=~(1<<PB2);
	GICR  |= (1<<INT2);
	MCUCR |= (1<<ISC2);
	PORTB|=(1<<2);
	GIFR|=(1<<INTF2);

}

void Timer1_Init (void){ /*TIMER 1 is set on compare mode, compare value is set to 1000 for approximation however 979 is more accurate to real time*/
	TIMSK |= (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	TCNT1=0;
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TIFR |= (1<<OCF1A);
	OCR1A = 1000;

}

ISR (INT0_vect)
{
	SEC1=0;
	SEC2=0;
	MIN1=0;
	MIN2=0;
	HR1=0;
	HR2=0;
}

ISR (INT1_vect)
{
	TCCR1B &=~ ((1<<CS10) | (1<<CS12)); /*disable clock pins */

}

ISR(INT2_vect)
{
	TCCR1B |= ((1<<CS10) | (1<<CS12));  /*enable clock pins*/
}

ISR(TIMER1_COMPA_vect){

	SEC1++;
	if(SEC1==10){
		SEC2++;
		SEC1=0;
	}
	if(SEC2==6){
		MIN1++;
		SEC2=0;
	}
	if(MIN1==10){
		MIN2++;
		MIN1=0;
	}
	if(MIN2==6){
		HR1++;
		MIN2=0;
	}
	if(HR1==10){
		HR2++;
		HR1=0;
	}
	if ((HR2==2) & (HR1==3) & (MIN2==9) & (MIN1==9) & (SEC2==9) & (SEC1==9)){ /* day stopwatch shall be reset when it reaches 23:99:99*/
		SEC1=0;
		SEC2=0;
		MIN1=0;
		MIN2=0;
		HR1=0;
		HR2=0;
	}

}
int main (void){

	Timer1_Init ();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	SREG|=(1<<7);

	DDRC |=0x0F;     /*setting 7447 ready */
	PORTC &=~(0x0F); /*giving initial value by turning 7447 off*/

	DDRA|=0x3f;      /*setting 7-segments ready*/
	PORTA&=~(0x3f);  /*giving initial value by turning 7-segments off*/

	while (1){

		        PORTA=0x01;  /*only display no.6 is enabled to display unit second*/
				PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F);
				_delay_ms(2);

				PORTA=0x02;  /*only display no.5 is enabled to display tenth of second*/
				PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F);
				_delay_ms(2);

				PORTA=0x04;  /*only display no.4 is enabled to display unit minute*/
				PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F);
				_delay_ms(2);

				PORTA=0x08;  /*only display no.3 is enabled to display tenth of minute*/
				PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F);
				_delay_ms(2);

				PORTA=0x10;  /*only display no.2 is enabled to display unit hour*/
				PORTC = (PORTC & 0xF0) | (HR1 & 0x0F);
				_delay_ms(2);

				PORTA=0x20;   /*only display no.1 is enabled to display tenth of hour*/
				PORTC = (PORTC & 0xF0) | (HR2 & 0x0F);
				_delay_ms(2);
	}

}
