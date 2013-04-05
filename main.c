/*
___________________________________________________________
----------------   Pin Assignment   -----------------------
___________________________________________________________

PB0		LED indication for Accept/Reject (A buzzer is connected in parallel as well)
PB1		Output to the Solenoid

PB2		Button to Add a Tag		//Not implemented yet
PB3		Button to remove a Tag	//Not implemented yet

=============================================================
*/

#include <avr/io.h>
#include <USART.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))


#define cTrue	1
#define cFalse	0

#define TagLength	16
#define NoTags		5





//Define functions
//==============================================================
void init_sys(void);      // initializes IO

uint8_t MatchTagID(char *, char *);	//Compare two strings
uint8_t MatchToTable (char * Tag);		//Compare the string to the table of IDs and find the match
void AcceptTag(void);					//Function to execute if the ID is in the table
void RejectTag(void);					//Function to execute if the ID is NOT in the table








//==============================================================
//		Global Variables



char rTag[TagLength];	//Received Tag
uint8_t iChar=0;		//Char index for writing to char array




//Array of "Accepted" tags
char Tags[NoTags][TagLength] =	{	"50003E7AB0A4",
									"50003E3AA3F7",
									"50003E8D00E3",
									"67005D6C3660",	//ButtonRFID
									"67005DB13FB4"	//Second Button RFID
								};


//==============================================================

// USART Interrupt Service Routine
ISR(USART_RX_vect)
{
	if (iChar < TagLength)
	{
		rTag[iChar]=UDR0;
		iChar++;
	}
}





int main (void)
{

	init_sys();	//IO initilization function
	
	UCSR0B |= (1 << RXCIE0);	//Enable USART Rx Interrupt
	sei();	//Enable global interrupts
	
	
	for(;;)
	{
		
		if (iChar == TagLength)	//If TagLength Characters has been read by the interrupt function
		{
			
			cli();	//stop the interrupts
			
			if (MatchToTable(rTag))	//Check if the read ID matches one of the stored IDs
				AcceptTag();
			else
				RejectTag();
			
			iChar=0;		//Reset the number of read characters
			USART_Flush();	//Flush USART
			
			sei();	// Re-enable inturrpts
		}
		else
			asm("nop");	//TO prevent MCU from hanging (to get my point try removing the else statement)
						//I found that When <if> inside the main loop function is not satisfied, the MCU "Hangs"
						//Adding this line solves the problem

	}
	
	return(0);
}






void init_sys(void)
{
	sbi(DDRB,PB0);
	sbi(DDRB,PB1);
	init_USART(9600);
}


// According to the Datasheet of the ID-20 RFID Reader
//Output Data Structure – ASCII
//STX (02h) DATA (10 ASCII) CHECK SUM (2 ASCII) CR LF ETX (03h) 
//[The 1byte (2 ASCII characters) Check sum is the “Exclusive OR” of the 5 hex bytes (10 ASCII) Data characters.]
// *note: I am making it over simplistic by read the 1 byte checksum as part of the tag

uint8_t MatchTagID(char * Tag1, char * Tag2)
{

	if ( (Tag1[0] == 0x02) && (Tag1[13]=0x0D) && (Tag1[14]=0x0A) && (Tag1[15]=0x03) ) //This verifies that ...
	{	// ... the received code matches the expected data format from the RFID tags, if so, then compare the ...
		// ... actual tag
		
		for (uint8_t i=0; i<TagLength-3; i++) 	//-3 here because it will not find the last three bytes (CR LF ETX)
			if (Tag1[i+1] != Tag2[i])			// +1 here to bypass the STX byte in the beginined of the received tag ID
				return cFalse;
		
		return cTrue;
	}
	else
		return cFalse;
}






uint8_t MatchToTable (char * Tag)
{


	for (uint8_t i=0; i< NoTags; i++)
	{
		if ( MatchTagID(Tag, Tags[i]))
			return cTrue;
	}
	
	return cFalse;

}


void AcceptTag(void)
{

	sbi(PORTB, PB0);
	sbi(PORTB, PB1);
	_delay_ms(500);
	cbi(PORTB, PB0);
	_delay_ms(5000);
	cbi(PORTB,PB1);

}


void RejectTag(void)
{

	for (uint8_t i=0; i<3; i++)
	{
		sbi(PORTB, PB0);
		_delay_ms(200);
		cbi(PORTB, PB0);
		_delay_ms(100);
	}
}



