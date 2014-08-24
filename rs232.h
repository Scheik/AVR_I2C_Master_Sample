/*
 * RS232.h
 *
 * Headerdatei f�r rs232.c
 *
 * Created: 23.02.2013 22:41:44
 * Author: Scheik
 */

#ifndef RS232_H_
#define RS232_H_


/* RS232 Definitionen                   */
/****************************************/
#define UART_BUFFER_SIZE 16													// UART-Puffer- Gr��e, in Bytes
#define UART_BAUDRATE 19200UL												// RS232 Baudrate
#define BAUD_PRESCALE (((F_CPU / (UART_BAUDRATE * 16UL))) - 1)
#define UBRR_VAL ((F_CPU+UART_BAUDRATE*8)/(UART_BAUDRATE*16)-1)				// runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))									// Reale Baudrate bei gegebener Taktfrequenz (FCPU) ermitteln
#define BAUD_ERROR ((BAUD_REAL*1000)/UART_BAUDRATE)							// Fehler in Promille ermitteln
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
	#error Systematischer Fehler der Baudrate gr�sser 1% und damit zu hoch! // Ist der Fehler > 1% gibt der Compiler einen Fehler aus
#endif
#define CR "\r\n"															// Ein Zeilenumbruch, abh�ngig davon, was die Gegenstelle haben will, hier f�r Windows : "\r\n"
																			// Linux  : "n"
																			// MacOS  : "r"
/* Funktionen und Variablen in rs232.c */
/***************************************/
extern volatile uint8_t UART_MSG_FLAG;										// Variable UART_MSG_FLAG=1, wenn letzes empfangenes Zeichen ein Zeilenumbruch war oder UART_Puffer voll ist
extern volatile unsigned char UART_RXBuffer[UART_BUFFER_SIZE];				// Ringpuffer- Array f�r empfangene	UART Daten (UART-Puffer)
extern volatile unsigned char UART_RxCount ;								// Z�hlervariable f�r UART-Puffer

extern void init_uart();													// Funktion in RS232.c zum Initalisieren und Aktivieren des UART mit oben eingestellten Werten
extern int uart_putc (unsigned char c);										// Funktion in RS232.c zum Senden von Bytes
extern void uart_puts (char *s);											// Funktion in RS232.c zum Senden von Strings
extern void uart_puts_p (const char *progmem_s );							// Funktion in RS232.c zum Senden von Strings aus dem Flash-Memory
extern void uart_puti (const int val);										// Funktion in RS232.c zum Senden von Zahlen

#endif /* RS232_H_ */
