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
uint8_t valor_buff=0;
uint8_t leds1=0;
uint8_t leds2=0;
uint8_t flag=1;
uint8_t flag_ascii=0;

void initUART9600(void);
void init_adc(void);
void writeUART(char caracter);//funcion para escribir
void writetxtUART(char* texto);



int main(void)
{
	cli();
	initUART9600();
	init_adc();
	sei();
	writeUART('H');
	writeUART('O');
	writeUART('L');
	writeUART('A');
	writeUART('\n');
    while (1)
    {
		ADCSRA|=(1<<ADSC);
		_delay_ms(100);
		if (flag==1){
			writetxtUART("Menú:\n 1. Leer potenciometro \n 2. Enviar Ascii \n");
		}
			
    }
}

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
	ADCSRA|=(1<<ADIF);
}

void initUART9600(void){
	//configurar pines tx y rx
	DDRD &=~(1<<DDD0);//entrada
	DDRD|=(1<<DDD1);//salida Tx
	
	//configurar registro A modo fast u2x0 en 1
	UCSR0A=0;
	UCSR0A|=(1<<U2X0);
	
	//configurar registro B, habilotar isr rx, bits para habilitar RX y TX
	UCSR0B=0;
	UCSR0B|=(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
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
	flag=0;
	}
}

void writeUART(char caracter){
	while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
	UDR0=caracter;//valor que queremos enviar
	
}

ISR(USART_RX_vect){
	buffRX=UDR0;
	while(!(UCSR0A&(1<<UDRE0)));//esperar hasta que el udre0 esté en 1
	valor_buff=buffRX;//valor que queremos enviar
	if (valor_buff==0b00110001){
		writeUART(valor_adc);
		writeUART('\n');
		/*leds1=(valor_adc);
		leds2=(valor_adc>>4);
		PORTB=leds1;
		PORTC=leds2;*/
		flag=1;
	}else if (valor_buff==0b00110010){
		writetxtUART("Ingrese un caracter\n");
		flag_ascii=1;
	}else if (flag_ascii==1){
		leds1=(buffRX);
		leds2=(buffRX>>4);
		PORTB=leds1;
		PORTC=leds2;
		flag=1;
		flag_ascii=0;
	}else{
		writetxtUART("Opcion invalida\n");
		flag=1;
	}
	
}
