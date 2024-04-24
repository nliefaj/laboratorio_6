/*
 * laboratorio6.c
 *
 * Created: 4/23/2024 9:26:49 PM
 * Author : lefaj
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//variables
volatile char buffRX;
uint8_t valor_adc=0;
uint8_t leds=0;

void initUART9600(void);
void writeUART(char caracter);//funcion para escribir



int main(void)
{
	initUART9600();
	//init_adc();
	writeUART('H');
	writeUART('O');
	writeUART('L');
	writeUART('A');
	writeUART('\n');
	sei();
	
    while (1) 
    {
		
    }
}
/*
void init_adc(void){
	ADMUX = 0;
	ADCSRA=0;
	
	// VCC=5V
	ADMUX |=(1<<REFS0);
	
	// Entrada 6 de MUX
	ADMUX |=(1<<MUX1)|(1<<MUX2)|(1<<ADLAR);
	
	//Encender ADC y  habilitar isr adc
	ADCSRA=0;
	ADCSRA|=(1<<ADEN)|(1<<ADIE);
	
	// preescaler = 16M/128=125k
	ADCSRA|=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
	
	//no se utilizara el estado B
	ADCSRB=0;
	//DIDRO=0;
}

ISR(ADC_vect){
	valor_adc=ADCH;	
}*/

void initUART9600(void){
	//configurar pines tx y rx
	DDRD &=~(1<<DDD0);//entrada
	DDRD|=(1<<DDD1);//salida Tx
	
	//configurar registro A modo fast u2x0 en 1
	UCSR0A=0;
	UCSR0A|=(1<<U2X0);
	
	//configurar registro B, habilotar isr rx, bits para habilitar RX y TX
	UCSR0B=0;
	UCSR0B|=(1<<RXCIE0)|(1<<RXEN0)|(TXEN0);
	
	//Configurar registro C<Frame asincrono 8 bits de datos sin pariedad, 1 bit de stop
	UCSR0C=0;
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);
	
	//baudrate = 207 con % de error igual a 0.16% con 9600
	UBRR0=207;
	
}

void writetxtUART(char* texto){
	uint8_t i;
	for (i=0;texto[i]!='\0';i++){
		while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
		UDR0=texto[i];//cuando i nulo se acaba
	}
}

void writeUART(char caracter){
//revisar el udre0 para recibir nuevo mensaje si =1 esta vacío
	while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
	UDR0=caracter;//valor que queremos enviar
	
}

ISR(USART_RX_vect){
	buffRX=UDR0;
	while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
	leds=buffRX;//valor que queremos enviar
}
