#include <msp430.h> 


/**
 * main.c
 */


unsigned int status = 1; //1->8 === off->max
unsigned int prevStatus = 1;

int setpoint[9] = {0, 0, 860, 0, 950,  0, 0, 0, 1015}; //quick access setpoint array thing

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	DCOCTL = 0;  // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ; // Set range
	DCOCTL = CALDCO_16MHZ; // Set DCO step + modulation*/

	P1DIR |= 1<<6; // set PWM output pins
	P1DIR |= 0b1111<<2; //set LED output pins
	P2DIR = 0;
	P2REN = 0xFF; //enable pulldowns on all pins on P2
	P2OUT = 0;
	P1OUT = prevStatus << 2;

	ADC10CTL0 = ADC10SHT_2 + ADC10ON + SREF_1 + MSC + REF2_5V + REFON; // ADC10ON, select internal reference
	ADC10CTL1 = INCH_7 + CONSEQ_2 + ADC10SSEL_2; //choose channel, set clock source and set continuous conversion
	ADC10AE0 = 1<<7; //configure A7 analog input
//	__enable_interrupt();
	ADC10CTL0 |= ENC + ADC10SC; //enable conversion immediately

	__delay_cycles(10000); //wait for voltage to stabilize



	while(1){
	    //poll buttons and set status
	    if (P2IN & 0b1111){
	        status = P2IN & 0b1111;
	        if (status != prevStatus){
                prevStatus = status;
                P1OUT &= !(0b1111<<2);
                P1OUT |= prevStatus << 2;
            }
	    }


//	    else {
//
//	    }



//	    if(prevStatus > 1){
//	    if (ADC10CTL1 & 1){
//	        P1OUT |= 1<<6;
//            __delay_cycles(70);
//            P1OUT &= ~(1<<6);

//	    }
	        if(ADC10MEM < setpoint[prevStatus]){ //ADC reads that setpoint is low, provide a pulse, less than so if the setpoint is 0 it will definitely not trigger
                P1OUT |= 1<<6;
                __delay_cycles(70);
                P1OUT &= ~(1<<6);
            }
//	    }
	    else {
	        P1OUT &= ~(1<<6);
	    }



	}


	
	return 0;
}

// ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    P1OUT ^= 1<<6;
}
