// I2C-Master-Routinen von Peter Fleury verwenden
// siehe http://homepage.hispeed.ch/peterfleury/avr-software.html#libs
// Hier sind auch die Dateien: i2cmaster.h und twimaster.c zu finden, die ben�tigt werden
// Letztes Update des Codes 5. April 2010 durch HannoHupmann

#include <avr/io.h>
#include <avr/interrupt.h>																	// Standard-Headerdatei(avr-libc): Funktionen in diesem Modul erm�glichen das Handhaben von Interrupts
#include <avr/pgmspace.h>
#include "global.h"
#include "rs232.h"																			// Headerdatei f�r rs232.c
#include "TWI_Master.h"
#define SLAVE_ADRESSE 0x50

const char FlashString[] PROGMEM = ("MASTER gestartet, bitte Daten gefolgt von Enter eingeben"CR);


int main(void)
{
	init_uart();																			// UART initalisieren und einschalten. Alle n�tigen Schnittstellen-Parameter und -Funktionen werden in rs232.h definiert
	i2c_init();																				// init I2C interface
	sei();
	uart_puts_p(FlashString);																// Demonstriert "rs232.c/uarts_put_p" f�r die Ausgabe eines Strings

	while(1)																				// Main- Endlosschleife
    {
		if (UART_MSG_FLAG==1)																// UART_MSG_FLAG auswerten: gesetzt in Empfangs- Interruptroutine wenn "CR" empfangen oder UART- Puffer voll
		{
			// (1) Daten an RS232 zur�ck
			uart_puts ("Folgende Daten an RS232 empfangen: ");
			uint8_t i;																		// Hilfsvariable f�r For-Next-Schleifenz�hler
			for (i=0;i<UART_RxCount;i++) {uart_putc(UART_RXBuffer[i]);}						// Empfangene Daten aus dem Puffer wieder �ber UART senden
			uart_puts(CR);
			// (2) Daten an I2C- Slave senden im MT-Mode
			if(!(i2c_start(SLAVE_ADRESSE+I2C_WRITE))) //Slave bereit zum schreiben?
				{
					uart_puts ("Daten: ");
					i2c_write(0x00);  // Buffer Startadresse setzen

					for (i=0;i<UART_RxCount;i++)											// Empfangene Daten in Array schreiben
						{
							i2c_write(UART_RXBuffer[i]);									// Bytes schreiben...
							uart_putc(UART_RXBuffer[i]);
						}
					i2c_stop();       // Zugriff beenden
					uart_puts (" an Slave gesendet."CR);
				}
			else
				{
					/* Hier k�nnte eine Fehlermeldung ausgegeben werden... */
					uart_puts("Fehler beim senden der Daten an Slave"CR);
				}
			// (3) Daten von I2C- Slave lesen im MR-Mode
			if(!(i2c_start(SLAVE_ADRESSE+I2C_WRITE))) //Slave bereit zum lesen?
				{
					uart_puts("Daten von Slave lesen: ");
					i2c_write(0x00); //Buffer Startadresse zum Auslesen
					i2c_rep_start(SLAVE_ADRESSE+I2C_READ); //Lesen beginnen

					for (i=0;i<UART_RxCount;i++)											// Empfangene Daten in Array schreiben
						{
							if ((i) < UART_RxCount-1)
							{
								TWI_RXBuffer[i]=i2c_readAck();									// Bytes schreiben...
								uart_putc(TWI_RXBuffer[i]);
							}
							else
							{
								TWI_RXBuffer[i]=i2c_readNak();									// letztes Bytes schreiben...
								uart_putc(TWI_RXBuffer[i]);
							}

						}

					i2c_stop();																// Zugriff beenden
					uart_puts(CR);
				}
			else
				{
					/* Hier k�nnte eine Fehlermeldung ausgegeben werden... */
					uart_puts("Fehler beim lesen der Daten an Slave"CR);
				}

			UART_RxCount=0;
			UART_MSG_FLAG=0;
		}

	}
}
