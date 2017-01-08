#include <msp430.h> 
#include <stdint.h>
#include "KBD_HUT.h"

void printf(char *format, ...);
void pressKey(uint8_t keyCode);
void releaseKey(uint8_t keyCode);
void sendKbdReport(void);
void sendMouseReport(void);
void buildMouseReport(void);

typedef struct {
	uint8_t buttons;
	uint8_t dX;
	uint8_t dY;
	uint8_t dZ;
} MOUSE_REPORT;

uint8_t index = 1;    
MOUSE_REPORT mouseReport = { 0, 0, 0, 0 };          // HID report, to be sent to the PC.

const int8_t tableSinCosLookUp[16][2]; 

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	//setup UART
	P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	// 1000000 Hz  57600 bps
	UCA0BR0 = 0x11;
	UCA0BR1 = 0x00;
	UCA0MCTL = UCBRS_3 + UCBRF_0;
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

	//setup LED
	P1OUT = 0;
	P1DIR |= BIT0;
	//setup button
	P1DIR &= ~BIT3;
	P1OUT |= BIT3; //pull-up, active low
	P1REN |= BIT3;

	while(1) {
		if((P1IN & BIT3) == BIT3) {
			releaseKey(hutSpacebar);
			P1OUT &= ~BIT0;
		} else {
			__delay_cycles(1000);
			if((P1IN & BIT3) != BIT3) {
				P1OUT |= BIT0;
				pressKey(hutSpacebar);

//				buildMouseReport();
//				sendMouseReport();
			}
		}
		sendKbdReport();
	}
}

void buildMouseReport(void) {
	mouseReport.dX =
			(tableSinCosLookUp[index][0] -
					tableSinCosLookUp[index - 1][0]) >> 1;
	mouseReport.dY =
			(tableSinCosLookUp[index][1] -
					tableSinCosLookUp[index - 1][1]) >> 1;

	if (index++ >= 90){
		index = 1;
	}
}

void pressKey(uint8_t keyCode) {
	printf("KD%c\r\n", keyCode); //send keycode as a character
}

void releaseKey(uint8_t keyCode) {
	printf("KU%c\r\n", keyCode); //send keycode as a character
}

void sendKbdReport(void) {
	printf("KUPDATE\r\n");
}

void sendMouseReport(void) {
	printf("M%c%c%c%c\r\n",mouseReport.buttons,mouseReport.dX,mouseReport.dY,mouseReport.dZ);
}

const int8_t tableSinCosLookUp[16][2] = {
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		4,0,
		-4,0,
		-4,0,
		-4,0,
		-4,0,
		-4,0,
		-4,0,
		-4,0,
		-4,0
};
