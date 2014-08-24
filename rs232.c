/*
* RS232.c
*
* Funktionsbibliothek f�r die serielle Kommunikation per AVR Hardware-UART
*
* Headerdatei: rs232.h
*
* Funktionen
**************************************************************************
* Funktion: ISR(USART_RXC_vect)
**************************************************************************
* Funktion: init_uart()
**************************************************************************
* Funktion: uart_putc(unsigned char c)
**************************************************************************
* Funktion: uart_puts(char *s)
**************************************************************************
* Funktion: uart_puts_p(const char *progmem_s)
**************************************************************************
* Funktion: uart_puti(const int val)
**************************************************************************
*
* Created: 23.02.2013 22:38:07
* Author: Scheik
*/

/*Includes*/
/************************************************************************/
#include <avr/io.h>															// Standard-Headerdatei(avr-libc): IO- Definitionen f�r gew�hlten Controller. (hier -mmcu=atmega32)
#include <avr/interrupt.h>													// Standard-Headerdatei(avr-libc): Funktionen in diesem Modul erm�glichen das Handhaben von Interrupts
#include <avr/pgmspace.h>													// Standard-Headerdatei(avr-libc): Funktionen in diesem Modul erm�glichen Zugriff auf Daten aus dem Flash-Memory
#include <stdlib.h>
#include "global.h"															// Headerdatei f�r globale Definitionen wie zB. Taktfrequenz FCPU (hier 16MHz)
#include "rs232.h"															// Headerdatei f�r rs232.c


/*Variablen*/
/************************************************************************/
volatile uint8_t UART_MSG_FLAG;												// Hilfs-Variable UART_MSG_FLAG. Wird in RX-Interruptroutine gesetzt, wenn letzes empfangenes Zeichen ein Linefeed (CR) war oder Ringpuffer voll ist
volatile unsigned char UART_RXBuffer[UART_BUFFER_SIZE];						// Ringpuffer- Array f�r empfangene	UART Daten, Gr��e UART_BUFFER_SIZE definiert in RS232.h
volatile uint8_t UART_RxCount = 0;											// Z�hlt empfangene Zeichen im UART- Ringpuffer


/*Funktionen*/
/************************************************************************/
/************************************************************************/

/*************************************************************************
Funktion:	ISR(USART_RXC_vect)
Zweck:		Interruptroutine RX
Eingabe:    none
Ausgabe:	none
**************************************************************************/
ISR(USART_RXC_vect)
{
	unsigned char Temp;
	Temp = UDR;																// Lesen des Datenregisters, l�scht Interrupt.
	UART_RXBuffer[UART_RxCount]=Temp;										// Speichere zeichen im Puffer- Array und ...
	UART_RxCount++;															// erh�he Pufferz�hler.
	if((Temp==0x0d) OR (UART_RxCount==UART_BUFFER_SIZE))					// <CR> oder Puffer voll?
	{
		UART_MSG_FLAG=1;													// Flag zeigt an ob das letzte empfangene Zeichen ein <CR> war ooder der Puffer voll ist, kann in Main-Funktion ausgewertet werden
	}
}


/*************************************************************************
Funktion:	init_uart()
Zweck:		Initalisiert UART mit in RS232.h definierten Parametern
Eingabe:    none
Ausgabe:	none
**************************************************************************/
void init_uart()
{
	//f�r ATMega16/32
		UCSRB |= (1 << RXEN) | (1 << TXEN);										// Setze Bits um UART Sende- und Empfangstreiber zu aktivieren
		UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);					// Setze Bits f�r Schnittstellenkonfiguration. Hier: 8N1
		// Set baud rate:
		UBRRH = (BAUD_PRESCALE >> 8);											// lade High-Byte (die oberen 8-bits) der Baudratenvariable in das High-Byte des UBRR Registers
		UBRRL = BAUD_PRESCALE;													// lade Low-Byte (die unteren 8-bits) der Baudratenvariable in das Low-Byte des UBRR Registers
		UCSRB |= (1 << RXCIE);													// setzten dieses Bits schaltet "USART Recieve Complete interrupt (USART_RXC)" ein
}


/*************************************************************************
Funktion:	uart_putc()
Zweck:		Funktion, um Bytes �ber das UART zu senden
Eingabe:    byte to be transmitted
Ausgabe:	none
**************************************************************************/
int uart_putc(unsigned char c)
{
    while (!(UCSRA & (1<<UDRE)))											// warten bis Senden moeglich: Pr�fe UDRE, Dieses Bit zeigt an, ob der Sendepuffer bereit ist, ein zu sendendes Zeichen aufzunehmen. Das Bit wird vom AVR gesetzt (1), wenn der Sendepuffer leer ist. Es wird gel�scht (0), wenn ein Zeichen im Sendedatenregister vorhanden ist und noch nicht in das Sende-Schieberegister �bernommen wurde. Atmel empfiehlt aus Kompatibilit�tsgr�nden mit kommenden �C, UDRE auf 0 zu setzen, wenn das UCSRA Register beschrieben wird. Das Bit wird automatisch gel�scht, wenn ein Zeichen in das Sendedatenregister geschrieben wird.
    {
    }
    UDR = c;																// sende Zeichen an UART
    return 0;
}


/*************************************************************************
Funktion:	uart_puts()
Zweck:		transmit string to UART
Eingabe:    string to be transmitted
Ausgabe:	none
**************************************************************************/
void uart_puts (char *s)
{
    while (*s)
    {
        uart_putc(*s);														// so lange wie *s != '\0' also ungleich dem "String-Endezeichen(Terminator)"
        s++;
    }
}


/*************************************************************************
Funktion:	uart_puts_p()
Zweck:		Funktion, um Strings aus dem Flash-EEpromspeicher �ber das UART zu senden
Eingabe:    String aus Flash-EEprom-Programmspeicher.
Ausgabe:	none
**************************************************************************/
void uart_puts_p(const char *progmem_s )
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) )
      uart_putc(c);
}


/*************************************************************************
Funktion:	uart_puti()
Zweck:		Funktion, um Zahlen �ber das UART zu senden
Eingabe:    int
Ausgabe:	none
**************************************************************************/
void uart_puti(const int val )
{
    char buffer[8];
    uart_puts( itoa(val, buffer, 10) );
}
